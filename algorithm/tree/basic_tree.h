#pragma once

#include "../global_declare.h"
#include "tree_struct.h"
#include <vector>

class BaseTree
{
    public:
        BaseTree(int* data, int start, int end);
        BaseTree() {}

        void init_tree(int* data, int start, int end);

        void init_rb_tree(int* data, int start, int end);

        void init_avl_tree(int* data, int start, int end);

        void init_level_order_tree(const std::vector<int>& data);

        void init_pre_order_tree(const std::vector<int>& data);

        void init_in_order_tree(const std::vector<int>& data);

        void init_post_order_tree(const std::vector<int>& data);

        TreeNodePtr get_root() { return root;}

    protected:
        TreeNodePtr root{nullptr};
};


void TestBaseTree();