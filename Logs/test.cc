#include "util.hpp"
#include "level.hpp"


int main()
{
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::DEBUG) << std::endl;
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::INFO) << std::endl;
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::WARN) << std::endl;
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::ERROR) << std::endl;
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::FATAL) << std::endl;
    std::cout << popolog::LogLevel::tostring(popolog::LogLevel::value::OFF) << std::endl;
}