#include "test_shared_ptr.h"
#include <memory>
#include <boost/smart_ptr.hpp>

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

void test_re_refrence() 
{
    
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

// 核心：，一个对象的 裸指针 不能用于初始化多个 只能指针
// 当前对象在不知道是否已经被其他指针指针管理的情况下，怎样通过智能指针对其管理？
// boost::enable_shared_from_this : 提供了内置的智能指针，不会和外部初始化后的智能指针冲突。

class Test: public boost::enable_shared_from_this<Test>
{
    public:
        Test() { std::cout << "Test Constructor. " << std::endl;}

        //析构函数
        ~Test() { std::cout << "Test Destructor." << std::endl; }
        //获取指向当前对象的指针
        boost::shared_ptr<Test> GetObject_ShareOrigin()
        {
            boost::shared_ptr<Test> pTest(this);
            return pTest;
        }

        boost::shared_ptr<Test> GetObject_EnableShare()
        {
            return shared_from_this();
        }    

        Test* GetObject_Origin() { return this;}
};

void test_boost_shared_ptr_enable_share_from_this()
{
    try
    {
        {
            // boost::shared_ptr<Test> test_obj_ptr_a = boost::make_shared<Test>();
            // boost::shared_ptr<Test> test_obj_ptr_b = test_obj_ptr_a.get()->GetObject_ShareOrigin();
            // boost::shared_ptr<Test> test_obj_ptr_c = test_obj_ptr_a;
            // cout << "test_obj_ptr_a.count: " << test_obj_ptr_a.use_count() << endl;
            // cout << "test_obj_ptr_b.count: " << test_obj_ptr_b.use_count() << endl;
            // cout << "test_obj_ptr_c.count: " << test_obj_ptr_c.use_count() << endl;
        }

        {
            // boost::shared_ptr<Test> test_obj_ptr_a = boost::make_shared<Test>();
            // boost::shared_ptr<Test> test_obj_ptr_b (test_obj_ptr_a.get()->GetObject());
            // cout << "test_obj_ptr_a.count: " << test_obj_ptr_a.use_count() << endl;
            // cout << "test_obj_ptr_b.count: " << test_obj_ptr_b.use_count() << endl;
        }
        
        {
            // Test* test = new Test();
            // boost::shared_ptr<Test> test_obj_ptr_a(test);
            // boost::shared_ptr<Test> test_obj_ptr_b (test);      
            // cout << "test_obj_ptr_a.count: " << test_obj_ptr_a.use_count() << endl;
            // cout << "test_obj_ptr_b.count: " << test_obj_ptr_b.use_count() << endl;          
        }

        {
            // Test* test = new Test();
            // boost::shared_ptr<Test> test_obj_ptr_a = test->shared_from_this();
            // boost::shared_ptr<Test> test_obj_ptr_b = test->shared_from_this();      
            // cout << "test_obj_ptr_a.count: " << test_obj_ptr_a.use_count() << endl;
            // cout << "test_obj_ptr_b.count: " << test_obj_ptr_b.use_count() << endl;           
        }

        {
            boost::shared_ptr<Test> test_obj_ptr_a (new Test());
            boost::shared_ptr<Test> test_obj_ptr_b = test_obj_ptr_a->GetObject_EnableShare();
            boost::shared_ptr<Test> test_obj_ptr_c = test_obj_ptr_a.get()->GetObject_EnableShare();
            cout << "test_obj_ptr_a.count: " << test_obj_ptr_a.use_count() << endl;
            cout << "test_obj_ptr_b.count: " << test_obj_ptr_b.use_count() << endl;        
            cout << "test_obj_ptr_c.count: " << test_obj_ptr_c.use_count() << endl;           
        }
    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
}

void test_boost_shared_ptr_reset()
{
    // boost::shared_ptr<Test> a = boost::make_shared<Test>();
    boost::shared_ptr<Test> a(new Test());
    boost::shared_ptr<Test> b = a;
    cout << "a.use_cout: " << a.use_count() << endl;
    cout << "b.use_cout: " << b.use_count() << endl;

    a.reset();
    cout << "a.use_cout: " << a.use_count() << endl;
    cout << "b.use_cout: " << b.use_count() << endl;    
}

void delete_test_array(Test* ptr)
{
    delete[] ptr;
}

void test_deleter()
{
    // boost::shared_ptr<Test> array_ptr = boost::make_shared<Test*>{new Test[5], delete_test_array};
    boost::shared_ptr<Test> array_ptr (new Test[5], delete_test_array);
}

void test_weak_ptr() {
    struct Node {
        Node(int value):value(value) {}
        std::shared_ptr<Node> child{nullptr};
        std::weak_ptr<Node> parent;

        int value;
    };

    std::shared_ptr<Node> root = std::make_shared<Node>(1);
    std::shared_ptr<Node> child = std::make_shared<Node>(2);

    root->child = child;
    child->parent = root;

    cout << "root.use_count: " << root.use_count() << ", child.use_count: " << child.use_count() << endl;

    auto root_p = child->parent.lock();
    if (root_p) {
        root_p->value = 10;
    }

    cout << "root.value: " << root->value << endl;
}

void test_boost_shared_ptr()
{
    test_boost_shared_ptr_enable_share_from_this();

    // test_boost_shared_ptr_reset();

    // test_deleter();
}

void test_shared_ptr()
{
    // test_boost_shared_ptr();

    test_weak_ptr();
}