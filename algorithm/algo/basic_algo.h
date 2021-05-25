#pragma once

#include "../global_declare.h"

class KMP
{
public:
    int get_pos(const string& ori_data, const string& des_substr);

    void get_next_vec(const string& des_substr, vector<int>& result);
};