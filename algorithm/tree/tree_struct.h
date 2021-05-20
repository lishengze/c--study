#include "../global_declare.h"

FORWARD_DECLARE_PTR(TreeNode);


struct TreeNode
{
    TreeNode(int value) { value_ = value; }
    int value_;

    TreeNodePtr lchild_{nullptr};
    TreeNodePtr rchild_{nullptr};
};

class BaseTree
{
    public:
        BaseTree(int* data, int start, int end);
        BaseTree() {}

        void init_tree(int* data, int start, int end);

        TreeNodePtr get_root() { return root;}

    private:


        TreeNodePtr root{nullptr};
};

void preoder_traversal(TreeNodePtr);

void inoder_traversal(TreeNodePtr);

void postorder_traversal(TreeNodePtr);

