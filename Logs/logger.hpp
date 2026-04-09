/*日志器模块*/
#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "util.hpp"
#include "level.hpp"
#include "format.hpp"
#include "sink.hpp"

#include <atomic>
#include <mutex>
#include <cstdarg>

namespace popolog{
    class Logger{
        public:
            using ptr = std::shared_ptr<Logger>;
            Logger(const std::string &logger_name,
                LogLevel::value level,
                Formatter::ptr &formatter,
                std::vector<LogSink::ptr> &sinks):
                _logger_name(logger_name),
                _limit_level(level),
                _formatter(formatter),
                _sinks(sinks.begin(),sinks.end()){}

            //完成构造日志消息对象并进行格式化，得到格式化后的日志消息字符串，然后进行落地输出
            void debug(const std::string &file,size_t line,const std::string &fmt,...)
            {
                //1.判断当前的日志是否达到了输出等级
                if(LogLevel::value::DEBUG < _limit_level){return ;}
                va_list ap;
                va_start(ap,fmt);
                char *res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout << "vasprintf failed !\n";
                    return;
                }
                va_end(ap);
                serialize(LogLevel::value::DEBUG,file,line,res);
                free(res);
            }
            void info(const std::string &file,size_t line,const std::string &fmt,...)
            {
                //1.判断当前的日志是否达到了输出等级
                if(LogLevel::value::INFO < _limit_level){return ;}
                va_list ap;
                va_start(ap,fmt);
                char *res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout << "vasprintf failed !\n";
                    return;
                }
                va_end(ap);
                serialize(LogLevel::value::INFO,file,line,res);
                free(res);
            }
            void warn(const std::string &file,size_t line,const std::string &fmt,...)
            {
                //1.判断当前的日志是否达到了输出等级
                if(LogLevel::value::WARN< _limit_level){return ;}
                va_list ap;
                va_start(ap,fmt);
                char *res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout << "vasprintf failed !\n";
                    return;
                }
                va_end(ap);
                serialize(LogLevel::value::WARN,file,line,res);
                free(res);
            }
            void error(const std::string &file,size_t line,const std::string &fmt,...)
            {
                //1.判断当前的日志是否达到了输出等级
                if(LogLevel::value::ERROR < _limit_level){return ;}
                va_list ap;
                va_start(ap,fmt);
                char *res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout << "vasprintf failed !\n";
                    return;
                }
                va_end(ap);
                serialize(LogLevel::value::ERROR,file,line,res);
                free(res);
            }
            void fatal(const std::string &file,size_t line,const std::string &fmt,...)
            {
                //1.判断当前的日志是否达到了输出等级
                if(LogLevel::value::FATAL < _limit_level){return ;}
                va_list ap;
                va_start(ap,fmt);
                char *res;
                int ret = vasprintf(&res,fmt.c_str(),ap);
                if(ret == -1)
                {
                    std::cout << "vasprintf failed !\n";
                    return;
                }
                va_end(ap);
                serialize(LogLevel::value::FATAL,file,line,res);
                free(res);
            }
        private:
            void serialize(LogLevel::value level,const std::string &file,size_t line,char *str)
            {
                LogMsg msg(level,line,file,_logger_name,str);
                //4.通过格式化工具对msg进行格式化，得到格式化后的日志字符串
                std::stringstream ss;
                _formatter->format(ss,msg);
                //5.进行日志落地
                log(ss.str().c_str(),ss.str().size());
            }    

            //抽象接口完成落地输出，根据不同的日志器会有不同的实际落地方式
            virtual void log(const char *data,size_t len) = 0;
        protected:
            std::mutex _mutex;
            std::string _logger_name;
            std::atomic<LogLevel::value> _limit_level;
            Formatter::ptr _formatter;
            std::vector<LogSink::ptr> _sinks;
    };

    class SynLogger :public Logger
    {
        public:
            SynLogger(const std::string &logger_name,
                LogLevel::value level,
                Formatter::ptr &formatter,
                std::vector<LogSink::ptr> &sinks):
                Logger(logger_name,level,formatter,sinks){}
        protected:
            void log(const char *data,size_t len)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if(_sinks.empty()) return;
                for(auto &sink : _sinks)
                {
                    sink->log(data,len);
                }
            }
    };  
}

#endif