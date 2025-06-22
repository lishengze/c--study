// #ifndef ORDER_H
// #define ORDER_H

#include <iostream>
#include "visitor.h"

class Order:public ElementBase {
    MP_VISITOR_PATTERN_DECLARE(ElementBase);
public:
    Order(int id, double amount) : id(id), amount(amount) {}

    int getId() const {
        return id;
    }

    double getAmount() const {
        return amount;
    }

    void display() const {
        std::cout << "Order ID: " << id << ", Amount: $" << amount << std::endl;
    }   
private:
    int id; 
    double amount;
};

class CancelOrder:public ElementBase {
    MP_VISITOR_PATTERN_DECLARE(ElementBase);
public:
    CancelOrder(int id, double amount) : id(id), amount(amount) {}

    int getId() const {
        return id;
    }

    double getAmount() const {
        return amount;
    }

    void display() const {
        std::cout << "CancelOrder ID: " << id << ", Amount: $" << amount << std::endl;
    }   
private:
    int id; 
    double amount;
};


// #endif // ORDER_H