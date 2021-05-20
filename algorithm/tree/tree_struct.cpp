#include "tree_struct.h"


BaseTree::BaseTree(int* data, int start, int end)
{
    init_tree(data, start, end);

    cout << "preoder_traversal" << endl;

    preoder_traversal(root);
}

void BaseTree::init_tree(int* data, int start, int end)
{
    try
    {
        vector<TreeNodePtr> node_vec_;

        for (int i = start; i <= end; ++i)
        {
            node_vec_.emplace_back(boost::make_shared<TreeNode>(data[i]));
        }

        if (node_vec_.size() > 0)
        {
            queue<TreeNodePtr> node_queue;
            node_queue.push(node_vec_[0]);

            root = node_vec_[0];

            for (int i = 1; i < node_vec_.size();)
            {
                TreeNodePtr node = node_queue.front();
                node_queue.pop();

                cout << "root " << node->value_ << " ";

                node->lchild_ = node_vec_[i];
                node_queue.push(node_vec_[i]);

                cout << "lchild: " << node->lchild_->value_ << " ";

                if (i++ >= node_vec_.size()) 
                {
                    cout << endl;
                    break;
                }

                node->rchild_ = node_vec_[i];
                node_queue.push(node_vec_[i]);

                i++;

                cout << "rchild_: " << node->rchild_->value_ << endl;

            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] init_tree  " << e.what() << '\n';
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