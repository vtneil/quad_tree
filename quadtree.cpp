
#include "quadtree.h"

namespace qt {
    template<typename T, typename PairT, typename ContainerT>
    QuadTree<T, PairT, ContainerT>::QuadTree(Vertex center, Vertex range, unsigned int bucket_size,
                                             unsigned int depth, bool sort) {
        m_root = new Node{center, range};
        max_depth = depth;
        max_bucket_size = bucket_size;
        m_sort = sort;
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

        return insert(point, data, m_root, 0);
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::update(const Vertex &point, const T &data) {
        // Bound check
        if (!in_region(point, m_root->bottom_left(), m_root->top_right())) return false;

        if (!contains(point)) {
            return insert(point, data, m_root, 0);
        }

        std::stack<Node *> nodes;
        nodes.push(m_root);
        Node *top = nodes.top();
        unsigned dir;

        while (!top->leaf) {
            dir = direction(point, top);
            if (top->children[dir]) {
                nodes.push(top->children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        for (int i = 0; i < top->bucket.size(); ++i)
            if (top->bucket[i].first == point)
                top->bucket[i].second = data;
        return false;
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::contains(const Vertex &point) {
        std::stack<Node *> nodes;
        nodes.push(m_root);
        Node *top = nodes.top();
        unsigned dir;

        // Find appropriate node (non-recursive loop)
        while (!top->leaf) {
            dir = direction(point, top);
            if (top->children[dir]) {
                nodes.push(top->children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        // Find that point.
        for (int i = 0; i < top->bucket.size(); ++i)
            if (top->bucket[i].first == point)
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
        while (!top->leaf) {
            dir = direction(point, top);
            if (top->children[dir]) {
                nodes.push(top->children[dir]);
                top = nodes.top();
            } else {
                return false;
            }
        }

        // Find that point and delete nodes.
        for (int i = 0; i < top->bucket.size(); ++i) {
            if (top->bucket[i].first == point) {
                top->bucket.erase(top->bucket.begin() + i);
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
            if (top->leaf) {
                enclosure status = this->status(top->center, top->range, bottom_left, top_right);
                switch (status) {
                    case IN_BOUND:
                        results.insert(results.end(), top->bucket.begin(), top->bucket.end());
                        break;

                    case PARTIAL_BOUND:
                        for (int i = 0; i < top->bucket.size(); ++i)
                            if (in_region(top->bucket[i].first, bottom_left, top_right))
                                results.insert(results.end(), top->bucket[i]);
                        break;

                    default:
                        break;
                }
                continue;
            }

            // Stem node
            for (int i = 0; i < 4; ++i) {
                if (top->child[i] == nullptr) continue;
                enclosure status = this->status(top->child[i]->center, top->child[i]->range,
                                                bottom_left, top_right);
                switch (status) {
                    case IN_BOUND:
                        add_points_to_result(top->child[i], results);
                        break;

                    case PARTIAL_BOUND:
                        nodes.push(top->child[i]);
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
        Vertex v(node->center.x, node->center.y);
        switch (direction) {
            case BOT_LEFT:
                v -= node->range / 2.0;
                break;
            case TOP_LEFT:
                v.x -= node->range.x / 2.0;
                v.y += node->range.y / 2.0;
                break;
            case BOT_RIGHT:
                v.x += node->range.x / 2.0;
                v.y -= node->range.y / 2.0;
                break;
            case TOP_RIGHT:
                v += node->range / 2.0;
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
        X |= ((point.x >= node->center.x) << 1);
        Y |= ((point.y >= node->center.y) << 0);
        return (int) (X | Y);
    }

    template<typename T, typename PairT, typename ContainerT>
    QuadTreeNode<T, PairT, ContainerT> *&
    QuadTree<T, PairT, ContainerT>::child_node(const Vertex &v, QuadTree::Node *&node) {
        unsigned dir = direction(v, node);
        if (node->children[dir] != nullptr) {
            // Child node already exists, return that child node.
            node->children[dir]->parent = m_root;
            return node->children[dir];
        } else {
            // Child node doesn't exist, create new one and return it.
            node->children[dir] = new Node(new_center(dir, node), node->range / 2.0);
            node->children[dir]->parent = m_root;
            return node->children[dir];
        }
    }

    template<typename T, typename PairT, typename ContainerT>
    bool QuadTree<T, PairT, ContainerT>::insert(const Vertex &v, const T &data, QuadTree::Node *&node, unsigned depth) {
        // Insertion will not happen if insertion point's depth limit has been reached.

        if (node->leaf) {
            if (node->bucket.size() < max_bucket_size) {
                // Bucket in that node is not full yet, add data to the bucket.
                typename std::vector<PairT>::iterator insert_it;
                if (m_sort)
                    insert_it = std::lower_bound(node->bucket.begin(), node->bucket.end(),
                                                 PairT{v, data}, PairComp());
                else
                    insert_it = node->bucket.end();
                node->bucket.insert(insert_it, PairT{v, data});
                return true;
            } else if (depth < max_depth) {
                // Change this leaf node to stem node first
                node->leaf = false;
                insert(v, data, child_node(v, node), 1 + depth);

                // Pull out data from this node and put it in corresponding child
                for (int i = 0; i < node->bucket.size(); ++i) {
                    insert(node->bucket[i].first,
                           node->bucket[i].second,
                           child_node(node->bucket[i].first, node),
                           1 + depth);
                }
                node->bucket.clear();
            }
        } else {
            insert(v, data, child_node(v, node), 1 + depth);
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
                if (top->children[i] && !top->children[i]->leaf) {
                    return;
                } else if (top->children[i] && top->children[i]->leaf) {
                    numKeys += top->children[i]->bucket.size();
                }
            }
            canReduce &= (numKeys <= max_bucket_size);
            if (canReduce) {
                for (int i = 0; i < 4; ++i) {
                    if (top->children[i]) {
                        for (int j = 0; j < top->children[i]->bucket.size(); ++j) {
                            if (m_sort)
                                insert_it = std::lower_bound(top->bucket.begin(),
                                                             top->bucket.end(),
                                                             top->children[i]->bucket[j],
                                                             PairComp());
                            else
                                insert_it = top->bucket.end();
                            top->bucket.insert(insert_it, top->children[i]->bucket[j]);
                        }
                        delete top->children[i];
                        top->children[i] = nullptr;
                    }
                }
                top->leaf = true;
            }
            nodes.pop();
        }
    }

    template<typename T, typename PairT, typename ContainerT>
    void QuadTree<T, PairT, ContainerT>::add_points_to_result(QuadTree::Node *node,
                                                              std::vector<std::pair<Vertex, T>> &results) {
        if (node->leaf) {
            results.insert(results.end(), node->bucket.begin(), node->bucket.end());
            return;
        }
        for (int i = 0; i < 4; ++i) {
            if (node->child[i]) {
                add_points_to_result(node->child[i], results);
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
}

