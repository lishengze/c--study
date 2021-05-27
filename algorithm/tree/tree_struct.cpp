#include "tree_struct.h"

void set_height(TreeNodePtr node)
{
    try
    {
        if (node)
        {
            set_height(node->lchild_);

            set_height(node->rchild_);

            if (node->lchild_ && node->rchild_)
            {
                node->height_ = max(node->lchild_->height_, node->rchild_->height_) + 1;
            }         
            else if (node->lchild_)
            {
                node->height_ = node->lchild_->height_ + 1;
            }
            else if (node->rchild_)
            {
                node->height_ = node->rchild_->height_ + 1;
            }
            else
            {
                node->height_ = 1;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

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

            if (node->height_)
            {
                cout << node->height_ << " ";
            }                    

            if (node->color_type_ == COLOR_TYPE::BLACK)
            {
                cout << "B  "; 
            }
            else if (node->color_type_ == COLOR_TYPE::RED)
            {
                cout << "R ";
            }

            cout << endl;

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

            if (node->height_)
            {
                cout << node->height_ << endl;
            }            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void level_traversal(TreeNodePtr node)
{
    try
    {
        if (!node) return;

        queue<TreeNodePtr> node_queue;

        node_queue.push(node);

        while(!node_queue.empty())
        {
            TreeNodePtr tmp = node_queue.front();
            node_queue.pop();

            cout << tmp->value_ << " ";
            // if (tmp->height_ != 0)
            // {
            //     cout << tmp->height_ << endl;
            // }

            if (!tmp->lchild_) node_queue.push(tmp->lchild_);

            if (!tmp->rchild_) node_queue.push(tmp->rchild_);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}