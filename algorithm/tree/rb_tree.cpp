#include "rb_tree.h"

void RBTree::init_rb_tree(int* data, int start, int end)
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
/*
左旋：右子树 > 左子树， 将当前节点降低到左子树，右子节点提升为父节点；
四个节点的关系转化: node, node->parent, node->rchild, node->rchild->lchild

1. 大小关系: node->rchild > node->rchild->lchild > node
2. 新的父子关系: 
    node->rchild => node->parent;
    node->rchild->lchild => node->rchild;
*/
void RBTree::turn_left(TreeNodePtr node)
{
    try
    {
        if (!node) return;

        TreeNodePtr ori_right = node->rchild_;

        if (!ori_right)
        {
            ori_right->parent_ = node->parent_;
            if (node->parent_)
            {
                if (node->parent_->rchild_ == node)
                {
                    node->parent_->rchild_ = ori_right;
                }
                else
                {
                    node->parent_->lchild_ = ori_right;
                }                
            }
            node->parent_ = ori_right.get();
            
            node->rchild_ = ori_right->lchild_;

            if (ori_right->lchild_)
            {
                ori_right->lchild_->parent_ = node.get();
            }

            ori_right->lchild_ = node;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << e.what() << '\n';
    }   
}

/*
右旋: 左子树 > 右子树 ， 将当前节点降低到右子树，左子节点提升为父节点；
四个节点的关系转化: node->parent, node, node->left, node->left->right;

*/
void RBTree::turn_right(TreeNodePtr node)
{
    try
    {
        if (!node) return;

        TreeNodePtr ori_left = node->lchild_;
        if (ori_left)
        {
            ori_left->parent_ = node->parent_;
            if (node->parent_)
            {
                if (node->parent_->lchild_ == node)
                {
                    node->parent_->lchild_ = ori_left;
                }
                else
                {
                    node->parent_->rchild_ = ori_left;
                }
            }
            node->parent_ = ori_left.get();

            node->lchild_ = ori_left->rchild_;
            if (ori_left->rchild_)
            {
                ori_left->rchild_->parent_ = node.get();
            }

            ori_left->rchild_ = node;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}