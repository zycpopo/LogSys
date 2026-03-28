#include <iostream>

template<typename T>
void cpprintf2(const T &v)//单个参数时直接打印
{
    std::cout << v << std::endl;
}

template<typename T, typename ...Args>//类型参数包
void cpprintf2(const T &v,const Args& ...args)
//值参数包，这里...是声明args是一个值参数包，const Args&是一个类型模式。
//同时这个 ... 也触发了类型模式的展开：把 const Args& 应用于 Args 的每个类型。
{
    std::cout << v << ' '; 
    cpprintf2(args...);//这里的...是展开符，对args进行展开，并触发传递归操作。
}

int main()
{
    cpprintf2("ziyangcon");
    cpprintf2("紫洋葱", 20060309);
    cpprintf2("huichengpiao", 221023, 251213);
}