#pragma once
#include "../global_declare.h"

FORWARD_DECLARE_PTR(TreeNode);

enum COLOR_TYPE
{
    RED,
    BLACK,
    UNKNOWN
};

struct TreeNode:public boost::enable_shared_from_this<TreeNode>
{

    TreeNode(int value) { value_ = value; }
    int value_{0};

    int height_{0};

    COLOR_TYPE color_type_{COLOR_TYPE::UNKNOWN};

    // using TreeNodeWptr = boost::weak_ptr<TreeNode>;

    TreeNodePtr lchild_{nullptr};
    TreeNodePtr rchild_{nullptr};
    TreeNode* parent_{nullptr};

    bool is_lchild_{true};

    TreeNodePtr get_shared_ptr()
    {
        return shared_from_this();
    }

    bool is_lchild()
    {
        if (!parent_)
        {
            cout << "TreeNode has no parent" << endl;
            throw ("TreeNode has no parent");
        }
        else
        {
            if (parent_->rchild_.get() == this)
            {
                return false;
            }
            else if (parent_->lchild_.get() == this)
            {
                return true;
            }
            else
            {
                cout << "TreeNode is not parent's child" << endl;
                throw ("TreeNode is not parent's child");
            }
        }
    }

    string get_color_type()
    {
        if (color_type_ == COLOR_TYPE::RED)
        {
            return "R";
        }
        else if (color_type_ == COLOR_TYPE::BLACK)
        {
            return "B";
        }
        else
        {
            return "U";
        }
    }
    
    string get_info(string intro="")
    {
        string result = intro + " " + std::to_string(value_) + " " + get_color_type() + "; ";
        return result;
    }

    ~TreeNode()
    {
        cout << "~TreeNode: " << value_ << endl;
    }
};

void set_height(TreeNodePtr);

void preoder_traversal(TreeNodePtr);

void inoder_traversal(TreeNodePtr);

void postorder_traversal(TreeNodePtr);

void level_traversal(TreeNodePtr);

