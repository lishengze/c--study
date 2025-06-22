#ifndef BUSSINESS_PROCESSOR_H
#define BUSSINESS_PROCESSOR_H

#include "order.h"
#include <iostream>
using namespace std;

class BussinessProcessor:public Visitor {
public:
    BussinessProcessor();
    virtual ~BussinessProcessor() {

    }

    /**
     * @brief 处理订单
     * @param order 订单对象
     */
    void processOrder(Order* order) {
        cout << "处理订单" << endl;
    }

    void processCancelOrder(CancelOrder* cancelOrder) {
        cout << "处理取消订单" << endl;
    }
};


#endif // BUSSINESS_PROCESSOR_H