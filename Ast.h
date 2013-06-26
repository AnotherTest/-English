#ifndef _NOT_ENGLISH_AST_H_INCLUDE_GUARD
#define _NOT_ENGLISH_AST_H_INCLUDE_GUARD

#include "TokenStream.h"
#include <vector>
#include <memory>

namespace Ast {

    class Value {
        boost::any value;
    public:
        Value()
            : value() { }

        template<class T>
        Value(const T& v)
            : value(v) { }

        template<class T>
        void set(const T& v)
        {
            value = v;
        }

        template<class T>
        T get() const
        {
            try {
                return boost::any_cast<T>(value);
            } catch(...) {
                throw std::runtime_error("type violation of Value");
            }
        }
    };

    class Node {
    protected:
        TokenType type;
    public:
        Node(const TokenType& t = TokenType::Unkown)
            : type(t) {}
        virtual Value execute() = 0;
        virtual ~Node() {}
    };

    typedef std::unique_ptr<Node> NodePtr;

    class Block : public Node {
        std::vector<NodePtr> stmnts;
    public:
        Block()
            : Node() {}

        template<class NodeType>
        void attach(NodeType* n)
        {
            stmnts.emplace_back(n);
        }

        Value execute()
        {
            for(auto& n : stmnts)
                n->execute();
            return Value();
        }
    };

    class Expression : public Node {
        NodePtr left;
        NodePtr right;
        char op;
    public:
        Expression()
            : Node(), left(), right() {}

        template<class NodeType>
        Expression(NodeType* l)
            : Node(), left(l), right(), op() {}

        template<class NodeType1, class NodeType2>
        Expression(NodeType1* l, NodeType2* r, char o)
            : Node(), left(l), right(r), op(o) {}
        Value execute()
        {
            Variable vleft = left->execute().get<Variable>();
            if(!right)
                return vleft;
            const Variable vright = right->execute().get<Variable>();
            switch(op) {
                case '+':
                    vleft.apply(AdditionVisitor(), vright);
                    break;
                case '-':
                    vleft.apply(SubtractionVisitor(), vright);
                    break;
                case '*':
                    vleft.apply(MultiplicationVisitor(), vright);
                    break;
                case '/':
                    vleft.apply(DivisionVisitor(), vright);
                    break;
                default:
                    throw std::runtime_error("Invalid operator " + op);
            }
            return Value(vleft);
        }
    };

    class UnaryOp : public Node {
        NodePtr sub;
        char op;
    public:
        UnaryOp()
            : Node(), sub(), op() {}

        template<class NodeType>
        UnaryOp(NodeType* n, char o = '\0')
            : Node(), sub(n), op(o) {}

        Value execute()
        {
            if(op == '-')
                return Value(sub->execute().get<Variable>().apply(UnaryMinusVisitor()));
            else
                return sub->execute();
        }
    };

    class Condition : public Node {
        NodePtr left;
        NodePtr right;
        char op;
    public:
        Condition()
            : Node(), left(), right() {}
        template<class NodeType1, class NodeType2>
        Condition(NodeType1* l, NodeType2* r, char o)
            : Node(), left(l), right(r), op(o) {}
        Value execute()
        {
            Value vleft = left->execute();
            const Value vright = right->execute();
            switch(op) {
                case '&':
                    return Value(vleft.get<bool>() && vright.get<bool>());
                case '|':
                    return Value(vleft.get<bool>() || vright.get<bool>());
                case '=':
                    return Value(Variable::apply<bool>(
                        EqualsVisitor(),
                        vleft.get<Variable>(), vright.get<Variable>()
                    ));
                case '!':
                    return Value(Variable::apply<bool>(
                        NotEqualsVisitor(),
                        vleft.get<Variable>(), vright.get<Variable>()
                    ));
                case '<':
                    return Value(Variable::apply<bool>(
                        SmallerThanVisitor(),
                        vleft.get<Variable>(), vright.get<Variable>()
                    ));
                case '>':
                    return Value(Variable::apply<bool>(
                        GreaterThanVisitor(),
                        vleft.get<Variable>(), vright.get<Variable>()
                    ));
                default:
                    throw std::runtime_error("Invalid operator " + op);
                    return Value();
            }
        }
    };

    class Literal : public Node {
        Value val;
    public:
        Literal()
            : Node(), val() {}
        template<class T>
        Literal(const T& v)
            : Node(), val(v) {}
        Value execute()
        {
            return val;
        }
    };

    class FunctionCall : public Node {
        std::string name;
        std::vector< std::unique_ptr<Expression> > args;
        DataHandler* data;
    public:
        FunctionCall()
            : Node(), name(), args(), data() {}
        FunctionCall(const std::string& n, DataHandler* d)
            : Node(), name(n), data(d) {}

        void addArgument(Expression* arg)
        {
            args.emplace_back(arg);
        }

        Value execute()
        {
            std::vector<Variable> vargs;
            vargs.reserve(args.size());
            for(auto& arg : args)
                vargs.push_back(arg->execute().get<Variable>());
            return Value(data->call(name, vargs));
        }
    };

    class Assignment : public Node {
        DataHandler* data;
        std::string name;
        std::unique_ptr<Expression> value;
    public:
        Assignment()
            : Node(), data(), name(), value() {}

        Assignment(const std::string& n, DataHandler* d, Expression* e)
            : Node(), data(d), name(n), value(e) {}
        Value execute()
        {
            if(data->exists(name))
                data->set(name, value->execute().get<Variable>());
            else
                throw std::runtime_error("Undefined variable " + name + " used.");
            return Value();
        }
    };

    class VarDeclaration : public Node {
        DataHandler* data;
        std::string name;
    public:
        VarDeclaration()
            : Node(), data(), name() {}

        VarDeclaration(const std::string& n, DataHandler* d)
            : Node(), data(d), name(n)  {}
        Value execute()
        {
            if(!data->exists(name))
                data->set(name, Variable());
            else
                throw std::runtime_error("Variable " + name + " double declared.");
            return Value();
        }
    };

    class VarNode : public Node {
        DataHandler* data;
        std::string name;
    public:
        VarNode()
            : Node(), data(), name() {}

        VarNode(const std::string& n, DataHandler* d)
            : Node(), data(d), name(n) {}
        Value execute()
        {
            if(data->exists(name))
                return Value(data->get(name));
            throw std::runtime_error("Undefined variable " + name + " used.");
        }
    };

    class IfStatement : public Node {
        std::unique_ptr<Condition> condition;
        std::unique_ptr<Block> body_if;
        std::unique_ptr<Block> body_else;
    public:
        IfStatement()
            : Node(), condition(), body_if(), body_else() {}
        IfStatement(Condition* c, Block* bi, Block* be)
            : Node(), condition(c), body_if(bi), body_else(be) {}
        Value execute()
        {
            if(body_else && condition->execute().get<bool>())
                body_if->execute();
            else
                body_else->execute();
            return Value();
        }
    };

    class WhileStatement : public Node {
        std::unique_ptr<Condition> condition;
        std::unique_ptr<Block> body;
    public:
        WhileStatement()
            : Node(), condition(), body() {}
        WhileStatement(Condition* c, Block* b)
            : Node(), condition(c), body(b) {}
        Value execute()
        {
            while(condition->execute().get<bool>())
                body->execute();
            return Value();
        }
    };
}
#endif // _NOT_ENGLISH_AST_H_INCLUDE_GUARD

