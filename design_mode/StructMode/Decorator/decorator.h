#pragma once

class Component 
{ 
    public: 
    virtual ~Component(); 
    virtual void Operation(); 
    protected: 
    Component(); 
    private: 
}; 

class ConcreteComponent:public Component 
{ 
    public: 
    ConcreteComponent(); 
    ~ConcreteComponent(); 
    void Operation() override; 
    protected: 
    private: 
}; 

class Decorator:public Component // Í¬Ñù¼Ì³Ð×Ô Component
{ 
    public: 
    explicit Decorator(Component* com); 
    virtual ~Decorator(); 
    void Operation() override; 
    protected: 
    Component* _com;
    private: 
}; 

class ConcreteDecorator:public Decorator
{ 
    public: 
    explicit ConcreteDecorator(Component* com);
    ~ConcreteDecorator(); 
    void Operation() override; 
    void AddedBehavior(); 
    protected: 
    private: 
};