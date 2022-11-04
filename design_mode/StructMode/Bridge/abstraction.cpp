#include "abstraction.h" 
#include "abstraction_imp.h" 
#include <iostream>
using namespace std; 
Abstraction::Abstraction() 
{ 
} 

Abstraction::~Abstraction() 
{ 
} 

RefinedAbstraction::RefinedAbstraction(AbstractionImp* imp)
{ 
 _imp = imp; 
} 

void RefinedAbstraction::Operation() 
{ 
 _imp->Operation();
} 
