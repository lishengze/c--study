#include <map>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

class Solution1 {
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param quote_orders int整型vector<vector<>> 每个挂单用一个长度为4的vec表示[time,ref,price,front]
     * @param cancel_orders int整型vector<vector<>> 每个撤单用一个长度为2的vec表示[time, ref]
     * @param query int整型vector<vector<>> 每个查询用一个长度为2的vec表示 [time, price]
     * @return bool布尔型vector
     */

    struct Order {
        Order(const std::vector<int>& tmp) {
            time = tmp[0];
            ref = tmp[1];
            price = tmp[2];
            front = tmp[3];
        }

        string str() {
            return string("time: ") + std::to_string(time) + ",ref:" + std::to_string(ref) 
            + ", price:" + std::to_string(price) + ", front:" + std::to_string(front);
        }

        int time;       // 挂单时间
        int ref;        // 订单编号
        int price;      // 订单价格
        int front;      // 订单席位  
    };
    using OrderShptr = std::shared_ptr<Order>;

    // 定义查询结构体，以price 为关键字排序
    struct QueryOrder {
        QueryOrder(const std::vector<int>& tmp) {
            time = tmp[0];
            price = tmp[1];
        }

        string str() {
            return string("time: ") + std::to_string(time) + ", price:" + std::to_string(price);
        }           

        int time;       // 查询时间
        int price;      // 查询价格
    };
    using QueryOrderShptr = std::shared_ptr<QueryOrder>;

    bool QueryOrderCmp(const QueryOrderShptr& p1, const QueryOrderShptr& p2) {
        return p1->price < p2->price;
    }


    void InsertNewOrder(vector<OrderShptr>& srcOrder, OrderShptr pNewOrder) {
        srcOrder.push_back(pNewOrder);

        for (int i = srcOrder.size()-1; i > 0; i--) {
            if (srcOrder[i-1]->price > srcOrder[i]->price) {
                OrderShptr tmp = srcOrder[i-1];
                srcOrder[i-1] = srcOrder[i];
                srcOrder[i] = tmp;
            } else {
                break;
            }
        }
    }

    void InsertNewOrder(vector<QueryOrderShptr>& srcOrder, QueryOrderShptr pNewOrder) {
        srcOrder.push_back(pNewOrder);

        for (int i = srcOrder.size()-1; i > 0; i--) {
            if (srcOrder[i-1]->price > srcOrder[i]->price) {
                QueryOrderShptr tmp = srcOrder[i-1];
                srcOrder[i-1] = srcOrder[i];
                srcOrder[i] = tmp;
            } else {
                break;
            }
        }
    }    

    void PrintMapOrder(std::unordered_map<int, OrderShptr>& srcOrder) {
         cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter.second->str() << endl;
        }
    }

    void PrintOrder(vector<OrderShptr>& srcOrder) {
        cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter->str() << endl;
        }
    }

    void PrintOrder(vector<QueryOrderShptr>& srcOrder) {
        cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter->str() << endl;
        }
    }    

    vector<bool> solve(vector<vector<int> >& quote_orders, vector<vector<int> >& cancel_orders, vector<vector<int> >& query) {
        // write code here

        // 初始化订单簿
        std::unordered_map<int, OrderShptr> mapOrder;
        for (auto tmp:quote_orders) {
            OrderShptr pShOrder = std::make_shared<Order>(tmp);
            mapOrder[pShOrder->ref] = pShOrder;            
        }
        cout << "--------- mapOrder:" << endl;
        PrintMapOrder(mapOrder);

        // 开始进行撤单;
        for (auto tmp:cancel_orders) {
            // 若是 当前订单簿里存在 ref 的单子，并且挂单时间早于撤单时间 那么将其从订单簿里删除;
            if (mapOrder.find(tmp[1]) != mapOrder.end() && mapOrder[tmp[1]]->time < tmp[0]) {
                mapOrder.erase(tmp[1]);
            }
        }

        cout << "--------- mapOrder after cancel:" << endl;
        PrintMapOrder(mapOrder);

        // 针对撤单后的订单簿 创建一个按照价格排序的新订单簿，并将其排序
        vector<OrderShptr> priceOrderVec;
        priceOrderVec.reserve(mapOrder.size());

        for (auto iter:mapOrder) {
            InsertNewOrder(priceOrderVec, iter.second);
        }

        cout << "--------- priceOrderVec:" << endl;
        PrintOrder(priceOrderVec);
// 开始进行查询
        vector<bool> res;
        res.reserve(query.size());

        // 按照查询价格排序
        std::vector<QueryOrderShptr> queryOrderVec;
        queryOrderVec.reserve(query.size());

        for (int i = 0; i < query.size(); i++) {
            QueryOrderShptr pShQuery = std::make_shared<QueryOrder>(query[i]);
            InsertNewOrder(queryOrderVec, pShQuery);
        }

        cout << "--------- queryOrderVec:" << endl;
        PrintOrder(queryOrderVec);

        // 查询逻辑:
        // 1. 判断当前订单簿价格小于等于查询价格的所有订单 对应的席位是否相同
        //   a. 若是相同此次查询结果为true
        //   b. 若是不同此次查询结果为false
        // 2. 若是当前订单簿所有订单价格大于查询价格，则此次查询结果为true;
        // 3. 若是当前订单簿为空，则此次查询结果为false;

        bool bLastFound = true;
        int iLastIndex = 0;  // 记录上一次查询的索引，用于判断是否出现不同的席位

        bool bFound = true; // 记录此次的结果;
        for (auto pQuery:queryOrderVec) {

            // 如果上一轮的查询已经失败，代表 在较小查询价格下，已经出现不同的席位，那么此次的查询结果一定为false
            if (bLastFound == false) {
                res.push_back(false);
                continue;
            }

            // 如果订单簿为空，那么所有查询结果都是false;
            if (priceOrderVec.empty()) {
                res.push_back(false);
                bLastFound = false;
                continue;
            }

            // 订单簿已经遍历完，且上一轮查询结果为true，那么此次查询结果为true;
            if (iLastIndex >= priceOrderVec.size()) {
                res.push_back(bLastFound);
                continue;
            }

            int curFront = priceOrderVec[iLastIndex]->front;
            int i = iLastIndex;
            for (i; i < priceOrderVec.size(); i++) {
                if (pQuery->price >= priceOrderVec[i]->price) {
                    if (curFront != priceOrderVec[i]->front) {
                        bFound = false;
                        break;
                    }
                } else {                    
                    break;
                }
                
            }
            iLastIndex = i;
            bLastFound = bFound;
            res.push_back(bFound);
        }
        return res;
    }
};


class Solution {
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param quote_orders int整型vector<vector<>> 每个挂单用一个长度为4的vec表示[time,ref,price,front]
     * @param cancel_orders int整型vector<vector<>> 每个撤单用一个长度为2的vec表示[time, ref]
     * @param query int整型vector<vector<>> 每个查询用一个长度为2的vec表示 [time, price]
     * @return bool布尔型vector
     */

    struct Order {
        explicit Order(int itime, int iref,int iprice, int ifront, int ioptype) {
            time = itime;
            ref = iref;
            price = iprice;
            front = ifront;   
            type = ioptype;         
            isCancel = false;
        }

        Order() = delete;

        string str() {
            return string("time: ") + std::to_string(time) + ",ref:" + std::to_string(ref) 
            + ", price:" + std::to_string(price) + ", front:" + std::to_string(front)
            + ", type:" + std::to_string(type) + ", isCancel:" + std::to_string(isCancel);
        }

        int time;       // 挂单时间
        int ref;        // 订单编号
        int price;      // 订单价格
        int front;      // 订单席位  

        int type; // 0-挂单，1-撤单，2-查询;
        bool isCancel; // 是否是撤单
    };
    using OrderShptr = std::shared_ptr<Order>;

    // 定义查询结构体，以price 为关键字排序
    struct QueryOrder {
        QueryOrder(const std::vector<int>& tmp) {
            time = tmp[0];
            price = tmp[1];
        }

        string str() {
            return string("time: ") + std::to_string(time) + ", price:" + std::to_string(price);
        }           

        int time;       // 查询时间
        int price;      // 查询价格
    };
    using QueryOrderShptr = std::shared_ptr<QueryOrder>;

    bool QueryOrderCmp(const QueryOrderShptr& p1, const QueryOrderShptr& p2) {
        return p1->price < p2->price;
    }


    void InsertNewOrder(vector<OrderShptr>& srcOrder, OrderShptr pNewOrder) {
        srcOrder.push_back(pNewOrder);

        for (int i = srcOrder.size()-1; i > 0; i--) {
            if (srcOrder[i-1]->price > srcOrder[i]->price) {
                OrderShptr tmp = srcOrder[i-1];
                srcOrder[i-1] = srcOrder[i];
                srcOrder[i] = tmp;
            } else {
                break;
            }
        }
    }

    void InsertNewOrderByTime(vector<OrderShptr>& srcOrder, OrderShptr pNewOrder) {
        srcOrder.push_back(pNewOrder);
        for (int i = srcOrder.size()-1; i > 0; i--) {
            if (srcOrder[i-1]->time > srcOrder[i]->time) {
                OrderShptr tmp = srcOrder[i-1];
                srcOrder[i-1] = srcOrder[i];
                srcOrder[i] = tmp;
            } else {
                break;
            }
        }
    }    

    void InsertNewOrder(vector<QueryOrderShptr>& srcOrder, QueryOrderShptr pNewOrder) {
        srcOrder.push_back(pNewOrder);

        for (int i = srcOrder.size()-1; i > 0; i--) {
            if (srcOrder[i-1]->price > srcOrder[i]->price) {
                QueryOrderShptr tmp = srcOrder[i-1];
                srcOrder[i-1] = srcOrder[i];
                srcOrder[i] = tmp;
            } else {
                break;
            }
        }
    }    

    void PrintMapOrder(std::unordered_map<int, OrderShptr>& srcOrder) {
         cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter.second->str() << endl;
        }
    }

    void PrintOrder(vector<OrderShptr>& srcOrder) {
        cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter->str() << endl;
        }
    }

    void PrintOrder(vector<QueryOrderShptr>& srcOrder) {
        cout << "size:" << srcOrder.size() << endl;
        for (auto iter:srcOrder) {
            cout << iter->str() << endl;
        }
    }    

    void CancelOrder(vector<OrderShptr>& srcOrder,  int cancel_index) {
        for (int i = 0; i < cancel_index; i++) {
            if (srcOrder[i]->ref == srcOrder[cancel_index]->ref) {
                srcOrder[i]->isCancel = true;
                break;
            }
        }
    }

    bool QueryOrder(vector<OrderShptr>& srcOrder,  int query_index) {
        bool result = true;
        int lastFront = -1;
        for (int i = 0; i < query_index; i++) {
            if (srcOrder[i]->type == 0 && srcOrder[i]->isCancel==false && srcOrder[i]->price <= srcOrder[query_index]->price) {
                if (lastFront == -1) {
                    lastFront = srcOrder[i]->front;
                } else if (lastFront != srcOrder[i]->front) {
                    result = false;
                    break;   
                }
            }
        }
        return result;
    }

    vector<bool> solve(vector<vector<int> >& quote_orders, vector<vector<int> >& cancel_orders, vector<vector<int> >& query) {
        // write code here

        // 将 所有操作按照时间排序
        vector<OrderShptr> timeOrderVec;
        timeOrderVec.reserve(quote_orders.size() + cancel_orders.size() + query.size());

        for (auto tmp:quote_orders) {
            OrderShptr pShOrder = std::make_shared<Order>(tmp[0], tmp[1], tmp[2], tmp[3], 0);

            InsertNewOrderByTime(timeOrderVec, pShOrder);
        }

        for (auto tmp:cancel_orders) {
            OrderShptr pShOrder = std::make_shared<Order>(tmp[0], tmp[1], 0, 0, 1);
            InsertNewOrderByTime(timeOrderVec, pShOrder);
        }

        for (auto tmp:query) {
            OrderShptr pShOrder = std::make_shared<Order>(tmp[0], 0, tmp[1], 0, 2);

            InsertNewOrderByTime(timeOrderVec, pShOrder);
        }

        cout << "--------- timeOrderVec:" << endl;
        PrintOrder(timeOrderVec);

        vector<bool> res;
        res.reserve(query.size());

        for (int i = 0; i < timeOrderVec.size(); i++) {
            if (timeOrderVec[i]->type == 1) {
                // 挂单
                CancelOrder(timeOrderVec, i);
            } else if (timeOrderVec[i]->type == 2) {
                res.push_back(QueryOrder(timeOrderVec, i));
            }
        }


        return res;
    }
};


void TestOrder() {
    cout << "TestOrder" << endl;

    Solution sol;

    vector<vector<int> > quote_orders = {{3,3,1,2}, {5,5,15,5}, {6,6,4,2}, {8,8,8,2}, {13,13,16,5}, {16,16,15,3}, {18,18,18,4}, {19,19,16,5}};
    vector<vector<int> > cancel_orders = {{1,1}, {7,6}, {10,11}, {11,2}, {12,12}, {14,7}};
    vector<vector<int> > query = {{2,1}, {4,1}, {9,9}, {15,1}, {17,11}, {20,20}};


    // vector<vector<int> > quote_orders = {{1,1,1,1}, {3,2,2,2}};
    // vector<vector<int> > cancel_orders = {};
    // vector<vector<int> > query = {{2,2}, {4,2}};

    vector<bool> res = sol.solve(quote_orders, cancel_orders, query);

    for (auto iter:res) {
        cout << iter << endl;
    }
}