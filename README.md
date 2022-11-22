# quad_tree

A C++ implementation of QuadTree Data Structure.

## Class templates
`T` is a data type;

`Vertex` is a custom class, which is an alias of `Vec2<long double>` with vector and scalar operations and comparators.

`PairT` is `std::pair<Vertex, T>`.

`ContainerT` is `std::vector<PairT>`, for storing points and data in Tree Nodes.

## Class constructor

```C++
QuadTree<T, PairT, ContainerT>(Vertex center = Vertex{0, 0},
                               Vertex range = Vertex{1, 1},
                               unsigned bucket_size = 1,
                               unsigned depth = 16,
                               bool sort = false);
```

The constructor has default parameters in it. Set each field if you want to increase its performance and features (sort within region).

## Class member functions

### Insertion
```C++
bool insert(const Vertex &point, const T &data);
```

### Update
```C++
bool update(const Vertex &point, const T &data);
```

### Find
```C++
bool contains(const Vertex &point);
```
### Removal
```C++
bool remove(const Vertex &point);
```

### Get data in specified region
```C++
std::vector<std::pair<Vertex, T>> data_in_region(const Vertex &bottom_left, const Vertex &top_right);
```
