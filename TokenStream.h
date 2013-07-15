#ifndef _TOKENSTREAMH_GUARD
#define _TOKENSTREAMH_GUARD
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <boost/any.hpp>

/**
 * Display an error message and throw an exception.
 */
void error(const std::string& msg, int line);

enum class TokenType {
    Unkown,
    Begin, Declaration,
    SetVar, ValueOf,
    Error, End,
    Article, Or, And,
    To, KnownAs,
    Identifier, String,
    Number, Operator, Dot, // 14, 15, 16
    Plus, Minus, Times, // These are only used in TokenStream
    If, Else,
    Equals, NotEquals, BlockEnd,
    BlockBegin, Is, FuncName,
    FuncResult, On, Of,
    While, //BlockBeginW,
    WhileCondition, WhileBody,
    Comment, Argument, When,
    Calling
};

class Token {
    boost::any value;
public:
    TokenType type;
    int line;

    Token()
        : value(), type(TokenType::Unkown), line(0) { }

    Token(TokenType type)
        : value(), type(type), line(0) { }

    template<class T>
    Token(const T& v, TokenType type)
        : value(v), type(type), line(0) { }

    template<class T>
    void setValue(const T& v)
    {
        value = v;
    }

    template<class T>
    T getValue() const
    {
        return boost::any_cast<T>(value);
    }
};

/**
 * Maps words (as std::string) to ::TokenType objects.
 */
class TokenTable {
    std::map<std::string, TokenType> table;
public:
    TokenTable() = default;

    void add(TokenType t, const std::string& word);

    template<class... Rest>
    void add(TokenType t, const std::string& word, const Rest&... words)
    {
        add(t, word);
        add(t, words...);
    }

    TokenType operator[](const std::string& word);
};

typedef std::vector<Token> TokenStream;

class Lexer {
    std::string filepath;
    std::ifstream ifs;
    TokenTable type_table;
    TokenStream tokens;

    char readChar(bool ws);

    void readString(std::string& str);

    double readNumber();

    void skipSentence();

    /**
     * Makes a comparions operator ::Token.
     */
    Token makeComparison(std::string& text);

    /**
     * Makes a function call ::Token.
     */
    Token makeFunctionCall(std::string& text);

    /**
     * Extracts a ::Token beginning with a word from the ::Lexer
     */
    Token getTxt();

    Token get();

    void open();
public:
    int line;

    Lexer(const std::string& filename);

    TokenStream tokenize();
};
#endif
