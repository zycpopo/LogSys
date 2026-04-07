/*日志落地模块的实现*/
#include "util.hpp"

#include <memory>
#include <fstream>
#include <cassert>
#include <sstream>


namespace popolog
{
    class LogSink{
        public:
            using ptr = std::shared_ptr<LogSink>;
            LogSink(){}
            virtual ~LogSink() {}
            virtual void log(const char *data,size_t len) = 0;
    };

    //输出到屏幕
    class StdoutSink : public LogSink{
        public:
            void log(const char *data,size_t len)
            {
                std::cout.write(data,len);
            }
    };

    //输出到文件
    class FileSink :public LogSink{
        public:
            //构造时传入文件名，并打开文件，将操作句柄管理起来
            FileSink(const std::string &pathname):_pathname(pathname)
            {
                //1.创建目录
                util::File::createDirectory(util::File::path(pathname));
                //2.创建并打开日志文件
                _ofs.open(_pathname,std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }
            void log(const char *data,size_t len)
            {
                _ofs.write(data,len);
                assert(_ofs.good());
            }
        private:
            std::string _pathname;
            std::ofstream _ofs;
    };
    
    //输出到滚动文件（以大小进行滚动）
    class RollyBySizeSink :public LogSink{
        public:
            //构造时传入文件名，并打开文件，将操作句柄管理起来
            RollyBySizeSink(const std::string &basename,size_t max_size):
                _basename(basename),max_fsize(max_size),cur_fsize(0)
            {
                std::string pathname = createNewFile();
                util::File::createDirectory(util::File::path(pathname));
                _ofs.open(pathname,std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }
            //将日志信息写入到标准输出中，写之前判断一下有没有超过最大文件限制
            void log(const char *data,size_t len)
            {
                if(cur_fsize >= max_fsize)
                {
                    _ofs.close();
                    std::string pathname = createNewFile();
                    _ofs.open(pathname,std::ios::binary | std::ios::app);
                    assert(_ofs.is_open());
                }
                _ofs.write(data,len);
                assert(_ofs.good());
            }
        private:
            std::string  createNewFile()
            {
                time_t t = util::Date::getTime();
                struct tm lt;
                localtime_r(&t, &lt);
                std::stringstream filename;
                filename << _basename;
                filename << lt.tm_year;
                filename << lt.tm_mon;
                filename << lt.tm_mday;
                filename << lt.tm_hour;
                filename << lt.tm_min;
                filename << lt.tm_sec;
                filename << ".log";

                return filename.str();

            }
        private:
            //通过基础文件名 + 扩展文件名（以时间生成）组成一个实际的当前输出文件名
            std::string _basename;
            std::ofstream _ofs;
            size_t max_fsize;
            size_t cur_fsize;
    };
    
}