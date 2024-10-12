#pragma once

#include <string>
#include <exception>
#include <unordered_map>
#include <deque>
#include <stack>
#include <iostream>

#include "common/n_ary_tree.hpp"

class Interpreter final { // make template
  std::vector<int> Data;
  std::unordered_map<std::string, int> Vars;

  using AddrType = unsigned long long;
  static constexpr auto DefaultRAMSize = 0x1000ull;

public:
  explicit Interpreter(AddrType Size = DefaultRAMSize) : Data(Size, 0) {}

  Value assignValue(Value Lhs, Value Rhs) {
    auto RhsVal = getValue(Rhs);
    if (!Lhs.isAddr)
      return {Lhs.Name, Vars[*Lhs.Name] = RhsVal, false};
    return {std::nullopt, Data[*Lhs.Value] = RhsVal, false};
  }

  void inputVar(std::string Name) {
    std::cin >> Vars[Name]; // handling
  }

  void inputValue(Value Val) {
    if (!Val.isAddr) {
      std::cin >> Vars[*Val.Name];
      return;
    }
    std::cin >> Data[*Val.Value];
  }

  void printValue(Value Val) const {
    if (!Val.isAddr) {
      if (!Val.Name)
        std::cout << *Val.Value << std::endl;
      else
        std::cout << Vars.at(*Val.Name) << std::endl;
      return;
    }
    std::cout << Data[*Val.Value] << std::endl;
  }

  void printAddr(AddrType Addr) const { std::cout << Data[Addr]; }

  bool contains(std::string Name) const { return Vars.contains(Name); }

  int getVal(std::string Name) { return Vars[Name]; }

  int getValue(Value Val) {
    if (!Val.isAddr) {
      if (!Val.Name)
        return *Val.Value;
      return Vars[*Val.Name];
    }
    return Data[*Val.Value];
  }

  struct GetType : public AST::Visitor {
    Interpreter &I;
    GetType(Interpreter &I) : I(I) {}
    Value visit(AST::ScopeNode *ref) override {
      auto beg = ref->crbegin();
      auto end = ref->crend();

      while (beg != end) {
        (*beg)->accept(*this);
        ++beg;
      }
      return {};
    }

    Value visit(AST::VarNode *ref) override {
      if (I.contains(ref->getName()))
        return {ref->getName(), I.getVal(ref->getName()), false};
      return {ref->getName(), std::nullopt, false};
    }
    Value visit(AST::ArrayNode *ref) override {
      auto Base = ref->front()->accept(*this);
      auto Offset = ref->back()->accept(*this);

      if (Base.Name != std::nullopt)
        return {std::nullopt, *Base.Value + *Offset.Value, true};
      else
        return {std::nullopt, *Offset.Value, true};
    }
    Value visit(AST::DummyNode *ref) override { return {}; }
    Value visit(AST::NumNode *ref) override {
      return {std::nullopt, ref->getValue(), false};
    }
    Value visit(AST::OperNode *ref) override {
      if (ref->getOpType() == AST::OperNode::OperType::ASSIGN) {
        auto Lval = ref->front()->accept(*this);
        auto Rval = ref->back()->accept(*this);

        return I.assignValue(Lval, Rval);
      }
      if (ref->getOpType() == AST::OperNode::OperType::PRINT) {
        I.printValue(ref->front()->accept(*this));
        return {};
      }
      if (ref->getOpType() == AST::OperNode::OperType::INPUT) {
        I.inputValue(ref->front()->accept(*this));
        return {};
      }
      if (ref->getOpType() == AST::OperNode::OperType::ADD) {
        auto FirstVal = ref->front()->accept(*this);
        auto SecondVal = ref->back()->accept(*this);

        return {std::nullopt, *FirstVal.Value + *SecondVal.Value, false};
      }
      if (ref->getOpType() == AST::OperNode::OperType::SUB) {
        auto FirstVal = ref->front()->accept(*this);
        auto SecondVal = ref->back()->accept(*this);

        return {std::nullopt, *FirstVal.Value - *SecondVal.Value, false};
      }
      throw std::runtime_error("Unknown operation node");
    }
  };

  template <typename ASTNode_T> void run(Tree::NAryTree<ASTNode_T> &tree) {
    GetType getType{*this};

    ASTNode_T *root = tree.getRoot();
    root->accept(getType);
  }
};