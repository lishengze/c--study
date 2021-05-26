#pragma once
#include "../global_declare.h"

FORWARD_DECLARE_PTR(TreeNode);

struct TreeNode
{
    enum TYPE
    {
        RED,
        BLACK
    };

    TreeNode(int value) { value_ = value; }
    int value_{0};

    TYPE type_;

    // using TreeNodeWptr = boost::weak_ptr<TreeNode>;

    TreeNodePtr lchild_{nullptr};
    TreeNodePtr rchild_{nullptr};
    TreeNode* parent_{nullptr};

    ~TreeNode()
    {
        // cout << "~TreeNode: " << value_ << endl;
    }
};


void preoder_traversal(TreeNodePtr);

void inoder_traversal(TreeNodePtr);

void postorder_traversal(TreeNodePtr);

