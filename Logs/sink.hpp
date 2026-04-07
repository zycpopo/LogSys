/*日志落地模块的实现*/
#include "util.hpp"
#include <memory>
#include <fstream>

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
            void log(const char *data,size_t len);
    };

    //输出到文件
    class FileSink :public LogSink{
        public:
            //构造时传入文件名，并打开文件，将操作句柄管理起来
            FileSink(const std::string &pathname);
            void log(const char *data,size_t len);
        private:
            std::string _filename;
            std::ofstream _ofs;
    };
    
    //输出到滚动文件（以大小进行滚动）
    class RollyBySizeSink :public LogSink{
        public:
            //构造时传入文件名，并打开文件，将操作句柄管理起来
            RollyBySizeSink(const std::string &basename,size_t max_size);
            //将日志信息写入到标准输出中，写之前判断一下有没有超过最大文件限制
            void log(const char *data,size_t len);
        private:
            void createNewFile();
        private:
            //通过基础文件名 + 扩展文件名（以时间生成）组成一个实际的当前输出文件名
            std::string _basename;
            std::ofstream _ofs;
            size_t max_fsize;
            size_t cur_fsize;
    };
    
}