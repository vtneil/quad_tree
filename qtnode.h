#ifndef QUAD_TREE_QTNODE_H
#define QUAD_TREE_QTNODE_H

namespace qt {
    template<typename T, typename PairT = std::pair<Vertex, T>, typename ContainerT = std::vector<PairT>>
    class QuadTreeNode {
    public:
        Vertex center;
        Vertex range;
        QuadTreeNode *parent;
        QuadTreeNode *children[4];
        bool leaf;
        ContainerT bucket{};

    public:
        QuadTreeNode<T>(const Vertex &center, const Vertex &range, QuadTreeNode<T> *parent = nullptr) :
                center{center}, range{range}, leaf{true} {
            this->parent = parent;
            for (auto &c: children) c = nullptr;
        }

        ~QuadTreeNode() {
            for (auto &c: children) delete c;
        }

        bool is_leaf() const {
            return leaf;
        }

        void set_parent(QuadTreeNode *&parent_node) {
            parent = parent_node;
        }

        Vertex top_right() const {
            return center + range;
        }

        Vertex bottom_left() const {
            return center - range;
        }
    };
}

#endif //QUAD_TREE_QTNODE_H
