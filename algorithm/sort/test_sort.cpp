#include "test_sort.h"

#include "basic_sort.h"

#include "../til/tool.h"

void TestSort()
{
    int ori_data[10] = {5, 1, 7, 3, 4, 6, 8, 9, 2, 10};
    int start = 0;
    int end = 9;
    
    PrintData(ori_data, start, end, "ori_data");

    // quick_sort(ori_data, start, end);

    // PrintData(ori_data, start, end, "quick_sorted_data");

    merge_sort(ori_data, start, end);

    PrintData(ori_data, start, end, "merge_sorted_data");

}