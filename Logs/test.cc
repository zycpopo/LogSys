#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "logger.hpp"


int main()
{
    // std::string logger_name = "sync_logger";
    // popolog::LogLevel::value limit = popolog::LogLevel::value::WARN;
    // popolog::Formatter::ptr fmt(new popolog::Formatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"));
    // popolog::LogSink::ptr stdout_lsp = popolog::SinkFactory::create<popolog::StdoutSink>();
    // popolog::LogSink::ptr file_lsp = popolog::SinkFactory::create<popolog::FileSink>("./logfile/test.log");
    // popolog::LogSink::ptr roll_lsp = popolog::SinkFactory::create<popolog::RollyBySizeSink>("./logfile/roll-",1024*1024);

    // std::vector<popolog::LogSink::ptr> sinks = {stdout_lsp,file_lsp,roll_lsp};
    // popolog::Logger::ptr logger (new popolog::SyncLogger(logger_name,limit,fmt,sinks));

    std::unique_ptr<popolog::LoggerBuilder> builder(new popolog::LocalLoggerBuilder());
    builder->buildLoggerName("sync_logger"); 
    builder->buildLoggerLevel(popolog::LogLevel::value::WARN);
    builder->buildFormatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    builder->buildLoggerType(popolog::LoggerType::LOGGER_SYNC);
    builder->buildSink<popolog::FileSink>("./logfile/test.log");
    builder->buildSink<popolog::StdoutSink>();
    popolog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__,__LINE__,"%s","popo的测试日志");
    logger->info(__FILE__,__LINE__,"%s","popo的测试日志");
    logger->warn(__FILE__,__LINE__,"%s","popo的测试日志");
    logger->error(__FILE__,__LINE__,"%s","popo的测试日志");
    logger->fatal(__FILE__,__LINE__,"%s","popo的测试日志");
    
    int cursize = 0,count=0;
    while(cursize < 1024*1024*10)
    {
        logger->fatal(__FILE__,__LINE__,"测试日志-%d",count++);
        cursize +=30;
    }


    // popolog::LogMsg msg(popolog::LogLevel::value::INFO,53,"main.c","root","格式化功能测试"); 
    // popolog::Formatter fmt;
    // std::string str = fmt.format(msg);

    // popolog::LogSink::ptr stdout_lsp = popolog::SinkFactory::create<popolog::StdoutSink>();
    // popolog::LogSink::ptr file_lsp = popolog::SinkFactory::create<popolog::FileSink>("./logfile/test.log");
    // popolog::LogSink::ptr roll_lsp = popolog::SinkFactory::create<popolog::RollyBySizeSink>("./logfile/roll-",1024*1024);

    // stdout_lsp->log(str.c_str(),str.size());
    // file_lsp->log(str.c_str(),str.size());

    // size_t count = 0;
    // size_t cursize = 0;
    // while(cursize < 1024*1024*10)
    // {
    //     std::string tmp=str + std::to_string(count++);
    //     roll_lsp->log(tmp.c_str(),tmp.size());
    //     cursize += tmp.size();
    // }

    //std::cout << str << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::DEBUG) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::INFO) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::WARN) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::ERROR) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::FATAL) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::OFF) << std::endl;

    return 0;
}