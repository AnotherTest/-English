#ifndef _NOT_ENGLISH_AST_H_INCLUDE_GUARD
#define _NOT_ENGLISH_AST_H_INCLUDE_GUARD

#include "TokenStream.h"
#include "DataHandler.h"
#include <vector>
#include <deque>
#include <memory>
#include <sstream>

namespace Ast {

    class Node {
    protected:
        TokenType type;
    public:
        Node(const TokenType& t = TokenType::Unkown)
            : type(t) {}
        virtual VarPtr execute() = 0;
        virtual void cleanup() {};
        virtual ~Node() {}
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
    };

    typedef std::unique_ptr<Node> NodePtr;

    class Block : public Node {
        std::deque<NodePtr> stmnts;
        DataHandler* data;
        bool scope;
    public:
        Block(DataHandler* d)
            : Node(), stmnts(), data(d), scope(false) {}

        template <class NodeType>
        void prepend(NodeType* n)
        {
            stmnts.emplace_front(n);
        }

        template <class NodeType>
        void attach(NodeType* n)
        {
            stmnts.emplace_back(n);
        }

        void premakeScope()
        {
            data->addScope();
            scope = true;
        } 

        VarPtr execute()
        {
            if(!scope)
                data->addScope();
            for(auto& n : stmnts)
                n->execute();
            cleanup(); // Execution done, cleanup
            return VarPtr();
        }

        void cleanup()
        {
            for(auto& n : stmnts)
                n->cleanup();
            data->popScope();
            scope = false;
        }
    };

    class Expression : public Node {
        NodePtr left;
        NodePtr right;
        char op;
    public:
        Expression()
            : Node(), left(), right(), op() {}

        template<class NodeType>
        Expression(NodeType* l)
            : Node(), left(l), right(), op() {}

        template<class NodeType1, class NodeType2>
        Expression(NodeType1* l, NodeType2* r, char o)
            : Node(), left(l), right(r), op(o) {}
        VarPtr execute()
        {
            VarPtr vleft = left->execute();
            if(!right)
                return vleft;
            const VarPtr vright = right->execute();
            switch(op) {
                case '+':
                    return Variable::apply(AdditionVisitor(), *vleft, *vright).clone();
                case '-':
                    return Variable::apply(SubtractionVisitor(), *vleft, *vright).clone();
                case '*':
                    return Variable::apply(MultiplicationVisitor(), *vleft, *vright).clone();
                case '/':
                    return Variable::apply(DivisionVisitor(), *vleft, *vright).clone();
                default:
                    std::stringstream ss("Invalid operator ");
                    ss << op;
                    throw std::runtime_error(ss.str().c_str());
                    return VarPtr();
                }
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

        VarPtr execute()
        {
            if(op == '-')
                return Variable::apply(UnaryMinusVisitor(), *sub->execute()).clone();
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
            : Node(), left(), right(), op() {}
        template<class NodeType1, class NodeType2>
        Condition(NodeType1* l, NodeType2* r, char o)
            : Node(), left(l), right(r), op(o) {}
        VarPtr execute()
        {
            VarPtr vleft = left->execute();
            const VarPtr vright = right->execute();
            switch(op) {
                case '&':
                    return Variable::apply(AndVisitor(), *vleft, *vright).clone();
                case '|':
                    return Variable::apply(OrVisitor(), *vleft, *vright).clone();
                case '=':
                    return Variable::apply(EqualsVisitor(), *vleft, *vright).clone();
                case '!':
                    return Variable::apply(NotEqualsVisitor(), *vleft, *vright).clone();
                case '<':
                    return Variable::apply(SmallerThanVisitor(), *vleft, *vright).clone();
                case '>':
                    return Variable::apply(GreaterThanVisitor(), *vleft, *vright).clone();
                default:
                    std::stringstream ss("Invalid operator ");
                    ss << op;
                    throw std::runtime_error(ss.str().c_str());
                    return VarPtr();
            }
        }
    };

    class Literal : public Node {
        VarPtr val;
    public:
        Literal()
            : Node(), val() {}
        
        Literal(VarPtr p)
            : Node(), val(p) {}

        template<class T>
        Literal(const T& v)
            : Node(), val(new Variable(v)) {}
        VarPtr execute()
        {
            return val;
        }
    };

    class FunctionCall : public Node {
        std::string name;
        std::vector< std::unique_ptr<Expression> > args;
        DataHandler* data;
    public:
        FunctionCall(const std::string& n, DataHandler* d)
            : Node(), name(n), data(d) {}

        void addArgument(Expression* arg)
        {
            args.emplace_back(arg);
        }

        VarPtr execute()
        {
            if(!data->funcExists(name)) {
                throw std::runtime_error("use of nonexistant function " + name);
                return VarPtr();
            }
            std::vector<VarPtr> vargs;
            vargs.reserve(args.size());
            for(auto& arg : args)
                vargs.push_back(arg->execute());
            return data->call(name, vargs);
        }

        // Cleanup is handled by the ::DataHandler
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
        VarPtr execute()
        {
            if(data->varExists(name))
                data->set(name, value->execute());
            else
                throw std::runtime_error("Undefined variable " + name + " used.");
            return VarPtr();
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

        VarPtr execute()
        {
            if(!data->varExists(name))
                data->addVar(name);
            else
                throw std::runtime_error("Variable " + name + " double declared.");
            return VarPtr();
        }

        void cleanup()
        {
            data->delVar(name);
        }
    };

    class FuncDeclaration : public Node {
        DataHandler* data;
        std::string name;
        std::vector<std::string> args;
    public:
        FuncDeclaration()
            : Node(), data(nullptr), name(), args() {}

        FuncDeclaration(const std::string& n, DataHandler* d)
            : Node(), data(d), name(n), args()  {}

        void addArg(const std::string& name)
        {
            args.push_back(name);
        }

        VarPtr execute()
        {
            if(!data->funcExists(name))
                data->addFunc(name, args);
            else
                throw std::runtime_error("Function " + name + " double declared.");
            return VarPtr();
        }

        void cleanup()
        {
            data->delFunc(name);
        }
    };

    class FuncImpl : public Node {
        DataHandler* data;
        std::string name;
        std::unique_ptr<Block> body;
    public:
        FuncImpl()
            : Node(), data(nullptr), name(), body(nullptr) {}

        FuncImpl(const std::string& n, DataHandler* d, Block* b)
            : Node(), data(d), name(n), body(b)  {}

        VarPtr execute()
        {
            if(data->funcExists(name))
                data->getFunc(name).setBody(body.get());
            else
                throw std::runtime_error("Undefined function " + name + " used.");
            return VarPtr();
        }
    };

    class VarNode : public Node {
        DataHandler* data;
        std::string name;
    public:
        VarNode()
            : Node(), data(nullptr), name() {}

        VarNode(const std::string& n, DataHandler* d)
            : Node(), data(d), name(n) {}
        VarPtr execute()
        {
            if(data->varExists(name))
                return data->getVar(name);
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
        VarPtr execute()
        {
            if(condition->execute()->getValue<Variable::BoolType>())
                body_if->execute();
            else if(body_else)
                body_else->execute();
            return VarPtr();
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
        VarPtr execute()
        {
            while(condition->execute()->getValue<Variable::BoolType>())
                body->execute();
            return VarPtr();
        }
    };
}
#endif // _NOT_ENGLISH_AST_H_INCLUDE_GUARD

