#include<iostream>
#include<string>
#include<memory>

//1.抽象产品
class phone
{
public:
    virtual void show() = 0;
    virtual ~phone() {}
};

//2.具体产品
class Apple :public phone{
public:
    void show() override
    {
        std::cout << "Apple手机来了" << std::endl;
    }
};

class Xiaomi :public phone{
public:
    void show() override{
        std::cout << "Xiaomi手机来了" << std::endl;
    }
};

//3.工厂类
class phoneFactory{
public:
    static std::unique_ptr<phone> createPhone(const std::string& brand)
    {
        if(brand == "Apple")
        {
            return std::make_unique<Apple>();
        }
        else if(brand == "Xiaomi")
        {
            return std::make_unique<Xiaomi>();
        }

        return nullptr;
    }
};

int main()
{

    //客户端只需要和工厂打交道，不需要和phone这个类打交道
    auto p1 = phoneFactory::createPhone("Apple");
    if(p1) p1->show();

    auto p2 = phoneFactory::createPhone("Xiaomi");
    if(p2)  p2->show();

    return 0;
}

//本代码以一个具体的简单工厂实例，演示了简单工厂的结构和工作流程。
//这种工厂模式核心思想就是定义一个工厂类，根据传入的参数，决定创建哪一种产品类的实例。
//优点：封装性好，客户端不需要知道具体的产品类。
//缺点：违背了“开闭原则”，如果想增加一种具体产品类，就必须修改phoneFactory这个类的源码。
//      工厂的职责任务过重，只能承担少量的产品，产品多了这个工厂类就太冗余臃肿的。