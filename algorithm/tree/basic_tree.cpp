#include "basic_tree.h"
#include "../global_declare.h"
#include "tree_struct.h"


BaseTree::BaseTree(int* data, int start, int end)
{
    init_tree(data, start, end);

    cout << "preoder_traversal" << endl;

    preoder_traversal(root);

    cout << "\ninoder_traversal" << endl;

    inoder_traversal(root);
}

void BaseTree::init_tree(int* data, int start, int end)
{
    try
    {
        vector<TreeNodePtr> node_vec_;

        for (unsigned int i = start; i <= end; ++i)
        {
            // node_vec_.push_back(boost::make_shared<TreeNode>(data[i]));

            TreeNodePtr node (new TreeNode(data[i]));
            node_vec_.push_back(node);
        }

        // for (auto node:node_vec_)
        // {
        //     cout << node->value_ << " ";
        // }
        // cout << endl;

        if (node_vec_.size() > 0)
        {
            // queue<TreeNodePtr> node_queue;

            list<TreeNodePtr> node_queue;

            // node_queue.push(node_vec_[0]);
            node_queue.push_back(node_vec_[0]);

            root = node_vec_[0];

            for (unsigned int i = 1; i < node_vec_.size();)
            {
                // cout << "node_queue.size: " << node_queue.size() << endl;

                // for (auto atom:node_queue)
                // {
                //     cout << atom->value_ << " ";
                // }
                // cout << endl;

                if (node_queue.size() <= 0) return;

                TreeNodePtr node = node_queue.front();
                node_queue.pop_front();

                // cout << "root " << node->value_ << " ";

                node->lchild_ = node_vec_[i];
                node_queue.push_back(node_vec_[i]);
                

                // cout << "lchild: " << node_vec_[i]->value_ << " ";

                if (i++ >= node_vec_.size()) 
                {
                    cout << endl;
                    break;
                }

                node->rchild_ = node_vec_[i];
                node_queue.push_back(node_vec_[i]);

                i++;

                // cout << "rchild_: " << node_vec_[i]->value_ << endl;

            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] init_tree  " << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr << "\n[E] init_tree unknown exceptions!" << '\n';
    }
    
}
