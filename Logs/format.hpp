#ifndef __M_FMT_H__
#define __M_FMT_H__

#include <memory>
#include <vector>
#include <tuple>

#include "util.hpp"
#include "message.hpp"
#include "level.hpp"

namespace popolog{

    //抽象格式化子项基类
    class FormatItem{
        public:
            using ptr = std::shared_ptr<FormatItem>;
            virtual void format(std::ostream &out,LogMsg &msg) = 0;
    };

    //派生格式化子项子类
    //消息
    class MsgFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << msg._payload;
            }
    };
    //等级
    class LevelFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << LogLevel::tostring(msg._level);
            }
    };
    //时间
    class TimeFormatItem : public FormatItem{
        public:
            TimeFormatItem(const std::string &fmt = "%H:%M:%S"): _time_fmt(fmt){}
            void format (std::ostream &out,LogMsg &msg) override{
                struct tm t;
                localtime_r(&msg._ctime,&t);
                char tmp[32] = {0};
                strftime(tmp,31,_time_fmt.c_str(),&t);
                out << tmp;
            }
        private:
            std::string _time_fmt;
    };
    //文件名
    class FileFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << msg._file;
            }
    };
    //行号
    class NLineFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << msg._line;
            }
    };
    //线程ID
    class ThreadFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << msg._tid;
            }
    };
    //日志器名称
    class LoggerFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << msg._logger;
            }
    };
    //制表符
    class TabFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << "\t";
            }
    };
    //换行
    class N_LineFormatItem : public FormatItem{
        public:
            void format (std::ostream &out,LogMsg &msg) override{
                out << "\n";
            }
    };
    //其它
    class OtherFormatItem : public FormatItem{
        public:
            OtherFormatItem(const std::string &str): _str(str){}
            void format (std::ostream &out,LogMsg &msg) override{
                out << _str;
            }
        private:
            std::string _str;
    };
    //等级


}

#endif