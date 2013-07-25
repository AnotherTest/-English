#ifndef _TOKENSTREAM_GUARD
#define _TOKENSTREAM_GUARD

#include "TokenStream.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

// error handling functions
void error(const std::string& msg, int line = 0)
{
    if(line)
        std::cerr << "Fatal Error: " << msg << " at line " << line << std::endl;
    else
        std::cerr << "Fatal Error: " << msg << std::endl;
    throw std::runtime_error(msg);
}

void TokenTable::add(TokenType t, const std::string& word)
{
    table[word] = t;
}

TokenType TokenTable::operator[](const std::string& word)
{
    return table[word];
}


// Lexer implementation starts here

Lexer::Lexer(const std::string& filename)
    : filepath(filename), ifs(), type_table(), line(1)
{
    ifs.exceptions(std::ifstream::eofbit);
    // TokenType::Declaration words
    type_table.add(TokenType::Declaration,
        "Declare", "Create", "Make", "Construct", "Spawn", "Manufacture",
        "Name", "Label"
    );
    // TokenType::SetVar words
    type_table.add(TokenType::SetVar,
        "Change", "Set", "Vary", "Alter", "Modify", "Adjust"
    );
    // TokenType::ValueOf words
    type_table.add(TokenType::ValueOf, "value");
    // TokenType::Articles
    type_table.add(TokenType::Article, "a", "an", "another", "the");
    // TokenType::Or words
    type_table.add(TokenType::Or, "or");
    // TokenType::And words
    type_table.add(TokenType::And, "and");
    // TokenType::To words
    type_table.add(TokenType::To, "to", "by", "into");
    // TokenType::KnownAs words
    type_table.add(TokenType::KnownAs, "named",  "called", "labeled", "titled");
    // TokenType::End words
    type_table.add(TokenType::End, "Stop", "End", "Quit", "Exit");
    // TokenType::Plus words(not symbols)
    type_table.add(TokenType::Plus, "plus");
    // TokenType::Minus words(not symbols)
    type_table.add(TokenType::Minus, "minus");
    // TokenType::Times words(not symbols)
    type_table.add(TokenType::Times, "times");
    // TokenType::If words
    type_table.add(TokenType::If, "If");
    // TokenType::Else words
    type_table.add(TokenType::Else, "Otherwise", "Else");
    // TokenType::Equals words
    type_table.add(TokenType::Equals, "equals");
    // TokenType::Not words
    type_table.add(TokenType::NotEquals, "differs");
    // TokenType::BlockEnd words
    type_table.add(TokenType::BlockEnd, "That's");
    // TokenType::BlockBegin(W) words
    type_table.add(TokenType::BlockBegin, "then:", "do:");
    // TokenType::Is words(used as operator)
    type_table.add(TokenType::Is, "is");
    // TokenType::FuncName words
    type_table.add(TokenType::FuncName, "Call", "Execute", "Evaluate");
    // TokenType::FuncResult words
    type_table.add(TokenType::FuncResult, "result", "outcome");
    // TokenType::On words
    type_table.add(TokenType::On, "on", "with");
    // TokenType::Of words
    type_table.add(TokenType::Of, "of", "from");
    // TokenType::While words
    type_table.add(TokenType::While, "While");
    // TokenType::Comment words
    type_table.add(TokenType::Comment, "Note", "Notice", "Note:", "Notice:");
    // TokenType::Argument words
    type_table.add(TokenType::Argument,
        "argument", "arguments", "parameter", "parameters"
    );
    // TokenType::When words
    type_table.add(TokenType::When, "When", "Whenever", "Upon");
    // TokenType::Calling words
    type_table.add(TokenType::Calling,
        "calling", "executing", "evaluating", "running"
    );
}

void Lexer::open()
{
    ifs.open(filepath.c_str());
    if(!ifs)
        error("could not open file \"" + filepath + "\".");

}


// Needed for line counting
char Lexer::readChar(bool ws = false)
{
    char ch = ifs.get();
    if(ws) {
        if(ch == '\n') ++line;
        return ch;
    }
    while(isspace(ch)) {
        if(ch == '\n') ++line;
        ch = ifs.get();
    }
    return ch;
}

// We need this for handling dots in ALL strings
void Lexer::readString(std::string& str)
{
    str = "";
    char c = readChar(true);
    while(!isspace(c)) {
        str += c;
        c = readChar(true);
    }
    // Check for dots and semicolons
    if(str.back() == '.' || str.back() == ',') {
        ifs.putback(str.back());
        str.erase(str.end() - 1);
    }
}

// We need this because we don't want to allow numbers like 100.
// (unlike the STL does)
double Lexer::readNumber()
{
    char c = readChar(true);
    std::string result;
    while(true) {
        switch(c) {
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '.':
                result += c;
                break;
            default: {
                // Since STL will ignore the dot, no need to remove it
                // (just add it to the stream again)
                if(result[result.length() - 1] == '.')
                    ifs.putback('.');
                std::stringstream ss;
                ss << result;
                double result;
                ss>>result;
                return result;
            }
        }
        c = ifs.get();
    }
    return .0;
}

void Lexer::skipSentence()
{
    while(readChar(true) != '.');
}

Token Lexer::makeComparison(std::string& text)
{
    Token t(TokenType::Operator);
    readString(text);
    if(text == "larger" || text == "greater")
        t.setValue('>');
    else if(text == "smaller" || text == "less" || text == "lower")
        t.setValue('<');
    else
        return Token(TokenType::Error);
    readString(text);
    if(text != "than")
        return Token(TokenType::Error);
    return t;
}

Token Lexer::makeFunctionCall(std::string& text)
{
    readString(text);
    // Read an optional article before function
    if(type_table[text] == TokenType::Article)
        readString(text);
    if(text != "function" && text != "subroutine" && text != "routine"
       && text != "procedure")
        return Token(TokenType::Error);
    // Read the name now(read it as "name", so that spaces are allowed)
    const Token str = get();
    if(str.type != TokenType::String)
        return Token(TokenType::Error);
    return Token(str.getValue<std::string>(), TokenType::FuncName);
}

Token Lexer::getTxt()
{
    std::string text;
    readString(text);
    // text[0] = std::tolower(text[0]);
    switch(type_table[text]) {
        case TokenType::Declaration: case TokenType::SetVar:
        case TokenType::End:         case TokenType::Article:
        case TokenType::To:          case TokenType::If:
        case TokenType::BlockBegin:  case TokenType::On:
        case TokenType::While:       case TokenType::FuncResult:
        case TokenType::Of:          case TokenType::Argument:
        case TokenType::KnownAs:     case TokenType::When:
        case TokenType::Calling:     case TokenType::Else:
            return Token(type_table[text]);
        case TokenType::FuncName:
            return makeFunctionCall(text);
        case TokenType::ValueOf:
            // Read "of"(hopefully?)
            readString(text);
            if(text != "of")
                return Token(TokenType::Error);
            return Token(TokenType::ValueOf);
        case TokenType::Is:
            return makeComparison(text);
        case TokenType::Plus:
            return Token('+', TokenType::Operator);
        case TokenType::Minus:
            return Token('-', TokenType::Operator);
        case TokenType::Times:
            return Token('*', TokenType::Operator);
        case TokenType::Equals:
            return Token('=', TokenType::Operator);
        case TokenType::NotEquals:
            readString(text);
            if(text != "from")
                return Token(TokenType::Error);
            return Token('!', TokenType::Operator);
        case TokenType::Or:
            return Token('|', TokenType::Operator);
        case TokenType::And:
            return Token('&', TokenType::Operator);
        case TokenType::BlockEnd:
            // Read an it/all now
            readString(text);
            if(text != "all" && text != "it")
                return Token(TokenType::Error);
            return Token(TokenType::BlockEnd);
        case TokenType::Comment:
            skipSentence();
            return get();
        default:
            return Token(text, TokenType::Identifier);
    }

}

Token Lexer::get()
{
    char ch = readChar();
    switch(ch) {
        case '"': {
            std::string value;
            ch = ifs.get();
            while(ch != '"') {
                value += ch;
                ch = ifs.get();
            }
            return Token(value, TokenType::String);
        }
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            ifs.putback(ch);
            return Token(readNumber(), TokenType::Number);
        case '+': case '-': case '*': case '/': case '(': case ')':
            return Token(ch, TokenType::Operator);
        case '.':
            return Token(TokenType::Dot);
        case ',':
            return Token('&', TokenType::Operator);
        default:
            ifs.putback(ch);
            return getTxt();
    }

}

std::vector<Token> Lexer::tokenize()
{
    open();
    std::vector<Token> tokens;
    try {
        while(true) {
            Token t = get();
            t.line = line;
            tokens.push_back(t);
        }
    } catch(std::ifstream::failure) {
        // EOF breaks the loop
    }
    return tokens;
}

#endif
