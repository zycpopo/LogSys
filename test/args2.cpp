#include <iostream>

void cpprintf()
{
    std::cout << std::endl;
}

template<typename T, typename ...Args>
void cpprintf(const T & v,Args && ...args)
{
    std::cout<< v;
    if((sizeof ...(args)) > 0)
    {
        cpprintf(std::forward<Args>(args)...);
    }
    else{
        cpprintf();
    }
}

int main()
{
    cpprintf("ziyangcon");
    cpprintf("紫洋葱",20060309);
    cpprintf("huichengpiao",221023,251213);
}