#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"


int main()
{
    popolog::LogMsg msg(popolog::LogLevel::value::INFO,53,"main.c","root","格式化功能测试"); 
    popolog::Formatter fmt;
    std::string str = fmt.format(msg);

    popolog::LogSink::ptr stdout_lsp = popolog::SinkFactory::create<popolog::StdoutSink>();
    popolog::LogSink::ptr file_lsp = popolog::SinkFactory::create<popolog::FileSink>("./logfile/test.log");
    popolog::LogSink::ptr roll_lsp = popolog::SinkFactory::create<popolog::RollyBySizeSink>("./logfile/roll-",1024*1024);

    stdout_lsp->log(str.c_str(),str.size());
    file_lsp->log(str.c_str(),str.size());

    size_t count = 0;
    size_t cursize = 0;
    while(cursize < 1024*1024*10)
    {
        std::string tmp=str + std::to_string(count++);
        roll_lsp->log(tmp.c_str(),tmp.size());
        cursize += tmp.size();
    }

    //std::cout << str << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::DEBUG) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::INFO) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::WARN) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::ERROR) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::FATAL) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::OFF) << std::endl;

    return 0;
}