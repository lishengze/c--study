#include "test_tree.h"
#include "basic_tree.h"

#include "tree_struct.h"

#include "rb_tree.h"

#include "../til/tool.h"

void test_tree_node()
{
    int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int start = 0;
    int end = 9;

    PrintData(data, start, end, "ori_data");

    BaseTree base_obj(data, 0, 9);
}

void test_rb_tree()
{
    // vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // vector<int> data = {10,9,8,7,6,5,4,3,2,1};

    vector<int> data = {1,4,8,6,5,7,2,10,9};

    PrintData(data, "ori_data");
    
    RBTree tree(data);
}



void TestTree()
{
    test_rb_tree();

    // test_tree_node();
}