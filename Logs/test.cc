#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"


int main()
{
    popolog::LogMsg msg(popolog::LogLevel::value::INFO,53,"main.c","root","格式化功能测试"); 
    popolog::Formatter fmt("abc%% abc[%d{%H:%M:%S}]%m%n%g");
    std::string str = fmt.format(msg);
    std::cout << str << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::DEBUG) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::INFO) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::WARN) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::ERROR) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::FATAL) << std::endl;
    //std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::OFF) << std::endl;
}