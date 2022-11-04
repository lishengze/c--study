#include "abstraction_imp.h" 
#include <iostream>
using namespace std; 

void AbstractionImp::Operation() 
{ 
 cout<<"AbstractionImp....imp..."<<endl; 
} 



void ConcreteAbstractionImpA::Operation() 
{ 
 cout<<"ConcreteAbstractionImpA...."<<endl; 
} 


void ConcreteAbstractionImpB::Operation() 
{ 
 cout<<"ConcreteAbstractionImpB...."<<endl; 
}