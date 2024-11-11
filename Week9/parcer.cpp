#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <unordered_map>
#include <fstream>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR, T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, T_WHILE, T_FOR, T_ASSIGN,
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_SEMICOLON, T_GT, T_LT, T_TRUE, T_FALSE,
    T_EQ, T_NEQ, T_AND, T_OR, T_EOF, T_Cout
};

struct Token {
    TokenType type;
    string value;
    int line;
};

class Lexer {
private:
    string src;
    size_t pos;
    int line;

public:
    Lexer(const string &src) : src(src), pos(0), line(1) {}

    string consumeNumber() {
        size_t start = pos;
        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

    string consumeString() {
        pos++;
        size_t start = pos;
        while (pos < src.size() && src[pos] != '"') pos++;
        if (pos >= src.size()) {
            cout << "Unterminated string literal at line " << line << endl;
            exit(1);
        }
        string str = src.substr(start, pos - start);
        pos++;
        return str;
    }

    char consumeChar() {
        pos++;
        if (pos + 1 >= src.size() || src[pos + 1] != '\'') {
            cout << "Invalid character literal at line " << line << endl;
            exit(1);
        }
        char ch = src[pos];
        pos += 2;
        return ch;
    }

    vector<Token> tokenizer() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];

            if (isspace(current)) {
                if (current == '\n') line++;
                pos++;
                continue;
            }

            if (current == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
                while (pos < src.size() && src[pos] != '\n') pos++;
                continue;
            }

            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }

            if (isalpha(current)) {
                string word = consumeWord();
                if (word == "int") tokens.push_back(Token{T_INT, word, line});
                else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                else if (word == "agar") tokens.push_back(Token{T_IF, word, line});
                else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                else if (word == "while") tokens.push_back(Token{T_WHILE, word, line});
                else if (word == "for") tokens.push_back(Token{T_FOR, word, line});
                else if (word == "true") tokens.push_back(Token{T_TRUE, word, line});
                else if (word == "false") tokens.push_back(Token{T_FALSE, word, line});
                else if (word == "cout") tokens.push_back(Token{T_Cout, word, line});
                else tokens.push_back(Token{T_ID, word, line});
                continue;
            }

            if (current == '"') {
                tokens.push_back(Token{T_STRING, consumeString(), line});
                continue;
            }
            if (current == '\'') {
                tokens.push_back(Token{T_CHAR, string(1, consumeChar()), line});
                continue;
            }

            switch (current) {
                case '=':
                    if (pos + 1 < src.size() && src[pos + 1] == '=') {
                        tokens.push_back(Token{T_EQ, "==", line});
                        pos += 2;
                    } else {
                        tokens.push_back(Token{T_ASSIGN, "=", line});
                        pos++;
                    }
                    break;
                case '!':
                    if (pos + 1 < src.size() && src[pos + 1] == '=') {
                        tokens.push_back(Token{T_NEQ, "!=", line});
                        pos += 2;
                    } else {
                        cout << "Unexpected character: " << current << " at line " << line << endl;
                        exit(1);
                    }
                    break;
                case '&':
                    if (pos + 1 < src.size() && src[pos + 1] == '&') {
                        tokens.push_back(Token{T_AND, "&&", line});
                        pos += 2;
                    } else {
                        cout << "Unexpected character: " << current << " at line " << line << endl;
                        exit(1);
                    }
                    break;
                case '|':
                    if (pos + 1 < src.size() && src[pos + 1] == '|') {
                        tokens.push_back(Token{T_OR, "||", line});
                        pos += 2;
                    } else {
                        cout << "Unexpected character: " << current << " at line " << line << endl;
                        exit(1);
                    }
                    break;
                case '+': tokens.push_back(Token{T_PLUS, "+", line}); pos++; break;
                case '-': tokens.push_back(Token{T_MINUS, "-", line}); pos++; break;
                case '*': tokens.push_back(Token{T_MUL, "*", line}); pos++; break;
                case '/': tokens.push_back(Token{T_DIV, "/", line}); pos++; break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", line}); pos++; break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", line}); pos++; break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", line}); pos++; break;
                case '}': tokens.push_back(Token{T_RBRACE, "}", line}); pos++; break;
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); pos++; break;
                case '>': tokens.push_back(Token{T_GT, ">", line}); pos++; break;
                case '<': tokens.push_back(Token{T_LT, "<", line}); pos++; break;
                default:
                    cout << "Unexpected character: " << current << " at line " << line << endl;
                    exit(1);
            }
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    }
};

class Parser {
private:
    vector<Token> tokens;
    size_t pos;
    unordered_map<string, TokenType> symbolTable;

public:
    Parser(const vector<Token> &tokens) : tokens(tokens), pos(0) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No syntax errors." << endl;
    }

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE ||
            tokens[pos].type == T_STRING || tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        } else if (tokens[pos].type == T_Cout) {
            parseAssignment();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_FOR) {
            parseForStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            reportError("Unexpected token");
        }
    }

    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        TokenType varType = tokens[pos].type;
        pos++;
        
        if (tokens[pos].type != T_ID) {
            reportError("Expected variable name after type");
        }
        
        string varName = tokens[pos].value;
        
        if (symbolTable.find(varName) != symbolTable.end()) {
            reportError("Variable '" + varName + "' redeclared");
        }
        
        symbolTable[varName] = varType;
        pos++;
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
        
        cout << "Variable declared: " << varName << " of type " << varType << endl;
    }

    void parseAssignment() {
        string varName = tokens[pos].value;
        if (symbolTable.find(varName) == symbolTable.end()) {
            reportError("Variable '" + varName + "' not declared");
        }
        pos++;
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE) {
            pos++;
            parseStatement();
        }
    }

    void parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
    }

    void parseForStatement() {
        expect(T_FOR);
        expect(T_LPAREN);
        parseAssignment();
        expect(T_SEMICOLON);
        parseExpression();
        expect(T_SEMICOLON);
        parseAssignment();
        expect(T_RPAREN);
        parseStatement();
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression() {
        parsePrimary();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS || tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parsePrimary();
        }
    }

    void parsePrimary() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            pos++;
            parseExpression();
            expect(T_RPAREN);
        } else {
            reportError("Expected primary expression");
        }
    }

    void expect(TokenType expectedType) {
        if (tokens[pos].type == expectedType) {
            pos++;
        } else {
            reportError("Unexpected token");
        }
    }

    void reportError(const string &message) {
        cout << "Syntax error: " << message << " at token '" << tokens[pos].value << "' on line " << tokens[pos].line << endl;
        exit(1);
    }
};

int main() {
    string source_code = R"(
        int a = 5;  // This is a single-line comment
        int b = 10; // Another comment here
        int c = 15; // Redeclaration of variable 'a'
        if (a == 5 && b != 10) {  
            return a;
        }
    )";

    Lexer lexer(source_code);
    vector<Token> tokens = lexer.tokenizer();

    for (const auto &token : tokens) {
        cout << "Token: " << token.value << " Type: " << token.type << " Line: " << token.line << endl;
    }

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
