#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <optional>

struct Value { // TODO
    std::optional<std::string> Name;
    std::optional<int> Value;
    bool isAddr = false;
};
namespace AST {

    class Node;
    class VarNode;
    class ArrayNode;
    class DummyNode;
    class NumNode;
    class ScopeNode;
    class OperNode;


    struct Visitor {
        using ret_type = Value;
        virtual Value visit (VarNode *node) = 0;
        virtual Value visit (ArrayNode *node) = 0;
        virtual Value visit (DummyNode *node) = 0;
        virtual Value visit (NumNode *node) = 0;
        virtual Value visit (OperNode *node) = 0;
        virtual Value visit (ScopeNode *node) = 0;

        virtual ~Visitor() = default;
    };

    struct Node : private std::vector<Node*> {
        Node (yy::location loc = yy::location{})
            : location_(loc) {}
        
        virtual ~Node () = default;

        Node (const Node &other) = delete;
        Node (Node &&other) = delete;
        Node &operator= (const Node &other) = delete;
        Node &operator= (Node &&other) = delete;

        virtual Visitor::ret_type accept(Visitor &v) = 0;

        virtual void nodeDump (std::ostream &out) const = 0;

        using vector::back;
        using vector::begin;
        using vector::cbegin;
        using vector::cend;
        using vector::crbegin;
        using vector::crend;
        using vector::end;
        using vector::front;
        using vector::size;
        using vector::push_back;
    protected:
        yy::location location_;
    };
}  // namespace AST

namespace AST {

    class VarNode final : public Node {
        std::string name_;
        int Val;

    public:
        VarNode (const std::string &name, yy::location loc)
            : Node (loc), name_ (name)
        {
        }

        auto accept(Visitor &v) -> decltype (v.visit(this)) override{
            return v.visit(this);
        }

        void nodeDump (std::ostream &out) const override { out << name_; }

        std::string getName () const { return name_; }

        void setValue(int Value) { Val = Value; }
    };

    class DummyNode final : public Node { // TODO
    public:
        DummyNode (yy::location loc = yy::location{})
            : Node (loc)
        {
        }

        auto accept(Visitor &v) -> decltype (v.visit(this)) override {
            return v.visit(this);
        }

        void nodeDump (std::ostream &out) const override { "DUMMY"; }
    };

    class ArrayNode final : public Node {
    public:
        ArrayNode (yy::location loc = yy::location{})
            : Node (loc) {}

        auto accept(Visitor &v) -> decltype (v.visit(this)) override{
            return v.visit(this);
        }

        void nodeDump (std::ostream &out) const override { out << "ARRAY"; }
    };

    class OperNode final : public Node {
    public:
        enum class OperType;

    private:
        OperType opType_;

    public:
        enum class OperType {
            ADD,  // a + b
            SUB,  // a - b

            ASSIGN,  // a = b

            INPUT,   // a = ?
            PRINT,  // print (a)
        };

        OperNode (const OperType opType, yy::location loc)
            : Node (loc), opType_ (opType)
        {
        }

        auto accept(Visitor &v) -> decltype (v.visit(this)) override{
            return v.visit(this);
        }

        OperNode (const OperType opType) :  opType_ (opType) {}

        OperType getOpType () const { return opType_; }

        void nodeDump (std::ostream &out) const override  {
            switch (opType_) {
                case OperType::ADD: out << "ADD (+)"; break;
                case OperType::SUB: out << "SUB (-)"; break;
                case OperType::ASSIGN: out << "ASSIGN (=)"; break;
                case OperType::INPUT: out << "INPUT [input ()]"; break;
                case OperType::PRINT: out << "PRINT [print ()]"; break;
                default: out << "Unexpected operator type!";
            }
        }
    };

    class NumNode final : public Node {
        int value_;

    public:
        NumNode (const int value = 0) : value_ (value) {}

        auto accept(Visitor &v) -> decltype (v.visit(this)) override{
            return v.visit(this);
        }

        void nodeDump (std::ostream &out) const override { out << value_; }

        int getValue () const { return value_; }
    };

    class ScopeNode final : public Node {
    public:
        ScopeNode (yy::location loc = yy::location{}) : Node (loc) {}

        auto accept(Visitor &v) -> decltype (v.visit(this)) override{
            return v.visit(this);
        }

        void nodeDump (std::ostream &out) const override { out << "SCOPE"; }
    };
}  // namespace AST