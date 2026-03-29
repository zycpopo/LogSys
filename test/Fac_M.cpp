#include <iostream>

class Fruit {
public:
    virtual void eat() = 0; 
};

// 具体产品：苹果和香蕉
class Apple : public Fruit {
public:
    void eat() override { std::cout << "吃掉一个苹果🍎" << std::endl; }
};

class Banana : public Fruit {
public:
    void eat() override { std::cout << "吃掉一根香蕉🍌" << std::endl; }
};

//抽象工厂：规定所有工厂都必须能“生产水果” 
class Factory {
public:
    virtual Fruit* create() = 0; // 这就是“工厂方法”
};

//具体工厂：专门造苹果的工厂 
class AppleFactory : public Factory {
public:
    Fruit* create() override { return new Apple(); }
};

// 专门造香蕉的工厂
class BananaFactory : public Factory {
public:
    Fruit* create() override { return new Banana(); }
};

int main()
{
    Factory* myfactory = new AppleFactory();
    Fruit* myfruit = myfactory->create();

    myfruit->eat();
}

//工厂方法模式虽然解决了扩展性的问题，完美符合开闭原则。
//但是有一个局限，就是一个工厂只能生产一种产品，每增加一个产品就需要多写一个类。