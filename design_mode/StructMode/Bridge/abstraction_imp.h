#pragma once
#include "../../include/global_declare.h"

class AbstractionImp
{ 
public: 
 virtual ~AbstractionImp() {}
 virtual void Operation() = 0;
protected: 
 AbstractionImp(); 
private: 
}; 
class ConcreteAbstractionImpA:public AbstractionImp
{ 
public: 
 ConcreteAbstractionImpA() {}
 ~ConcreteAbstractionImpA(){} 
 virtual void Operation(); 
protected: 
 
private: 
}; 
class ConcreteAbstractionImpB:public AbstractionImp
{ 
 ConcreteAbstractionImpB() {}
 ~ConcreteAbstractionImpB() {}
 virtual void Operation(); 
protected: 
 
private: 
};