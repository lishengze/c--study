#include <iostream>
#include <stack>

struct node {
    node(int data): data(data), left(nullptr), right(nullptr) {
    }
int data;
struct node *left;
struct node *right;
};

// 按照中序遍历转换 二叉搜索树;
node* ConvertToBST(node* root) {
    if (root == NULL) {
        return NULL;
    }

    node* head = nullptr;
    node* prev = nullptr;
    node* curr = root;

    std::stack<node*> stkNode;

    while(nullptr != curr ||!stkNode.empty()) {
        while(nullptr != curr) {
            stkNode.push(curr);
            curr = curr->left;
        }

        curr = stkNode.top();
        stkNode.pop();

        if (nullptr != prev) {
            head = curr;
        } else {
            curr->left = prev;
            curr->right = nullptr;
        }

        prev = curr;
        curr = curr->right;

    }

    return head;

}