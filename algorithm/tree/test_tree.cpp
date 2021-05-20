#include "test_tree.h"
#include "basic_tree.h"

#include "tree_struct.h"

#include "../til/tool.h"

void TestTree()
{
    int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int start = 0;
    int end = 9;

    PrintData(data, start, end, "ori_data");

    BaseTree base_obj(data, 0, 9);

    
}