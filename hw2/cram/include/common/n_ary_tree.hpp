#pragma once

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>

namespace Tree {

template <typename T> class NAryTree final {
  T *root_ = nullptr;

  void PrintNodeIntoGraphviz(T *curNode, std::ostream &out) const // TODO
  {
    out << "\"" << curNode << "\" [label = \"";
    curNode->nodeDump(out);
    out << "\"]\n";

    for (auto *curChild : *curNode)
      if (curChild)
        PrintNodeIntoGraphviz(curChild, out);
  }

  void BuildConnectionsInGraphviz(T *curNode, std::ostream &out) const {
    for (auto *curChild : *curNode)
      if (curChild)
        out << "\"" << curNode << "\" -> \"" << curChild << "\"\n";

    for (auto *curChild : *curNode)
      if (curChild)
        BuildConnectionsInGraphviz(curChild, out);
  }

public:
  NAryTree(T *root = nullptr) : root_(root) {}

  ~NAryTree() {
    T *curNode = root_;
    if (!root_)
      return;

    std::stack<T *> stack;
    std::vector<T *> queueOnDelete;
    stack.push(curNode);

    while (stack.size()) {
      curNode = stack.top();
      stack.pop();
      queueOnDelete.push_back(curNode);

      auto childrenSt = curNode->begin();
      auto childrenFin = curNode->end();

      while (childrenSt != childrenFin) {
        stack.push(*childrenSt);
        childrenSt = std::next(childrenSt, 1);
      }
    }

    std::reverse(queueOnDelete.begin(), queueOnDelete.end());
    for (auto *Node : queueOnDelete)
      delete Node;
  }

  NAryTree(const NAryTree &) = delete;
  NAryTree(NAryTree &&) = delete;
  NAryTree &operator=(const NAryTree &) = delete;
  NAryTree &operator=(NAryTree &&) = delete;

  void dump(std::ostream &out) const {
    if (!root_)
      return;

    out << "digraph tree {\n"
           "rankdir = \"LR\"\n"
           "node [fontsize=10, shape=box, height=0.5]\n"
           "edge [fontsize=10]\n";

    PrintNodeIntoGraphviz(root_, out);
    BuildConnectionsInGraphviz(root_, out);

    out << "}\n";
  }

  void setRoot(T *root) { root_ = root; }

  T *getRoot() const noexcept { return root_; }
};
} // namespace Tree