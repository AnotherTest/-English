#include "TokenHandler.h"
#include <stdexcept>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <iostream>

void TokenHandler::setupHandlers()
{
    handlers[TokenType::Declaration] = &TokenHandler::handle_declaration;
    handlers[TokenType::SetVar] = &TokenHandler::handle_setvar;
    handlers[TokenType::If] = &TokenHandler::handle_if;
    handlers[TokenType::While] = &TokenHandler::handle_while;
    handlers[TokenType::Identifier] = &TokenHandler::handleIdentifier;
}

TokenHandler::TokenHandler(const TokenStream& tokens)
    : ts(tokens), current(), lexer(""), data_handler(DataHandler()), handlers(),
      program(new Block)
{
    setupHandlers();
}

TokenHandler::TokenHandler(const char* filename)
    : ts(), current(), lexer(filename), data_handler(DataHandler()), handlers(),
      program(new Block)
{
    setupHandlers();
}

Block* TokenHandler::run()
{
    if(ts.empty())
        ts = lexer.tokenize();
    current = ts.begin();
    while(handleToken());
    return program;
}

void TokenHandler::skipOptional(TokenType type)
{
    ++current;
    if(current->type != type)
        --current;
}

void TokenHandler::read_block(std::vector<Token>& tokens, TokenType begin)
{
    ++current;
    if(current->type != begin)
        error("expecting a 'then:' or perhaps 'do:' as a block beginning", current->line);

    int to_find = 1;
    while(to_find > 0) {
        ++current;
        if(current->type == TokenType::BlockEnd)
            --to_find;
        else if(current->type == TokenType::BlockBegin)// || current->type == TokenType::BlockBeginW)
            ++to_find;
        tokens.push_back(*current);
    }
    // We read a TokenType::BlockEnd that we do not need(other ones are needed for nesting though)
    tokens.pop_back();
    // Skip the following TokenType::Dot
    ++current;
}

void TokenHandler::insert(const Token& t, int offset)
{
    current = ts.insert(current + offset, t);
    current -= offset;
}


void TokenHandler::insertBlock(const TokenStream& tokens)
{
    // Insert one by one to avoid iterator invalidation
    for(auto it = tokens.rbegin(); it != tokens.rend(); ++it)
        current = ts.insert(current + 1, *it);
    current -= tokens.size();
}


void TokenHandler::handleUnexpectedToken()
{
    error(
        "unexpected token (id " + boost::lexical_cast<std::string>(
        static_cast<int>(current->type)) + ")", current->line
    );
}

bool TokenHandler::handleToken() {
    switch(current->type) {
        case TokenType::Begin:
        case TokenType::End:
        case TokenType::Error:
            return false;
        case TokenType::FuncName:
            handleFunctionCall();
            break;
        default: {
            auto handler = handlers.find(current->type);
            if(handler == handlers.end())
                handleUnexpectedToken();
            else
                (handler->second)(this);
        }
    }
    ++current;
    if(current->type != TokenType::Dot)
        error("sentences are usually ended with a dot", current->line);
    ++current;
    return true;
}

void TokenHandler::handleIdentifier()
{
    program->attach(handleFunctionCall(false));
}


void TokenHandler::handle_declaration() {
    // Assuming an article here
    ++current;
    if(current->type != TokenType::Article)
        error("article required before this variable noun", current->line);
    // Expecting a variable now
    ++current;
    if(current->type != TokenType::Identifier)
        return error("expecting a name on declaration", current->line);
    program->attach(new VarDeclaration(
        current->getValue<std::string>(), &data_handler
    ));
}

void TokenHandler::handle_setvar() {
    // Assuming an (optional) article here
    skipOptional(TokenType::Article);
    // Expecting an (optional) value now
    skipOptional(TokenType::ValueOf);
    // Expecting a name OR an (optional) article
    skipOptional(TokenType::Article);
    ++current;
    if(current->type != TokenType::Identifier)
        error("expecting a name that contains the value", current->line);
    const std::string name = current->getValue<std::string>();
    // Expecting a to now
    ++current;
    if(current->type != TokenType::To)
        error("expecting to after the name", current->line);
    // Now we need to read an expression and set the name with it
    program->attach(new Assignment(name, &data_handler, expression()));
}

void TokenHandler::handle_if() {
    // Read the condition first
    Condition* if_cond = condition();
    // Read a block
    std::vector<Token> tokens;
    read_block(tokens);
    Block* if_body = TokenHandler(tokens).run();
    Block* else_body;
    // Read a possible else
    if((current + 1)->type == TokenType::Else) {
        ++current;
        std::vector<Token> tokens2;
        read_block(tokens2);
        else_body = TokenHandler(tokens2).run();
    }
    insert(Token(TokenType::Dot));
    program->attach(new IfStatement(if_cond, if_body, else_body));
}

void TokenHandler::handle_while() {
    // Read the condition first
    Condition* cond = condition();
    // Read the body tokens
    std::vector<Token> tokens;
    read_block(tokens, TokenType::BlockBegin);
    Block* body = TokenHandler(tokens).run();
    insert(Token(TokenType::Dot));
    program->attach(new WhileStatement(cond, body));
}

FunctionCall* TokenHandler::handleFunctionCall(bool in_expr)
{
    // Get the function name
    const std::string name = current->getValue<std::string>();
    FunctionCall* call = new FunctionCall(name, &data_handler);
    if(in_expr) {
        // If we don't find a TokenType::On now, we return the result
        if((current + 1)->type != TokenType::On)
            return call;
        ++current;
    }
    // We found a TokenType::On, read all arguments(separated by "and")
    while(true) {
        call->addArgument(expression());
        ++current;
        if(current->type != TokenType::Operator || current->getValue<char>() != '&') {
            --current;
            break;
        }
    }
    return call;
}
// handlers(handle_*)
// // // // // // // // //


// // // // // // //
// expression handling
// // // // // // //

UnaryOp* TokenHandler::primary()
{
    ++current;
    switch(current->type) {
        case TokenType::String:
            return new UnaryOp(new Literal(Variable(current->getValue<Variable::StringType>())));
        case TokenType::Number:
            return new UnaryOp(new Literal(Variable(current->getValue<Variable::NumberType>())));
        case TokenType::Article:
            // We actually expect another primary now
            // because we allow an optional article before a primary
            return primary();
        case TokenType::Identifier:
            return new UnaryOp(new VarNode(current->getValue<std::string>(), &data_handler));
        //case TokenType::FuncName:
        //    return handleFunctionCall();
        case TokenType::Operator: {
            char op = current->getValue<char>();
            if(op == '(') {
                UnaryOp* uop = new UnaryOp(expression());
                ++current;
                if(current->getValue<char>() != ')')
                    error("expected ')' after '('", current->line);
                return uop;
            } else if(op == '-')
                return new UnaryOp(primary(), op);
            else
                error("unexpected operator in primary", current->line);
        }
        default:
            error("primary expected", current->line);
    }
    return nullptr;
}

Expression* TokenHandler::term() {
    UnaryOp* left = primary();
    ++current;
    if(current->type != TokenType::Operator) {
        --current;
        return new Expression(left);
    }
    const char op = current->getValue<char>();
    if(op != '*' && op != '/') {
        --current;
        return new Expression(left);
    }
    return new Expression(left, term(), op);
}

Expression* TokenHandler::expression() {
    Expression* left = term();
    ++current;
    if(current->type != TokenType::Operator) {
        --current;
        return left;
    }
    const char op = current->getValue<char>();
    if(op != '+' && op != '-') {
        --current;
        return left;
    }
    return new Expression(left, expression(), op);
}

Condition* TokenHandler::condition_term() {
    Expression* left = expression();
    ++current;
    if(current->type != TokenType::Operator)
        error("expecting operator in the condition", current->line);

    const char op = current->getValue<char>();
    if(op != '=' && op != '!' && op != '<' && op != '>')
        error("unsupported operator in the condition", current->line);

    return new Condition(left, expression(), op);
}

Condition* TokenHandler::condition()
{
    Condition* left = condition_term();
    ++current;
    if(current->type != TokenType::Operator) {
        --current;
        return left;
    }
    const char op = current->getValue<char>();
    if(op != '&' && op != '|') {
        --current;
        return left;
    }
    return new Condition(left, condition(), op);
}
