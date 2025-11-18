#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
using namespace std;

// 2）
//假设我们已经给定了长度为N的输入数组data，以及它的Rank值数组rank，
// 现在需要求出这个数组给定的一个半开半闭子序列区间[start, stop)，0≤start＜stop≤N的rank值，
// 即这，取值为1 … stop-start。
// 要求：由于已经给定了原序列的rank，求子序列rank的算法复杂度应不高于O(NlogN)，最好是O(N)。
/// <summary>
/// 返回给定数据的子序列[start, stop)按从小到大排列的rank
/// 注意：不需要校验输入参数data/start/stop的合法性
/// </summary>
/// <param name="data">数据数组</param>
/// <param name="rank">已求得的rank数组（取值1...N）</param>
/// <param name="start">子序列的起始位置（含）</param>
/// <param name="stop">子序列的终止位置（不含）</param>
/// <returns>子序列的rank数组（取值1...stop-start）</returns>
std::vector<size_t> get_sub_rank(const std::vector<double>& data, 
	const std::vector<size_t>& rank, size_t start, size_t stop)
{
	//请编写代码

    // 1. 先求出子序列的rank
    std::vector<size_t> sub_rank(stop - start);

    vector<std::pair<double, size_t>> data_rank;
    for (size_t i = 0; i < data.size(); i++) {
        data_rank.push_back({data[i], i});
    }
     // 2. 排序子序列的rank
     std::sort(sub_rank.begin(), sub_rank.end());
     return sub_rank;
}

/// 返回给定数据按从小到大顺序排列的rank值，
/// 例如令data={1.1, 0.8, 2.3, 0.4, 3.5};返回get_rank(data)=={3, 2, 4, 1, 5};
/// </summary>
/// <param name="data">输入的数据数组</param>
/// <returns>输出的rank数组</returns>

std::vector<size_t> get_sub_rank(const std::vector<double>& data) {
    std::vector<size_t> rank(data.size());

    vector<size_t> sorted_data;
    for (int i = 0; i < data.size(); i++) { 
        sorted_data.push_back(data[i]);
    }
    sort(sorted_data.begin(), sorted_data.end());

    std::unordered_map<double, size_t> data_rank_map;

    for (int i = 0; i < data.size(); i++) {
        data_rank_map[sorted_data[i]] = i + 1;
    }

    for (size_t i = 0; i < data.size(); i++) {
        rank[i] = data_rank_map[data[i]];
    }
    return rank;
}

#include "test_2025_11_18.h"

