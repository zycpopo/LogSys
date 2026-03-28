#include <iostream>

//饿汉式（Eager Initialization)
/*class singleton 
{
private:
    static singleton* instance;
    singleton() {}
public:
    static singleton* getInstance(){
        return instance;
    }
};
//类外初始化
singleton* singleton::instance = new singleton();*/


//现代c++安全写法
class singleton2
{
public:
    static singleton2& getInstance()
    {
        static singleton2 instance;//性能非常好
        //保证了线程安全，还有就是一旦初始化完成，标记位就会变成1，后续的getInstance的操作就直接返回已经初始化成功的实例
        return instance;
    }
private:
    singleton2(){}
    //禁止拷贝和赋值
    singleton2(const singleton2&) = delete;
    singleton2 operator=(const singleton2&) = delete;
};