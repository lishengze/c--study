#include "basic_algo.h"


int KMP::get_pos(const string& ori_data, const string& des_substr)
{
    try
    {
        vector<int> next;
        get_next_vec(des_substr, next);

        cout << "Next: " << endl;
        for (auto index:next)
        {
            cout << index << " ";
        }
        cout << endl;

        int i = 0;
        int j = 0;
        int ori_data_len = ori_data.length();
        int des_substr_len = des_substr.length();

        // while (i < ori_data_len && j < des_substr_len)
        // while (i < ori_data.length() && j < des_substr.length())
        while (i < int(ori_data.size()) && j < int(des_substr.size()))
        {
            // cout << "Start: i= " << i <<", j=" << j << " ";
            if (j == -1 || ori_data[i] == des_substr[j])
            {
                i++;
                j++;                
            }
            else
            {
                j = next[j];
            }

            // cout << "End: i= " << i <<", j=" << j << " ";
            // cout << "ori_data.length: " << ori_data.length() << ", des_substr.length: " << des_substr.size() << endl;

            // cout << "oridata[" << i << "]= " << ori_data[i] << ", des_substr[" << j << "]= " << des_substr[j] << endl;
        }

        // cout << "ori_data: " << ori_data << ", len: " << ori_data.length() << endl;
        // cout << "des_substr: " << des_substr << ", len: " << des_substr.length() << endl;

        // cout << "i: " << i << ", j: " << j << endl;

        if (j == des_substr.length())
        {
            // cout << "a " << endl;
            return i - j;
        }
        else if (i == ori_data.length())
        {
            // cout << "b " << endl;
            return -1;
        }
        else
        {
            cout << "unknow error" << endl;
            return -2;
            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] KMP::get_pos " << e.what() << '\n';
    }

}

/*
1. i == 0 : next[i] == -1;
2. i < j : 
if    p[i] == p[j] ==> next[j+1] = i + 1
else: i = next[i]

*/
void KMP::get_next_vec(const string& des_substr, vector<int>& result)
{
    try
    {
        int i = -1;
        int j = 0;
        result.push_back(-1);
        while (j < des_substr.length()-1)
        {
            if (i == -1 || des_substr[i] == des_substr[j])
            {
                // result[j+1] = i + 1;
                result.push_back(i+1);
                // cout << "des_substr["<< i <<"]=" << des_substr[i] << ",des_substr["<< j << "]=" << des_substr[j] 
                //      << ",next[" << j+1 << "] = " << i+1 << endl;
                ++i;
                ++j;
            }
            else
            {
                i = result[i];
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] KMP::get_next_vec " << e.what() << '\n';
    }
    
}