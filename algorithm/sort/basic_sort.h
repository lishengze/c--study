#pragma once

#include "../global_declare.h"

template<class T>
void quick_sort(T* data, int left, int right)
{
    try
    {
        if (NULL == data) return;

        if (left < 0 || left >= right || right < 0) return;

        T pivot = data[left];

        int low = left + 1;        
        int high = right;

        while (low < high)
        {
            while(data[high] >= pivot && high > low)
            {
                --high;
            }

            while(data[low] <= pivot && high > low)
            {
                ++low;
            }

            if (high > low)
            {
                T tmp = data[high];
                data[high] = data[low];
                data[low] = tmp;
            }
        }

        data[left] = data[low];
        data[low] = pivot;

        quick_sort(data, low+1, right);
        quick_sort(data, left, low-1);        
    }
    catch (const std::exception& e)
    {
        std::cout << "\n[E]Quick Sort " << e.what() << std::endl;
    }
}

template<class T>
void merge_sort(T* data, int left, int right)
{
    try
    {
        if (right > left)
        {
            int middle = (right + left) / 2;

            merge_sort(data, left, middle);

            merge_sort(data, middle+1, right);

            int index1 = left;
            int index2 = middle+1;

            vector<T> tmp_data;

            while(index1 <= middle && index2 <= right)
            {
                if (data[index1] < data[index2])
                {
                    tmp_data.emplace_back(data[index1++]);
                }
                else
                {
                    tmp_data.emplace_back(data[index2++]);
                }
            }

            while (index1 <= middle)
            {
                tmp_data.emplace_back(data[index1++]);
            }

            while(index2 <= right)
            {
                tmp_data.emplace_back(data[index2++]);
            }

            int start = left;
            for (int i = 0; i < tmp_data.size(); ++i)
            {
                data[start++] = tmp_data[i];
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "\n[E]merge_sort " << e.what() << std::endl;
    }
}




