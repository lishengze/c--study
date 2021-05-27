#include "rb_tree.h"

RBTree::RBTree(const vector<int>& data)
{
    try
    {
        init_rb_tree(data);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void RBTree::init_rb_tree(const vector<int>& data)
{
    try
    {
        // cout << "init_rb_tree " << endl;
        for (auto value:data)
        {
            insert_node(value);
        }

        cout << "\ninoder_traversal " << endl;
        inoder_traversal(root);
        cout << endl;

        set_height(root);
        cout << "\nAdd Height inoder_traversal " << endl;
        inoder_traversal(root);
        cout << endl;        

        // cout << "\nlevel_traversal " << endl;
        // level_traversal(root);

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
void RBTree::turn_left(TreeNode* node)
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
                if (!node->is_lchild())
                {
                    node->parent_->rchild_ = ori_right;
                    // ori_right->is_lchild_ = false;
                }
                else
                {
                    node->parent_->lchild_ = ori_right;
                    // ori_right->is_lchild_ = true;
                }                
            }
            node->parent_ = ori_right.get();
            
            node->rchild_ = ori_right->lchild_;
            // ori_right->lchild_->is_lchild_ = false;

            if (ori_right->lchild_)
            {
                ori_right->lchild_->parent_ = node;
            }

            ori_right->lchild_ = node->get_shared_ptr();
            // node->is_lchild_ = true;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << e.what() << '\n';
    }   
}


void RBTree::turn_left(TreeNodePtr node)
{
    try
    {
        if (!node) return;

        turn_left(node.get());
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
void RBTree::turn_right(TreeNode* node)
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
                if (node->is_lchild())
                {
                    node->parent_->lchild_ = ori_left;
                    // ori_left->is_lchild_ = true;
                }
                else
                {
                    node->parent_->rchild_ = ori_left;
                    // ori_left->is_lchild_ = false;
                }
            }
            node->parent_ = ori_left.get();

            node->lchild_ = ori_left->rchild_;
            // ori_left->rchild_->is_lchild_ = true;

            if (ori_left->rchild_)
            {
                ori_left->rchild_->parent_ = node;
            }

            ori_left->rchild_ = node->get_shared_ptr();
            // node->is_lchild_ = false;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void RBTree::turn_right(TreeNodePtr node)
{
    try
    {
        if (!node) return;

        turn_right(node.get());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void RBTree::insert_node(int value)
{
    try
    {
        // cout << "insert: " << value << endl;
        // TreeNode* pnode = new TreeNode(value);
        // TreeNodePtr node = pnode->get_shared_ptr();

        TreeNodePtr node = boost::make_shared<TreeNode>(value);

        node->color_type_ = COLOR_TYPE::RED;

        insert_simple(root, node);

        reform_node(node);

        cout << endl;
        inoder_traversal(root);
        cout << endl;

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] RBTree::insert_node " << e.what() << '\n';
    }
}

void RBTree::insert_simple(TreeNodePtr& parent, TreeNodePtr node)
{
    try
    {
        if (parent)
        {
            // cout << "parent: " << parent->value_ << endl;

            if (parent->value_ < node->value_)
            {
                if (parent->rchild_)
                {
                    insert_simple(parent->rchild_, node);

                }
                else
                {
                    parent->rchild_ = node;
                    node->parent_ = parent.get();
                    parent->rchild_->is_lchild_ = false;
                }                
            }
            else
            {
                if (parent->lchild_)
                {
                    insert_simple(parent->lchild_, node);

                }
                else
                {
                    parent->lchild_ = node;
                    node->parent_ = parent.get();
                    parent->lchild_->is_lchild_ = true;
                }                
            }
        }
        else
        {
            // cout << "parent is null: " << node->value_ << endl;
            parent = node;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] insert_simple " << e.what() << '\n';
    }    
}

bool RBTree::insert_simple_with_height(TreeNodePtr& parent, TreeNodePtr node)
{
    try
    {
        bool is_height_added = false;
        if (parent)
        {
            
            // cout << "parent: " << parent->value_ << endl;

            if (parent->value_ < node->value_)
            {
                if (parent->rchild_)
                {
                    is_height_added = insert_simple_with_height(parent->rchild_, node);

                    if (is_height_added)
                    {
                        if (parent->lchild_ && parent->lchild_->height_ >= parent->rchild_->height_)
                            is_height_added = false;
                    }
                }
                else
                {
                    parent->rchild_ = node;
                    node->parent_ = parent.get();
                    parent->rchild_->height_ = 1;
                    parent->rchild_->is_lchild_ = false;

                    if (!parent->lchild_) is_height_added = true;
                }                
            }
            else
            {
                if (parent->lchild_)
                {
                    is_height_added = insert_simple_with_height(parent->lchild_, node);

                    if (is_height_added)
                    {
                        if (parent->rchild_ && parent->rchild_->height_ >= parent->lchild_->height_)
                            is_height_added = false;
                    }
                }
                else
                {
                    parent->lchild_ = node;
                    node->parent_ = parent.get();
                    parent->lchild_->height_ = 1;
                    parent->lchild_->is_lchild_ = true;

                    if (!parent->rchild_) is_height_added = true;
                }                
            }

            if (is_height_added) parent->height_ += 1;            
        }
        else
        {
            // cout << "parent is null: " << node->value_ << endl;
            parent = node;
            parent->height_ = 1;
        }

        return is_height_added;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] insert_simple " << e.what() << '\n';
    }    
}

void RBTree::reform_node(TreeNodePtr& node)
{
    try
    {
        if (!node->parent_) 
        {
            node->color_type_ = COLOR_TYPE::BLACK;
            root = node;
            cout << root->get_info("root: ") << endl;
            return;
        }
        
        if (node->parent_->color_type_ == COLOR_TYPE::BLACK) 
        {
            cout << node->get_info("node:")
                 << node->parent_->get_info("parent:")
                 << endl; 
            return;
        }

        // TreeNode* parent = node->parent_;

        TreeNodePtr parent = node->parent_->get_shared_ptr();
        // cout << parent->get_info("parent ") << " user_count: " << parent.use_count() << endl;

        if (parent->parent_)
        {

            // TreeNode* grandparent = parent->parent_;
            TreeNodePtr grandparent = parent->parent_->get_shared_ptr();
            // cout << grandparent->get_info("grandparent ") << " user_count: " << grandparent.use_count() << endl;

            TreeNodePtr uncle;

            // parent is on the left tree;
            if (parent->is_lchild())
            {
                uncle = grandparent->rchild_;
                if (uncle)
                {
                    if (uncle->color_type_ == COLOR_TYPE::RED)
                    {
                        parent->color_type_ = COLOR_TYPE::BLACK;
                        uncle->color_type_ = COLOR_TYPE::BLACK;
                        grandparent->color_type_ = COLOR_TYPE::RED;
                        cout << "[REL] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << uncle->get_info("uncle:")
                                        << node->get_info("node:")
                                        << endl;

                        reform_node(grandparent);
                    }
                    else 
                    {
                        // LR
                        if (!node->is_lchild())
                        {
                            cout << "[LR] " << grandparent->get_info("gp:")
                                          << parent->get_info("parent:")
                                          << uncle->get_info("uncle:")
                                          << node->get_info("node:")
                                          << endl;                            
                            turn_left(parent);
                            reform_node(parent);
                        }
                        else // LL
                        {
                            cout << "[LL] " << grandparent->get_info("gp:")
                                          << parent->get_info("parent:")
                                          << uncle->get_info("uncle:")
                                          << node->get_info("node:")
                                          << endl;                               
                            parent->color_type_ = COLOR_TYPE::BLACK;
                            grandparent->color_type_ = COLOR_TYPE::RED;
                            turn_right(grandparent);
                        }
                    }
                } 
                else
                {
                    // LR
                    if (!node->is_lchild())
                    {
                        cout << "[LR] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << node->get_info("node:")
                                        << endl;                           
                        turn_left(parent);
                        reform_node(parent);
                    }
                    else // LL
                    {
                        cout << "[LL] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << node->get_info("node:")
                                        << endl;                            
                        parent->color_type_ = COLOR_TYPE::BLACK;
                        grandparent->color_type_ = COLOR_TYPE::RED;
                        turn_right(grandparent);
                    }
                }
            }
            else
            {
                uncle = grandparent->lchild_;
                if (uncle)
                {
                    if (uncle->color_type_ == COLOR_TYPE::RED)
                    {
                        parent->color_type_ = COLOR_TYPE::BLACK;
                        uncle->color_type_ = COLOR_TYPE::BLACK;
                        grandparent->color_type_ = COLOR_TYPE::RED;
                        cout << "[RER] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << uncle->get_info("uncle:")
                                        << node->get_info("node:")
                                        << endl;                        
                        reform_node(grandparent);
                    }   
                    else
                    {
                        // RL
                        if (node->is_lchild())
                        {
                            cout << "[RLU] " << grandparent->get_info("gp:")
                                            << parent->get_info("parent:")
                                            << uncle->get_info("uncle:")
                                            << node->get_info("node:")
                                            << endl;                                
                            turn_right(parent);
                            reform_node(parent);
                        }
                        else // RR
                        {
                            cout << "[RRU] " << grandparent->get_info("gp:")
                                          << parent->get_info("parent:")
                                          << uncle->get_info("uncle:")
                                          << node->get_info("node:")
                                          << endl;                                  
                            parent->color_type_ = COLOR_TYPE::BLACK;
                            grandparent->color_type_ = COLOR_TYPE::RED;
                            turn_left(grandparent);                        
                        }
                    }   
                } 
                else
                {
                    // RL
                    if (node->is_lchild())
                    {
                        cout << "[RL] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << node->get_info("node:")
                                        << endl;                          
                        turn_right(parent);
                        reform_node(parent);
                    }
                    else // RR
                    {
                        cout << "[RR] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << node->get_info("node:")
                                        << endl;                         
                        parent->color_type_ = COLOR_TYPE::BLACK;
                        grandparent->color_type_ = COLOR_TYPE::RED;
                        turn_left(grandparent);    

                        cout << "[RR_E] " << grandparent->get_info("gp:")
                                        << parent->get_info("parent:")
                                        << node->get_info("node:")
                                        << endl;                                               
                    }                    
                }        
            }
        }
        else
        {
            std::cerr << node->parent_->value_ << " 's parent is null!" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}