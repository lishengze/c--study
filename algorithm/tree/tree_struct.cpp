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

// 中 左 右
// 非递归的方式实现树的前序遍历
// 前序遍历 - 非递归实现 (中 -> 左 -> 右)
void preorder_traversal_no_recu_doubao(TreeNodePtr node) {
    cout << "------- preorder_traversal_no_recu doubao -------" << endl;
    
    if (!node) return;
    
    std::stack<TreeNodePtr> stack;
    stack.push(node);
    
    while (!stack.empty()) {
        // 1. 访问当前节点
        TreeNodePtr current = stack.top();
        stack.pop();
        cout << current->get_info() << endl;
        
        // 2. 先压右子节点（栈后进先出，保证左子节点先访问）
        if (current->rchild_) {
            stack.push(current->rchild_);
        }
        
        // 3. 后压左子节点
        if (current->lchild_) {
            stack.push(current->lchild_);
        }
    }
    cout << "-----------------------------------" << endl;
}

void preorder_traversal_no_recu(TreeNodePtr node) {
    cout << "------- preorder_traversal_no_recu -------" << endl;
    
    if (!node) return;
    
    std::stack<TreeNodePtr> stack;
    
    while (node || !stack.empty()) {
        while (node) {
            stack.push(node);
            cout << node->get_info() << endl;
            node = node->lchild_;
        }

        node = stack.top();
        stack.pop();
        node = node->rchild_; // 转向右子树， 一层是取父节点，一层是进入右子树；
    }
    cout << "-----------------------------------" << endl;
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

            current = current->rchild_; // 一层是取父节点，一层是进入右子树；
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

// 以非递归的方式实现树的后序遍历
void postorder_traversal_no_recu(TreeNodePtr node)
{
    cout << "------- postorder_traversal_no_recu -------" << endl;

    std::stack<TreeNodePtr> stack;

    TreeNodePtr ptrCurrNode = node;
    TreeNodePtr lastVisited = nullptr;


    while (ptrCurrNode || !stack.empty()) {
        while (ptrCurrNode) {
            stack.push(ptrCurrNode);
            ptrCurrNode = ptrCurrNode->lchild_;
        }

        ptrCurrNode = stack.top();

        if (nullptr == ptrCurrNode->rchild_ || ptrCurrNode->rchild_ == lastVisited) {
            cout << ptrCurrNode->get_info() << endl;
            stack.pop();
            lastVisited = ptrCurrNode; // 第一次是左，第二次是右;
            ptrCurrNode = nullptr; // 防止重新进入左子树遍历 -- 取出父节点;
        } else {
            ptrCurrNode = ptrCurrNode->rchild_; // 转向右子树
        }
    
    }

}

// 后序遍历 - 非递归实现 (左 -> 右 -> 中)
void postorder_traversal_no_recu_doubao(TreeNodePtr node) {
    cout << "------- postorder_traversal_no_recu doubao -------" << endl;
    
    if (!node) return;
    
    std::stack<TreeNodePtr> stack;
    TreeNodePtr current = node;
    TreeNodePtr last_visited = nullptr;
    
    while (current || !stack.empty()) {
        // 1. 遍历左子树，将所有左子节点入栈
        while (current) {
            stack.push(current);
            current = current->lchild_;
        }
        
        current = stack.top();
        
        // 2. 检查右子树是否存在且未被访问
        if (!current->rchild_ || current->rchild_ == last_visited) {
            // 3. 访问当前节点
            cout << current->get_info() << endl;
            stack.pop();
            last_visited = current;
            current = nullptr; // 防止重新进入左子树遍历
        } else {
            // 4. 转向右子树
            current = current->rchild_;
        }
    }
    cout << "-----------------------------------" << endl;
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