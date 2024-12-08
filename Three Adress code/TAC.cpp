#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

enum TokenType {
    T_INT, T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, 
    T_FOR, T_WHILE, // Added tokens for for and while
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF
};

struct Token {
    TokenType type;
    string value;
    int lineNumber;
};

class Lexer {
private:
    string src;
    size_t pos;
    int lineNumber;

public:
    Lexer(const string &src) : src(src), pos(0), lineNumber(1) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < src.size()) {
            char current = src[pos];

            if (current == '\n') {
                lineNumber++;
                pos++;
                continue;
            }
            if (isspace(current)) {
                pos++;
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(Token{T_NUM, consumeNumber(), lineNumber});
                continue;
            }
            if (isalpha(current)) {
                string word = consumeWord();
                TokenType type = T_ID;

                if (word == "int") type = T_INT;
                else if (word == "if") type = T_IF;
                else if (word == "else") type = T_ELSE;
                else if (word == "return") type = T_RETURN;
                else if (word == "for") type = T_FOR;     // Added recognition for for
                else if (word == "while") type = T_WHILE; // Added recognition for while

                tokens.push_back(Token{type, word, lineNumber});
                continue;
            }

            switch (current) {
                case '=': tokens.push_back(Token{T_ASSIGN, "=", lineNumber}); break;
                case '+': tokens.push_back(Token{T_PLUS, "+", lineNumber}); break;
                case '-': tokens.push_back(Token{T_MINUS, "-", lineNumber}); break;
                case '*': tokens.push_back(Token{T_MUL, "*", lineNumber}); break;
                case '/': tokens.push_back(Token{T_DIV, "/", lineNumber}); break;
                case '(': tokens.push_back(Token{T_LPAREN, "(", lineNumber}); break;
                case ')': tokens.push_back(Token{T_RPAREN, ")", lineNumber}); break;
                case '{': tokens.push_back(Token{T_LBRACE, "{", lineNumber}); break;
                case '}': tokens.push_back(Token{T_RBRACE, "}", lineNumber}); break;
                case ';': tokens.push_back(Token{T_SEMICOLON, ";", lineNumber}); break;
                case '>': tokens.push_back(Token{T_GT, ">", lineNumber}); break;
                default:
                    cout << "Unexpected character: " << current << " at line " << lineNumber << endl;
                    exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, "", lineNumber});
        return tokens;
    }

    string consumeNumber() {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos])) pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord() {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos])) pos++;
        return src.substr(start, pos - start);
    }
};

class SymbolTable {
public:
    void declareVariable(const string &name, const string &type) {
        if (symbolTable.find(name) != symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is already declared.");
        }
        symbolTable[name] = type;
    }

    string getVariableType(const string &name) {
        if (symbolTable.find(name) == symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
        }
        return symbolTable[name];
    }

    bool isDeclared(const string &name) const {
        return symbolTable.find(name) != symbolTable.end();
    }

private:
    map<string, string> symbolTable;
};

class IntermediateCodeGnerator {
public:
    vector<string> instructions;
    int tempCount = 0;

    string newTemp() {
        return "t" + to_string(tempCount++);
    }

    void addInstruction(const string &instr) {
        instructions.push_back(instr);
    }

    void printInstructions() {
        for (const auto &instr : instructions) {
            cout << instr << endl;
        }
    }
};

class Parser {
public:
    Parser(const vector<Token> &tokens, SymbolTable &symTable, IntermediateCodeGnerator &icg)
        : tokens(tokens), pos(0), symTable(symTable), icg(icg) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
    }

private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable &symTable;
    IntermediateCodeGnerator &icg;

    void parseStatement() {
        if (tokens[pos].type == T_INT) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileLoop();
        } else if (tokens[pos].type == T_FOR) {
            parseForLoop();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {
            parseBlock();
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].lineNumber << endl;
            exit(1);
        }
    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        string cond = parseExpression();
        expect(T_RPAREN);

        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + cond);

        string labelTrue = icg.newTemp();
        string labelEnd = icg.newTemp();

        icg.addInstruction("if " + temp + " goto " + labelTrue);
        icg.addInstruction("goto " + labelEnd);
        icg.addInstruction(labelTrue + ":");

        parseStatement();

        if (tokens[pos].type == T_ELSE) {
            string labelElse = icg.newTemp();
            icg.addInstruction("goto " + labelElse);
            icg.addInstruction(labelEnd + ":");

            expect(T_ELSE);
            parseStatement();

            icg.addInstruction(labelElse + ":");
        } else {
            icg.addInstruction(labelEnd + ":");
        }
    }

    void parseWhileLoop() {
        expect(T_WHILE);
        expect(T_LPAREN);
        string condLabel = icg.newTemp();
        string startLabel = icg.newTemp();
        string endLabel = icg.newTemp();

        icg.addInstruction(condLabel + ":");
        string cond = parseExpression();
        expect(T_RPAREN);

        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + cond);
        icg.addInstruction("if " + temp + " goto " + startLabel);
        icg.addInstruction("goto " + endLabel);

        icg.addInstruction(startLabel + ":");
        parseStatement();
        icg.addInstruction("goto " + condLabel);
        icg.addInstruction(endLabel + ":");
    }

    void parseForLoop() {
        expect(T_FOR);
        expect(T_LPAREN);

        parseStatement(); // Initialization
        string condLabel = icg.newTemp();
        string bodyLabel = icg.newTemp();
        string endLabel = icg.newTemp();

        icg.addInstruction(condLabel + ":");
        string cond = parseExpression(); // Condition
        expect(T_SEMICOLON);

        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + cond);
        icg.addInstruction("if " + temp + " goto " + bodyLabel);
        icg.addInstruction("goto " + endLabel);

        icg.addInstruction(bodyLabel + ":");
        string update = parseExpression(); // Update
        expect(T_RPAREN);

        parseStatement(); // Body
        icg.addInstruction(update); // Apply update
        icg.addInstruction("goto " + condLabel);
        icg.addInstruction(endLabel + ":");
    }

    void parseDeclaration() {
        expect(T_INT);
        string varName = expectAndReturnValue(T_ID);
        symTable.declareVariable(varName, "int");
        expect(T_SEMICOLON);
    }

    void parseAssignment() {
        string varName = expectAndReturnValue(T_ID);
        symTable.getVariableType(varName);
        expect(T_ASSIGN);
        string expr = parseExpression();
        icg.addInstruction(varName + " = " + expr);
        expect(T_SEMICOLON);
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        string expr = parseExpression();
        icg.addInstruction("return " + expr);
        expect(T_SEMICOLON);
    }

    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    string parseExpression() {
        string term = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            TokenType op = tokens[pos++].type;
            string nextTerm = parseTerm();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term + (op == T_PLUS ? " + " : " - ") + nextTerm);
            term = temp;
        }
        if (tokens[pos].type == T_GT) {
            pos++;
            string nextExpr = parseExpression();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term + " > " + nextExpr);
            term = temp;
        }
        return term;
    }

    string parseTerm() {
        string factor = parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            TokenType op = tokens[pos++].type;
            string nextFactor = parseFactor();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + factor + (op == T_MUL ? " * " : " / ") + nextFactor);
            factor = temp;
        }
        return factor;
    }

    string parseFactor() {
        if (tokens[pos].type == T_NUM) {
            return tokens[pos++].value;
        } else if (tokens[pos].type == T_ID) {
            return tokens[pos++].value;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            string expr = parseExpression();
            expect(T_RPAREN);
            return expr;
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].lineNumber << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type != type) {
            cout << "Syntax error: expected '" << type << "' at line " << tokens[pos].lineNumber << endl;
            exit(1);
        }
        pos++;
    }

    string expectAndReturnValue(TokenType type) {
        string value = tokens[pos].value;
        expect(type);
        return value;
    }
};

int main() {
    string src = R"(
    int x;
    x = 0;

    for (int i = 0; i < 10; i = i + 1) {
        x = x + i;
    }

    while (x > 0) {
        x = x - 1;
    }

    if (x == 0) {
        x = 100;
    } else {
        x = 200;
    }

    return x;
    )";
    Lexer lexer(src);
    vector<Token> tokens = lexer.tokenize();

    SymbolTable symTable;
    IntermediateCodeGnerator icg;
    Parser parser(tokens, symTable, icg);

    parser.parseProgram();
    icg.printInstructions();

    return 0;
}
