#ifndef _TOKEN_HANDLER_GUARD
#define _TOKEN_HANDLER_GUARD

#include "TokenStream.h"
#include "DataHandler.h"
#include <vector>
#include <functional>
#include "Ast.h"

/**
 * Creates an AST from a ::TokenStream.
 */
class Parser {
    typedef std::function<void(Parser*)> Handler;
    typedef std::map<TokenType, Handler> HandlerMap;

    TokenStream& ts;
    TokenStream::iterator current;
    DataHandler& data_handler;
    HandlerMap handlers;
    Ast::Block* program;

    /**
     * Gets a ::Token from the ::TokenStream but skips one optional token of
     * a given type.
     * @param type the type of ::Token to skip
     */
    void skipOptional(TokenType type);

    void readBlock(std::vector<Token>& tokens, TokenType begin = TokenType::BlockBegin);

    /**
     * Insert a ::Token before current + offset.
     */
    void insert(const Token& t, int offset = 1);

    /**
     * Handles an unexpected token in Parser::handleToken.
     * @see Parser::handleToken
     */
    void handleUnexpectedToken();

    bool handleToken();
    void handleIdentifier();
    void handleFuncImpl();
    void handle_declaration();
    void handle_setvar();
    void handle_if();
    void handle_while();
    void handle_while_run();
    /**
     * Parses the next tokens as expected for an Ast::FunctionCall.
     * @param in_expr determines whether this function call should be seen
     *  as a separate statement (if so, false)
     */
    Ast::FunctionCall* handleFunctionCall(bool in_expr = true);
    Ast::Expression* expression();
    Ast::Expression* term();
    Ast::UnaryOp* primary();
    Ast::Condition* condition_term();
    Ast::Condition* condition();

    void setupHandlers();
public:
    Parser(TokenStream& tokens, DataHandler& data);
    Ast::Block* run();
};
#endif
