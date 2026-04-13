#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "logger.hpp"
#include "buffer.hpp"


int main()
{
   // 创建测试文件
    std::ofstream outFile("test_input.txt");
    outFile << "This is a test file.\nIt has multiple lines.\nAnd some content to verify.";
    outFile.close();
    
    // 读取文件并写入缓冲区
    std::ifstream inFile("test_input.txt");
    popolog::Buffer buffer;
    
    char chunk[10];  // 每次读取10个字节
    while (inFile.read(chunk, sizeof(chunk)) || inFile.gcount() > 0) {
        size_t bytesRead = inFile.gcount();
        buffer.push(chunk, bytesRead);
    }
    inFile.close();
    
    // 将缓冲区数据写入新文件
    std::ofstream outFile2("test_output.txt");
    outFile2.write(buffer.begin(), buffer.readAbleSize());
    outFile2.close();
    
    // 比较两个文件
    std::ifstream file1("test_input.txt");
    std::ifstream file2("test_output.txt");
    
    bool isEqual = (file1.tellg() == file2.tellg());  // 比较大小
    
    if (isEqual) {
        std::string content1, content2;
        file1.seekg(0); file2.seekg(0);
        getline(file1, content1, '\0');  // 读取全部内容
        getline(file2, content2, '\0');
        isEqual = (content1 == content2);
    }
    
    file1.close();
    file2.close();
    
    std::cout << "Files are " << (isEqual ? "identical" : "different") << std::endl;
    
    return isEqual ? 0 : 1;





    // std::string logger_name = "sync_logger";
    // popolog::LogLevel::value limit = popolog::LogLevel::value::WARN;
    // popolog::Formatter::ptr fmt(new popolog::Formatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"));
    // popolog::LogSink::ptr stdout_lsp = popolog::SinkFactory::create<popolog::StdoutSink>();
    // popolog::LogSink::ptr file_lsp = popolog::SinkFactory::create<popolog::FileSink>("./logfile/test.log");
    // popolog::LogSink::ptr roll_lsp = popolog::SinkFactory::create<popolog::RollyBySizeSink>("./logfile/roll-",1024*1024);

    // std::vector<popolog::LogSink::ptr> sinks = {stdout_lsp,file_lsp,roll_lsp};
    // popolog::Logger::ptr logger (new popolog::SyncLogger(logger_name,limit,fmt,sinks));

    // std::unique_ptr<popolog::LoggerBuilder> builder(new popolog::LocalLoggerBuilder());
    // builder->buildLoggerName("sync_logger"); 
    // builder->buildLoggerLevel(popolog::LogLevel::value::WARN);
    // builder->buildFormatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    // builder->buildLoggerType(popolog::LoggerType::LOGGER_SYNC);
    // builder->buildSink<popolog::FileSink>("./logfile/test.log");
    // builder->buildSink<popolog::StdoutSink>();
    // popolog::Logger::ptr logger = builder->build();

    // logger->debug(__FILE__,__LINE__,"%s","popo的测试日志");
    // logger->info(__FILE__,__LINE__,"%s","popo的测试日志");
    // logger->warn(__FILE__,__LINE__,"%s","popo的测试日志");
    // logger->error(__FILE__,__LINE__,"%s","popo的测试日志");
    // logger->fatal(__FILE__,__LINE__,"%s","popo的测试日志");
    
    // int cursize = 0,count=0;
    // while(cursize < 1024*1024*10)
    // {
    //     logger->fatal(__FILE__,__LINE__,"测试日志-%d",count++);
    //     cursize +=30;
    // }


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