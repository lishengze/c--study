#include "tree_struct.h"
#include <stack>
#include <queue>

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
    // cout << "------- preoder_traversal -------" << endl;
    try
    {
        if (node)
        {
            cout << node->get_info() << endl;

            preoder_traversal(node->lchild_);

            preoder_traversal(node->rchild_);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void preorder_traversal_no_recu(TreeNodePtr node) {

}

// 左 中 右
void inoder_traversal(TreeNodePtr node)
{
    try
    {
        if (node)
        {
            inoder_traversal(node->lchild_);

            cout << node->get_info() << endl;

            // cout << node->value_ << " ";        

            // if (node->height_)
            // {
            //     cout << node->height_ << " ";
            // }                    

            // if (node->color_type_ == COLOR_TYPE::BLACK)
            // {
            //     cout << "B  "; 
            // }
            // else if (node->color_type_ == COLOR_TYPE::RED)
            // {
            //     cout << "R ";
            // }

            // cout << endl;

            inoder_traversal(node->rchild_);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

// 左 中 右
void inoder_traversal_no_recu(TreeNodePtr node)
{
    try
    {
        std::stack<TreeNodePtr> stack;
        TreeNodePtr current = node;
        cout << "------- inoder_traversal_no_recu -------" << endl;

        while (current || !stack.empty())
        {
            while (current)
            {
                stack.push(current);
                current = current->lchild_;
            }

            current = stack.top();
            stack.pop();

            cout << current->get_info() << endl;

            current = current->rchild_;
        }

        cout << "-------------" << endl;
        current = node;
        while(nullptr != current || !stack.empty()) {
            while(nullptr != current) {
                stack.push(current);

                if (nullptr == current->lchild_) {
                    cout << current->get_info() << endl;
                    stack.pop();
                    
                } 
                current = current->lchild_;
            }

            if (!stack.empty()) {
                TreeNodePtr parent = stack.top();
                stack.pop();
                cout << parent->get_info() << endl;
                current = parent->rchild_;
            }
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
        cout << "------- level_traversal -------" << endl;
        if (!node) return;

        queue<TreeNodePtr> node_queue;

        node_queue.push(node);

        while(!node_queue.empty())
        {
            TreeNodePtr tmp = node_queue.front();
            node_queue.pop();

            cout << tmp->get_info() << " ";

            if (nullptr != tmp->lchild_) node_queue.push(tmp->lchild_);

            if (nullptr != tmp->rchild_) node_queue.push(tmp->rchild_);
        }

        cout << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}