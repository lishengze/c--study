#include "test_shared_ptr.h"
#include <memory>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

void test_re_refrence() {
    
    class Node {
        public:
            std::shared_ptr<Node> leftPtr;
            std::shared_ptr<Node> rightPtr;
            // std::shared_ptr<Node> parentPtr;
            std::weak_ptr<Node> parentPtr;
            Node(int val) : value(val) {
                std::cout << "Constructor" << std::endl;
            }
            ~Node() {
                std::cout << "Destructor" << std::endl;
            }

        private:
            int value;        
    };

    std::shared_ptr<Node> ptr = std::make_shared<Node>(4);
    ptr->leftPtr = std::make_shared<Node>(2);
    ptr->leftPtr->parentPtr = ptr;
    ptr->rightPtr = std::make_shared<Node>(2);
    ptr->rightPtr->parentPtr = ptr;

    cout << "Ptr refrence count: " << ptr.use_count() << endl;
    cout << "Ptr leftchild refrence count: " << ptr->leftPtr.use_count() << endl;
    cout << "Ptr rightchild refrence count: " << ptr->rightPtr.use_count() << endl;
}
