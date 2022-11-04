#pragma once

class Subject 
{ 
    public: 
    virtual ~Subject(); 
    virtual void Request() = 0; 
    protected: 
    Subject();
    private: 
}; 

class ConcreteSubject:public Subject 
{ 
    public: 
    ConcreteSubject();
    ~ConcreteSubject(); 
    void Request() override; 
    protected: 
    private: 
}; 

class Proxy 
{ 
    public: 
    Proxy(); 
    explicit Proxy(Subject* sub);
    ~Proxy(); 
    void Request(); 
    protected: 
    
    private: 
    Subject* _sub;
};