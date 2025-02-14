#include <iostream>
#include <vector>

template <typename T> class Node {
public:
  T value;                     // 节点存储的值
  std::vector<Node *> forward; // 各层的前向指针数组
  /*
   * 构造函数
   *  value 节点值
   * level 节点所在的最大层级
   */
  Node(T value, int level) : value(value), forward(level + 1, nullptr) {}
};

template <typename T> class SkipList {
public:
  SkipList(int max_level = 16, float p = 0.5)
      : max_level(max_level), current_level(0), probability(p),
        header(new Node<T>(T(), max_level)) {}
  void insert(T value);
  void erase(T value);
  bool contains(T value);

private:
  Node<T> *header;   // 头节点（哑节点）
  int max_level;     // 允许的最大层数
  int current_level; // 当前实际使用最大层数
  float probability; // 节点晋升概率

  int random_level(); // 生成随机层数
};

template <typename T> bool SkipList<T>::contains(T value) {
  Node<T> *current = header;

  // 从最高层开始逐层搜索
  for (int i = current_level; i >= 0; --i) {
    // 在当前层向右搜索
    while (current->forward[i] && current->forward[i]->value < value) {
      current = current->forward[i];
    }
  }

  // 最终到达最底层（第0层）的下一个节点
  current = current->forward[0];

  // 验证是否为查找的值
  return current != nullptr && current->value == value;
}
template <typename T> void SkipList<T>::insert(T value) {
  std::vector<Node<T> *> update(max_level + 1, nullptr);
  Node<T> *current = header;
  // 从最高层开始查找插入位置
  for (int i = current_level; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->value < value) {
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
  Node<T> *newNode = new Node<T>(value, new_level);
  // 更新各层指针
  for (int i = 0; i <= new_level; ++i) {
    newNode->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = newNode;
  }
}

template <typename T> void SkipList<T>::erase(T value) {
  std::vector<Node<T> *> update(max_level + 1, nullptr);
  Node<T> *current = header;
  // 查找需要删除的节点
  for (int i = current_level; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->value < value) {
      current = current->forward[i];
    }
    update[i] = current; // 记录各层需要更新的节点
  }
  current = current->forward[0];
  // 验证找到的节点是否为要删除的节点
  if (current && current->value == value) {
    // 更新各层指针
    for (int i = 0; i <= current_level; ++i) {
      if (update[i]->forward[i] != current)
        break;
      update[i]->forward[i] = current->forward[i];
    }
    // 删除节点后可能需要更新current_level
    while (current_level > 0 && header->forward[current_level] == nullptr) {
      current_level--;
    }
    delete current;
  }
}

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