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
        RBTree(const vector<int>& data);

        RBTree() {}

        void init_rb_tree(const vector<int>& data);     

        void turn_left(TreeNodePtr& node);

        void turn_right(TreeNodePtr& node);

        void insert_node(int value);

        void insert_simple(TreeNodePtr& parent, TreeNodePtr node);

        bool insert_simple_with_height(TreeNodePtr& parent, TreeNodePtr node);

        // void reform_node(TreeNodePtr& node);

        void reform_node_simple(TreeNodePtr& node);

        void UR_Set(TreeNodePtr& G, TreeNodePtr&P, TreeNodePtr& U, TreeNodePtr& N);
        void LR_Rotate(TreeNodePtr& G, TreeNodePtr&P, TreeNodePtr& U, TreeNodePtr& N);
        void LL_Rotate(TreeNodePtr& G, TreeNodePtr&P, TreeNodePtr& U, TreeNodePtr& N);
        void RL_Rotate(TreeNodePtr& G, TreeNodePtr&P, TreeNodePtr& U, TreeNodePtr& N);
        void RR_Rotate(TreeNodePtr& G, TreeNodePtr&P, TreeNodePtr& U, TreeNodePtr& N);

        void reset_root();

        // TreeNodePtr parent{nullptr};
        // TreeNodePtr grandparent{nullptr};
};