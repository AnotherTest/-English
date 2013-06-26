#ifndef _TOKEN_HANDLER_GUARD
#define _TOKEN_HANDLER_GUARD
#include "TokenStream.h"
#include "DataHandler.h"
#include <vector>
#include <functional>
#include <stack>
#include "Ast.h"

class TokenHandler {
    typedef std::function<void(TokenHandler*)> Handler;
    typedef std::map<TokenType, Handler> HandlerMap;

    TokenStream ts;
    TokenStream::iterator current;
    Lexer lexer;
    DataHandler data_handler;
    HandlerMap handlers;
    Block* program;

    /**
     * Gets a ::Token from the ::TokenStream but skips one optional token of
     * a given type.
     * @param type the type of ::Token to skip
     */
    void skipOptional(TokenType type);

    void read_block(std::vector<Token>& tokens, TokenType begin = TokenType::BlockBegin);

    /**
     * Insert a ::Token before current + offset.
     */
    void insert(const Token& t, int offset = 1);
    void insertBlock(const TokenStream& tokens);

    /**
     * Handles an unexpected token in TokenHandler::handleToken.
     * @see TokenHandler::handleToken
     */
    void handleUnexpectedToken();

    bool handleToken();
    void handleIdentifier();
    void handle_declaration();
    void handle_setvar();
    void handle_if();
    void handle_while();
    void handle_while_run();
    /**
     * Parses the next tokens as expected for a ::FunctionCall.
     * @param in_expr determines whether this function call should be seen
     *  as a separate statement (if so, false)
     */
    FunctionCall* handleFunctionCall(bool in_expr = true);
    Expression* expression();
    Expression* term();
    UnaryOp* primary();
    Condition* condition_term();
    Condition* condition();

    void setupHandlers();
public:
    TokenHandler(const TokenStream& tokens);
    TokenHandler(const char* filename);
    Block* run();
};
#endif
