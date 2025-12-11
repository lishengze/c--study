#include "basic_tree.h"
#include "../global_declare.h"
#include "tree_struct.h"

#include <iostream>
#include <memory>
#include <vector>
#include <queue>

using std::cout;
using std::endl;


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

void BaseTree::init_level_order_tree(const std::vector<int>& vecSrcData) {
    std::queue<TreeNodePtr> first_node_queue; 
    std::queue<TreeNodePtr> second_node_queue;

    std::queue<TreeNodePtr>* pFirstNodeQueue = &first_node_queue; 
    std::queue<TreeNodePtr>* pSecondNodeQueue = &second_node_queue;

    int iNodeIndex = 0;
    for (int i = 0; i < vecSrcData.size(); i++) {
        TreeNodePtr node  = std::make_shared<TreeNode>( vecSrcData[i]);

        if (pFirstNodeQueue->empty()) {
            pFirstNodeQueue->push(node);
            root = node;
        } else {
            TreeNodePtr parent = pFirstNodeQueue->front();

            cout << "parent: " << parent->value_ << endl;

            pSecondNodeQueue->push(node);

            if (parent->lchild_ == nullptr)
            {
                parent->lchild_ = node;
                node->is_lchild_ = true;
                iNodeIndex++;
                cout << "--lchild: " << node->value_ << endl;
            } else if (parent->rchild_ == nullptr)
            {
                parent->rchild_ = node;
                node->is_lchild_ = false;
                iNodeIndex++;
                cout << "--rchild: " << node->value_ << endl;
            } 

            // 当前父节点的左右子节点都满了，切换下一个父节点;
            if (iNodeIndex == 2) {
                pFirstNodeQueue->pop();
                iNodeIndex = 0;
            }

            // 当前层遍历完成;
            if (pFirstNodeQueue->empty()) {
                std::queue<TreeNodePtr>* tmp = pFirstNodeQueue;
                pFirstNodeQueue = pSecondNodeQueue;
                pSecondNodeQueue = tmp;
                // cout << "second_node_queue.size: " << pFirstNodeQueue->size() << ", first_node_queue.size: " << pSecondNodeQueue->size() << endl;
            }
            
        }
        
    }
}

void BaseTree::init_pre_order_tree(const std::vector<int>& data) {
    
}

void BaseTree::init_in_order_tree(const std::vector<int>& data) {

}

void BaseTree::init_post_order_tree(const std::vector<int>& data) {

}


void test_init_tree() {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,15};

    BaseTree tree;

    tree.init_level_order_tree(data);
    level_traversal(tree.get_root());

    cout << "preoder_traversal" << endl;
    preoder_traversal(tree.get_root());
    cout << endl;

    preorder_traversal_no_recu(tree.get_root());
    preorder_traversal_no_recu_doubao(tree.get_root());

    inoder_traversal_no_recu(tree.get_root());
    
    postorder_traversal_no_recu(tree.get_root());
    postorder_traversal_no_recu_doubao(tree.get_root());
}

void TestBaseTree() {
    test_init_tree();
}