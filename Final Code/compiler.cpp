#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip> // for formatting

using namespace std;

enum TokenType {
    T_INT,
    T_FLOAT,
    T_ID,
    T_NUM,
    T_IF,
    T_ELSE,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_GT,
    T_LT,
    T_FOR,
    T_EOF,
};

string TokenTypeString(TokenType type) {
    switch (type) {
    case T_NUM: return "NUM";
    case T_FLOAT: return "FLOAT";
    case T_INT: return "INT";
    case T_ID: return "ID";
    case T_IF: return "IF";
    case T_ELSE: return "ELSE";
    case T_ASSIGN: return "ASSIGN";
    case T_PLUS: return "PLUS";
    case T_MINUS: return "MINUS";
    case T_MUL: return "MUL";
    case T_DIV: return "DIV";
    case T_LPAREN: return "LPAREN";
    case T_RPAREN: return "RPAREN";
    case T_LBRACE: return "LBRACE";
    case T_RBRACE: return "RBRACE";
    case T_SEMICOLON: return "SEMICOLON";
    case T_GT: return "GT";
    case T_LT: return "LT";
    case T_EOF: return "EOF";
    case T_FOR: return "FOR";
    default: return "UNKNOWN";
    }
}

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
    Lexer(const string& src) : src(src), pos(0), line(1) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];

            if (current == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
                pos += 2;
                while (pos < src.size() && src[pos] != '\n') pos++;
                continue;
            }

            if (current == '\n') {
                line++;
                pos++;
                continue;
            }

            if (isspace(current)) {
                pos++;
                continue;
            }

            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }

            if (isalpha(current)) {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{T_INT, word, line});
                else if (word == "float")
                    tokens.push_back(Token{T_FLOAT, word, line});
                else if (word == "if")
                    tokens.push_back(Token{T_IF, word, line});
                else if (word == "else")
                    tokens.push_back(Token{T_ELSE, word, line});
                else if (word == "for")
                    tokens.push_back(Token{T_FOR, word, line});
                else
                    tokens.push_back(Token{T_ID, word, line});
                continue;
            }

            switch (current) {
            case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
            case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
            case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
            case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
            case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
            case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
            case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
            case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;
            case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;
            case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
            case '>': tokens.push_back(Token{T_GT, ">", line}); break;
            case '<': tokens.push_back(Token{T_LT, "<", line}); break;
            default:
                cout << "Unexpected character: " << current << endl;
                exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    }

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
};

struct SymbolEntry {
    string name;
    TokenType datatype;
    string value;
};

class SymbolTable {
private:
    map<string, SymbolEntry> table;

public:
    void addEntry(const string& name, TokenType datatype, const string& value = "") {
        table[name] = SymbolEntry{name, datatype, value};
    }

    void setValue(const string& name, const string& value) {
        if (exists(name))
            table[name].value = value;
        else {
            cout << "Variable " << name << " not declared!" << endl;
            exit(1);
        }
    }

    string getValue(const string& name) const {
        if (exists(name))
            return table.at(name).value;
        else {
            cout << "Variable " << name << " not declared!" << endl;
            exit(1);
        }
    }

    bool exists(const string& name) const {
        return table.find(name) != table.end();
    }

    void printTable() const {
        cout << "\nSymbol Table:" << endl;
        cout << setw(10) << "Name" << setw(10) << "Type" << setw(15) << "Value" << endl;
        for (const auto& entry : table) {
            cout << setw(10) << entry.second.name
                 << setw(10) << TokenTypeString(entry.second.datatype)
                 << setw(15) << entry.second.value << endl;
        }
    }
};

class IntermediateCodeGenerator {
private:
    vector<string> instructions;
    int tempCounter = 0;
    int labelCounter = 0;

public:
    string newTemp() { return "T" + to_string(tempCounter++); }
    string newLabel() { return "L" + to_string(labelCounter++); }

    void addInstruction(const string& instr) {
        instructions.push_back(instr);
    }

    void printInstructions() const {
        cout << "\nThree-Address Code:" << endl;
        for (const auto& instr : instructions)
            cout << instr << endl;
    }

    void generateAssembly() const {
        cout << "\nAssembly Code:" << endl;
        for (const auto& instr : instructions) {
            stringstream ss(instr);
            string op, arg1, arg2, result;
            ss >> result >> op >> arg1;

            if (op == "=")
                cout << "MOV " << result << ", " << arg1 << endl;
            else if (op == "+")
                cout << "ADD " << result << ", " << arg1 << endl;
            else if (op == "-")
                cout << "SUB " << result << ", " << arg1 << endl;
            else if (op == "*")
                cout << "MUL " << result << ", " << arg1 << endl;
            else if (op == "/")
                cout << "DIV " << result << ", " << arg1 << endl;
            else if (op == "goto")
                cout << "JMP " << result << endl;
            else if (op == "if")
                cout << "CMP " << arg1 << ", 0\nJNZ " << result << endl;
        }
    }
};

class Parser {
private:
    vector<Token> tokens;
    size_t pos = 0;
    IntermediateCodeGenerator& icg;
    SymbolTable& symbolTable;

public:
    Parser(const vector<Token>& tokens, IntermediateCodeGenerator& icg, SymbolTable& symbolTable)
        : tokens(tokens), icg(icg), symbolTable(symbolTable) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
    }

private:
    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void parseDeclaration() {
        TokenType type = tokens[pos].type;
        string name = tokens[++pos].value; // Next token is the variable name
        symbolTable.addEntry(name, type, "");
        pos++; // Skip the name
        pos++; // Skip the semicolon
    }

    void parseAssignment() {
        string varName = tokens[pos++].value;
        pos++; // Skip the '='
        string expr = parseExpression();
        icg.addInstruction(varName + " = " + expr);
        symbolTable.setValue(varName, expr); // Update value in the symbol table
        pos++; // Skip the semicolon
    }

    void parseIfStatement() {
        pos++; // Skip 'if'
        pos++; // Skip '('
        string condition = parseExpression();
        pos++; // Skip ')'
        string labelTrue = icg.newLabel();
        string labelFalse = icg.newLabel();
        icg.addInstruction("if " + condition + " goto " + labelTrue);
        icg.addInstruction("goto " + labelFalse);
        icg.addInstruction(labelTrue + ":");
        parseStatement();
        icg.addInstruction(labelFalse + ":");
    }

    string parseExpression() {
        string term1 = parseTerm();

        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS ||
               tokens[pos].type == T_GT || tokens[pos].type == T_LT) {
            string op = tokens[pos++].value; // Get the operator
            string term2 = parseTerm();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term1 + " " + op + " " + term2);
            term1 = temp;
        }

        return term1;
    }

    string parseTerm() {
        if (tokens[pos].type == T_NUM) {
            return tokens[pos++].value; // Return numbers directly
        } else if (tokens[pos].type == T_ID) {
            string varName = tokens[pos++].value;
            return symbolTable.getValue(varName); // Fetch value from symbol table
        } else if (tokens[pos].type == T_LPAREN) {
            pos++; // Skip '('
            string expr = parseExpression();
            pos++; // Skip ')'
            return expr;
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }
};

int main() {
    string sourceCode = R"(
    int x;
    float y;
    int z;
    float w;
    w = 1.3;
    z = 19;
    x = 11;
    y = 3.14;
    if (x > 3) x = x + 1;
    
    )";

    Lexer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    // Display tokens
    cout << "Tokens:" << endl;
    for (const auto& token : tokens) {
        cout << "Type: " << TokenTypeString(token.type) << ", Value: " << token.value << ", Line: " << token.line << endl;
    }

    SymbolTable symbolTable;
    IntermediateCodeGenerator icg;

    Parser parser(tokens, icg, symbolTable);
    parser.parseProgram();

    // Display symbol table
    symbolTable.printTable();

    // Display three-address code
    icg.printInstructions();

    // Generate and display assembly code
    icg.generateAssembly();
}
