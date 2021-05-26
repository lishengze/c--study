#pragma once

#include "../global_declare.h"
#include "tree_struct.h"

class BaseTree
{
    public:
        BaseTree(int* data, int start, int end);
        BaseTree() {}

        void init_tree(int* data, int start, int end);

        void init_rb_tree(int* data, int start, int end);

        void init_avl_tree(int* data, int start, int end);

        TreeNodePtr get_root() { return root;}

    private:

        TreeNodePtr root{nullptr};
};

