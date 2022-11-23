
#include "quadtree.h"

namespace qt {
    template<typename T, typename PairT, typename ContainerT>
    QuadTree<T, PairT, ContainerT>::QuadTree(Vertex center, Vertex range, unsigned int bucket_size,
                                             unsigned int depth, bool sort) {
        m_root = new Node{center, range};
        max_depth = depth > 0 ? depth : 16;
        max_bucket_size = bucket_size > 0 ? bucket_size : 1;
        m_sort = sort;
        m_pair_comp = PairComp();
    }

    template<typename T, typename PairT, typename ContainerT>
    QuadTree<T, PairT, ContainerT>::~QuadTree() {
        delete m_root;
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::insert(const Vertex &point, const T &data) {
        // Duplicates check
        if (contains(point)) return false;

        // Bound check
        if (!in_region(point, m_root->bottom_left(), m_root->top_right())) return false;

        return insert(point, data, m_root, nullptr, 0);
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::update(const Vertex &point, const T &data) {
        // Bound check
        if (!in_region(point, m_root->bottom_left(), m_root->top_right())) return false;

        if (!contains(point)) {
            return insert(point, data, m_root, m_root, 0);
        }

        std::stack<Node *> nodes;
        nodes.push(m_root);
        Node *top = nodes.top();
        unsigned dir;

        while (!top->m_leaf) {
            dir = direction(point, top);
            if (top->m_children[dir]) {
                nodes.push(top->m_children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        for (int i = 0; i < top->m_bucket.size(); ++i)
            if (top->m_bucket[i].first == point)
                top->m_bucket[i].second = data;
        return false;
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::contains(const Vertex &point) {
        std::stack<Node *> nodes;
        nodes.push(m_root);
        Node *top = nodes.top();
        unsigned dir;

        // Find appropriate node (non-recursive loop)
        while (!top->m_leaf) {
            dir = direction(point, top);
            if (top->m_children[dir]) {
                nodes.push(top->m_children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        // Find that point.
        for (int i = 0; i < top->m_bucket.size(); ++i)
            if (top->m_bucket[i].first == point)
                return true;
        return false;
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::remove(const Vertex &point) {
        std::stack<Node *> nodes;
        nodes.push(m_root);
        Node *top = nodes.top();
        unsigned dir;

        // Find appropriate node (non-recursive loop)
        while (!top->m_leaf) {
            dir = direction(point, top);
            if (top->m_children[dir]) {
                nodes.push(top->m_children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        // Find that point and delete nodes.
        for (int i = 0; i < top->m_bucket.size(); ++i) {
            if (top->m_bucket[i].first == point) {
                top->m_bucket.erase(top->m_bucket.begin() + i);
                reduce(nodes);
                return true;
            }
        }
        return false;
    }

    template<typename T, typename PairT, typename ContainerT>
    std::vector<std::pair<Vertex, T>>
    QuadTree<T, PairT, ContainerT>::data_in_region(const Vertex &bottom_left, const Vertex &top_right) {
        std::vector<std::pair<Vertex, T>> results{};
        std::queue<Node *> nodes;
        nodes.push(m_root);

        while (!nodes.empty()) {
            Node *top = nodes.front();

            // Leaf node
            if (top->m_leaf) {
                enclosure status = this->status(top->m_center, top->m_range, bottom_left, top_right);
                switch (status) {
                    case IN_BOUND:
                        results.insert(results.end(), top->m_bucket.begin(), top->m_bucket.end());
                        break;

                    case PARTIAL_BOUND:
                        for (int i = 0; i < top->m_bucket.size(); ++i)
                            if (in_region(top->m_bucket[i].first, bottom_left, top_right))
                                results.insert(results.end(), top->m_bucket[i]);
                        break;

                    default:
                        break;
                }
                nodes.pop();
                continue;
            }

            // Stem node
            for (int i = 0; i < 4; ++i) {
                if (top->m_children[i] == nullptr) continue;
                enclosure status = this->status(top->m_children[i]->m_center, top->m_children[i]->m_range,
                                                bottom_left, top_right);
                switch (status) {
                    case IN_BOUND:
                        add_points_to_result(top->m_children[i], results);
                        break;

                    case PARTIAL_BOUND:
                        nodes.push(top->m_children[i]);
                        break;

                    default:
                        break;
                }
            }
            nodes.pop();
        }
        return results;
    }

    template<typename T, typename PairT, typename ContainerT>
    Vertex QuadTree<T, PairT, ContainerT>::new_center(int direction, QuadTree::Node *node) {
        Vertex v(node->m_center.x, node->m_center.y);
        switch (direction) {
            case BOT_LEFT:
                v -= node->m_range / 2.0;
                break;
            case TOP_LEFT:
                v.x -= node->m_range.x / 2.0;
                v.y += node->m_range.y / 2.0;
                break;
            case BOT_RIGHT:
                v.x += node->m_range.x / 2.0;
                v.y -= node->m_range.y / 2.0;
                break;
            case TOP_RIGHT:
                v += node->m_range / 2.0;
                break;
            default:
                break;
        }
        return v;
    }

    template<typename T, typename PairT, typename ContainerT>
    int QuadTree<T, PairT, ContainerT>::direction(const Vertex &point, QuadTree::Node *node) {
        unsigned X = 0;
        unsigned Y = 0;
        X |= ((point.x >= node->m_center.x) << 1);
        Y |= ((point.y >= node->m_center.y) << 0);
        return (int) (X | Y);
    }

    template<typename T, typename PairT, typename ContainerT>
    QuadTreeNode<T, PairT, ContainerT> *&QuadTree<T, PairT, ContainerT>::child_node(const Vertex &v,
                                                                                    QuadTree::Node *&node) {
        unsigned dir = direction(v, node);
        if (node->m_children[dir] != nullptr) {
            // Child node already exists, return that child node.
            return node->m_children[dir];
        } else {
            // Child node doesn't exist, create new one and return it.
            node->m_children[dir] = new Node(new_center(dir, node), node->m_range / 2.0);
            return node->m_children[dir];
        }
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::insert(const Vertex &v, const T &data,
                                                QuadTree::Node *&node, QuadTree::Node *parent_node,
                                                unsigned depth) {
        // Insertion will not happen if insertion point's depth limit has been reached.

        // Insert only when the node is a m_leaf node
        if (node->m_leaf) {
            if (node->m_bucket.size() < max_bucket_size) {
                // Bucket in that node is not full yet, add data to the m_bucket.
                typename std::vector<PairT>::iterator insert_it;
                if (m_sort)
                    insert_it = std::lower_bound(node->m_bucket.begin(), node->m_bucket.end(),
                                                 PairT{v, data}, m_pair_comp);
                else
                    insert_it = node->m_bucket.end();
                node->set_parent(parent_node);
                node->m_bucket.insert(insert_it, PairT{v, data});
                return true;
            } else if (depth < max_depth) {
                // Change this m_leaf node to stem node first
                node->m_leaf = false;
                insert(v, data, child_node(v, node), node, 1 + depth);

                // Pull out data from this node and put it in corresponding child
                for (int i = 0; i < node->m_bucket.size(); ++i) {
                    insert(node->m_bucket[i].first,
                           node->m_bucket[i].second,
                           child_node(node->m_bucket[i].first, node),
                           node,
                           1 + depth);
                }
                node->m_bucket.clear();
            }
        } else {
            insert(v, data, child_node(v, node), node, 1 + depth);
        }
        return false;
    }

    template<typename T, typename PairT, typename ContainerT>
    void QuadTree<T, PairT, ContainerT>::reduce(std::stack<Node *> &nodes) {
        bool canReduce = true;
        typename std::vector<PairT>::iterator insert_it;
        nodes.pop();
        while (canReduce && !nodes.empty()) {
            canReduce = true;
            Node *top = nodes.top();
            int numKeys = 0;
            for (int i = 0; i < 4; ++i) {
                if (top->m_children[i] && !top->m_children[i]->m_leaf) {
                    return;
                } else if (top->m_children[i] && top->m_children[i]->m_leaf) {
                    numKeys += top->m_children[i]->m_bucket.size();
                }
            }
            canReduce &= (numKeys <= max_bucket_size);
            if (canReduce) {
                for (int i = 0; i < 4; ++i) {
                    if (top->m_children[i]) {
                        top->m_children[i]->set_parent(top);
                        for (int j = 0; j < top->m_children[i]->m_bucket.size(); ++j) {
                            if (m_sort)
                                insert_it = std::lower_bound(top->m_bucket.begin(),
                                                             top->m_bucket.end(),
                                                             top->m_children[i]->m_bucket[j],
                                                             m_pair_comp);
                            else
                                insert_it = top->m_bucket.end();
                            top->m_bucket.insert(insert_it, top->m_children[i]->m_bucket[j]);
                        }
                        delete top->m_children[i];
                        top->m_children[i] = nullptr;
                    }
                }
                top->m_leaf = true;
            }
            nodes.pop();
        }
    }

    template<typename T, typename PairT, typename ContainerT>
    void QuadTree<T, PairT, ContainerT>::add_points_to_result(QuadTree::Node *node,
                                                              std::vector<std::pair<Vertex, T>> &results) {
        if (node->m_leaf) {
            results.insert(results.end(), node->m_bucket.begin(), node->m_bucket.end());
            return;
        }
        for (int i = 0; i < 4; ++i) {
            if (node->m_children[i] != nullptr) {
                add_points_to_result(node->m_children[i], results);
            }
        }
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::in_region(const Vertex &point,
                                                   const Vertex &bottom_left,
                                                   const Vertex &top_right) {
        return (point.x >= bottom_left.x) &&
               (point.x < top_right.x) &&
               (point.y >= bottom_left.y) &&
               (point.y < top_right.y);
    }

    template<typename T, typename PairT, typename ContainerT>
    enclosure QuadTree<T, PairT, ContainerT>::status(const Vertex &center, const Vertex &range,
                                                     const Vertex &bottom_left, const Vertex &top_right) {
        int enclosedPts = 0;
        enclosedPts += in_region({center.x - range.x, center.y - range.y}, bottom_left, top_right);
        enclosedPts += in_region({center.x - range.x, center.y + range.y}, bottom_left, top_right);
        enclosedPts += in_region({center.x + range.x, center.y - range.y}, bottom_left, top_right);
        enclosedPts += in_region({center.x + range.x, center.y + range.y}, bottom_left, top_right);

        if (enclosedPts == 4)
            return IN_BOUND;
        if (enclosedPts > 0)
            return PARTIAL_BOUND;

        Vertex nodeMin{center - range};
        Vertex nodeMax(center + range);
        enclosedPts += in_region(bottom_left, nodeMin, nodeMax);
        enclosedPts += in_region({bottom_left.x, top_right.y}, nodeMin, nodeMax);
        enclosedPts += in_region(bottom_left, nodeMin, nodeMax);
        enclosedPts += in_region({bottom_left.x, top_right.y}, nodeMin, nodeMax);

        if (enclosedPts > 0)
            return PARTIAL_BOUND;

        return OUT_OF_BOUND;
    }

    template<typename T, typename PairT, typename ContainerT>
    std::vector<std::pair<Vertex, T>> QuadTree<T, PairT, ContainerT>::extract_all() {
        return data_in_region(m_root->m_center - m_root->m_range, m_root->m_center + m_root->m_range);
    }
}

