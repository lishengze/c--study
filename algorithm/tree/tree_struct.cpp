#include "tree_struct.h"

// 中 左 右
void preoder_traversal(TreeNodePtr node)
{
    try
    {
        if (node)
        {
            cout << node->value_ << " ";

            preoder_traversal(node->lchild_);

            preoder_traversal(node->rchild_);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

// 左 中 右
void inoder_traversal(TreeNodePtr node)
{
    try
    {
        if (node)
        {
            inoder_traversal(node->lchild_);

            cout << node->value_ << " ";            

            inoder_traversal(node->rchild_);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

// 左 右 中
void postorder_traversal(TreeNodePtr node)
{
    try
    {
        if (node)
        {
            postorder_traversal(node->lchild_);

            postorder_traversal(node->rchild_);

            cout << node->value_ << " ";

            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}