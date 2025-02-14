# advanced-data
1. 什么是高级数据结构呢？简单来说就是你了解的/使用的，诸如此类，跳表，图，大/小根堆，桶，哈希表，红黑树...
***
但不包括*内存池，线程池，进程池，对象池*，理由：这些“池”技术主要是为了实现资源的管理和复用，提高系统性能和资源利用率，属于系统资源管理和优化的范畴，不是传统意义上的数据结构，但在实现这些“池”的过程中，可能会用到各种数据结构来进行管理和组织。
---
## 跳表- skip list
1. 定义：跳表是一种随机化的数据结构，它基于链表，通过在链表节点中增加多级索引来提高查找、插入和删除操作的效率。在跳表中，每个节点都有多个指针，这些指针跨越不同数量的节点，使得可以快速跳过一些节点进行查找等操作，其时间复杂度平均为O(log n)，可以看作是一种能快速查找的有序链表扩展结构。

## 大小根堆- Max/Min Heap
1. 定义：大根堆是一种完全二叉树，其中每个节点的值都大于或等于其子节点的值，根节点的值是整个堆中的最大值；小根堆同样是完全二叉树，不过每个节点的值都小于或等于其子节点的值，根节点的值是整个堆中的最小值。大小根堆常用于实现优先队列等数据结构，能够快速获取最大或最小元素。
***
### 1.1 节点类 (Node)
```cpp
/*
 * 跳表节点类
 * T 节点存储的数据类型
 */
template <typename T>
class Node {
public:
    T value;            // 节点存储的值
    std::vector<Node*> forward; // 各层的前向指针数组
    /*
     * 构造函数
     *  value 节点值
     * level 节点所在的最大层级
     */
    Node(T value, int level) : value(value), forward(level + 1, nullptr) {}
};
```
**私有成员说明**:
- `value`: 存储节点的实际数值
- `forward`: 指针数组，`forward[i]`表示在第i层指向的下一个节点
---
1.2 跳表类 (SkipList)
```cpp
template <typename T>
class SkipList {
private:
    Node<T>* header;    // 头节点（哑节点）
    int max_level;      // 允许的最大层数
    int current_level;  // 当前实际使用最大层数
    float probability;  // 节点晋升概率
    
    int random_level(); // 生成随机层数
};
```
**私有成员说明**：
- `header`: 头节点，不存储实际数据，作为搜索起点
- `max_level`: 跳表允许创建的最大层数（防止内存过度消耗）
- `current_level`: 当前实际使用的最大层数（动态调整）
- `probability`: 节点晋升到上一层的概率（通常为0.5）
---
核心操作实现
---
2.1 插入操作
```cpp
template <typename T>
void SkipList<T>::insert(T value) {
    std::vector<Node<T>*> update(max_level + 1, nullptr);
    Node<T>* current = header;
    // 从最高层开始查找插入位置
    for (int i = current_level; i >= 0; --i) {
        while (current->forward[i] && 
              current->forward[i]->value < value) {
            current = current->forward[i];
        }
        update[i] = current; // 记录每层需要更新的节点
    }
    // 生成新节点层数
    int new_level = random_level();
    // 如果新节点层数超过当前最大层
    if (new_level > current_level) {
        for (int i = current_level + 1; i <= new_level; ++i) {
            update[i] = header; // 超过当前层的上层都指向header
        }
        current_level = new_level; // 更新当前最大层
    }
    // 创建新节点
    Node<T>* newNode = new Node<T>(value, new_level);
    // 更新各层指针
    for (int i = 0; i <= new_level; ++i) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}
```
2.2 删除操作
```cpp
template <typename T>
void SkipList<T>::erase(T value) {
    std::vector<Node<T>*> update(max_level + 1, nullptr);
    Node<T>* current = header;
    // 查找需要删除的节点
    for (int i = current_level; i >= 0; --i) {
        while (current->forward[i] && 
              current->forward[i]->value < value) {
            current = current->forward[i];
        }
        update[i] = current; // 记录各层需要更新的节点
    }
    current = current->forward[0];
    // 验证找到的节点是否为要删除的节点
    if (current && current->value == value) {
        // 更新各层指针
        for (int i = 0; i <= current_level; ++i) {
            if (update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }
        // 删除节点后可能需要更新current_level
        while (current_level > 0 && 
              header->forward[current_level] == nullptr) {
            current_level--;
        }
        delete current;
    }
}
```
3. 测试用例
```cpp
#include <iostream>
int main() {
    SkipList<int> skl(3, 0.5); // 最大3层，晋升概率0.5
    // 插入测试
    skl.insert(3);
    skl.insert(6);
    skl.insert(7);
    skl.insert(9);
    skl.insert(12);
    // 查找测试
    std::cout << "Contains 6: " << skl.contains(6) << std::endl; // 1
    std::cout << "Contains 5: " << skl.contains(5) << std::endl; // 0
    // 删除测试
    skl.erase(6);
    skl.erase(9);
    std::cout << "Contains 6: " << skl.contains(6) << std::endl; // 0
    return 0;
}
```
- 支持C++11标准的编译器
- 推荐使用g++ 7.0+(更新的编译器) 或 clang++ 5.0+(更新的编译器)
---
**编译命令**：
```bash
g++ -std=c++11 -O2 -o skl_test skip_list.cpp
```
***
## 桶- Bucket 
1. 定义：桶通常是在桶排序或其他一些算法中使用的概念，它可以理解为一个容器，用于存放具有一定特征或范围的数据元素。在桶排序中，会根据数据的范围将数据分配到不同的桶中，然后再对每个桶内的数据进行单独处理，比如排序等操作，最后将各个桶中的数据按顺序合并起来得到最终结果。

## 哈希表- Hash Table/Map 
1. 定义：哈希表是一种数据结构，它通过一个哈希函数将数据的键值映射到一个固定大小的数组中，以实现快速的查找、插入和删除操作。当要存储或查找一个数据时，先通过哈希函数计算出该数据的键对应的哈希值，这个哈希值作为数组的索引，将数据存储在对应的位置或从该位置获取数据。哈希表可以在较短时间内完成这些操作，平均时间复杂度通常为O(1)

