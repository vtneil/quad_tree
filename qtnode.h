#ifndef QUAD_TREE_QTNODE_H
#define QUAD_TREE_QTNODE_H

namespace qt {
    template<typename T, typename PairT, typename ContainerT>
    class QuadTree;

    template<typename T, typename PairT = std::pair<Vertex, T>, typename ContainerT = std::vector<PairT>>
    class QuadTreeNode {
        friend class QuadTree<T, PairT, ContainerT>;

    protected:
        Vertex m_center;
        Vertex m_range;
        QuadTreeNode *m_parent;
        QuadTreeNode *m_children[4];
        bool m_leaf;
        ContainerT m_bucket{};

    public:
        QuadTreeNode<T>(const Vertex &center, const Vertex &range, QuadTreeNode<T> *parent = nullptr) :
                m_center{center}, m_range{range}, m_leaf{true} {
            this->m_parent = parent;
            for (auto &c: m_children) c = nullptr;
        }

        ~QuadTreeNode() {
            for (auto &c: m_children) delete c;
        }

        Vertex center() const {
            return m_center;
        }

        Vertex range() const {
            return m_range;
        }

        QuadTreeNode *parent() const {
            return m_parent;
        }

        QuadTreeNode *children() const {
            return m_children;
        }

        ContainerT *&bucket() {
            return m_bucket;
        }

        bool is_leaf() const {
            return m_leaf;
        }

        void set_parent(QuadTreeNode *&parent_node) {
            m_parent = parent_node;
        }

        Vertex top_right() const {
            return m_center + m_range;
        }

        Vertex bottom_left() const {
            return m_center - m_range;
        }
    };
}

#endif //QUAD_TREE_QTNODE_H
