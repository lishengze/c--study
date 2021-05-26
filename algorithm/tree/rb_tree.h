#pragma once

#include "basic_tree.h"

/*
红黑树:
1. 根节点，叶子节点都为 黑色 - 节点只有两个颜色，叶子节点不存储指;
2. 红色节点的 子结点 都为黑色;
3. 节点到任意一个叶子节点的路径中，黑色子结点的个数相同;
4. 从任意一个叶子节点到根节点路径上不能有两个连续的红节点
*/
class RBTree:public BaseTree 
{
    public:
        RBTree(int* data, int start, int end);
        RBTree() {}

        void init_rb_tree(int* data, int start, int end);     

        void turn_left(TreeNodePtr node);

        void turn_right(TreeNodePtr node);
};