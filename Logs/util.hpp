//实用工具类的实现
//1.获取系统时间
//2.判断文件是否存在
//3.获取文件所在路径
//4.创建目录

#ifndef __MY_UTIL_H__
#define __MY_UTIL_H__

#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

namespace popolog
{
    namespace util
    {
        class Date{
            public:
                static size_t getTime(){
                    return (size_t)time(nullptr);
                }
        };
        class File{
            public:
                static bool exists(const std:: string &pathname)//2
                {
                    struct stat st;
                    if(stat(pathname.c_str(),&st) < 0)
                    {
                        return false;
                    }
                    return true;
                }
                static std::string path(const std::string &name)//3
                {
                    if (name.empty()) return ".";
                    size_t pos = name.find_last_of("/\\");
                    if (pos == std::string::npos) return ".";
                    return name.substr(0, pos + 1);
                }
                static void createDirectory(const std::string &path)//4
                {
                    if (path.empty()) return ;
                    if (exists(path)) return ;
                    size_t pos, idx = 0;
                    while(idx < path.size()) {
                        pos = path.find_first_of("/\\", idx);
                        if (pos == std::string::npos) {
                            mkdir(path.c_str(), 0755);
                            return;
                        }
                        if (pos == idx) {idx = pos + 1; continue;}
                        std::string subdir = path.substr(0, pos);
                        if (subdir == "." || subdir == "..") {idx = pos + 1; continue;}
                        if (exists(subdir)) {idx = pos + 1; continue;}
                        mkdir(subdir.c_str(), 0755);
                        idx = pos + 1;
                    }
                }
        };
    }
}

#endif