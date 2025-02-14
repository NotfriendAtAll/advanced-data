#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

template <typename T> class Node {
public:
  T value;
  std::vector<Node *> forward; // 各层的横向指针
  Node(T val, int level) : value(val), forward(level + 1, nullptr) {}
};

template <typename T> class SkipList {
public:
  SkipList(int max_lvl = 16, float p = 0.5)
      : max_level(max_lvl), current_level(0), probability(p) {
    header = new Node<T>(T(), max_level);
    srand(time(0));
  }
  ~SkipList();

  int random_level();
  bool contains(T vallue);
  void insert(T value);
  void erase(T value);

private:
  Node<T> *header;
  int max_level;
  int current_level;
  float probability;
};

template <class T> int SkipList<T>::random_level() {
  int level = 0;
  while ((rand() % 100) < (probability * 100) && level < max_level) {
    level++;
  }
  return level;
}

template <class T> SkipList<T>::~SkipList() {
  Node<T> *current = header->forward[0];
  while (current) {
    Node<T> *temp = current;
    current = current->forward[0];
    delete temp;
  }
  delete header;
}

template <class T> bool SkipList<T>::contains(T value) {
  Node<T> *current = header;
  for (int i = current_level; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->value < value) {
      current = current->forward[i];
    }
  }
  current = current->forward[0];
  return current && current->value == value;
}

template <class T> void SkipList<T>::insert(T value) {
  std::vector<Node<T> *> update(max_level + 1, nullptr);
  Node<T> *current = header;

  for (int i = current_level; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->value < value) {
      current = current->forward[i];
    }
    update[i] = current;
  }
  int new_level = random_level();
  if (new_level > current_level) {
    for (int i = current_level + 1; i <= new_level; ++i) {
      update[i] = header;
    }
    current_level = new_level;
  }

  Node<T> *newNode = new Node<T>(value, new_level);
  for (int i = 0; i <= new_level; ++i) {
    newNode->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = newNode;
  }
}

template <class T> void SkipList<T>::erase(T value) {
  std::vector<Node<T> *> update(max_level + 1, nullptr);
  Node<T> *current = header;
  for (int i = current_level; i >= 0; --i) {
    while (current->forward[i] && current->forward[i]->value < value) {
      current = current->forward[i];
    }
    update[i] = current;
  }

  current = current->forward[0];
  if (current && current->value == value) {
    for (int i = 0; i <= current_level; ++i) {
      if (update[i]->forward[i] != current)
        break;
      update[i]->forward[i] = current->forward[i];
    }
    delete current;
    while (current_level > 0 && header->forward[current_level] == nullptr) {
      current_level--;
    }
  }
}

int main() {
  SkipList<int> skl(3, 0.5);
  skl.insert(3);
  skl.insert(6);
  skl.insert(7);
  skl.insert(9);
  skl.insert(12);

  std::cout << "Contains 6: " << skl.contains(6) << std::endl; // 1
  skl.erase(6);
  std::cout << "Contains 6: " << skl.contains(6) << std::endl; // 0
  return 0;
}
