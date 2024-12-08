#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <map>
#include <algorithm>

using namespace std;

enum TokenType
{
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

string TokenTypeString(TokenType type)
{
    switch (type)
    {
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

struct Token
{
    TokenType type;
    string value;
    int line;
};

class Lexer
{
private:
    string src;
    size_t pos;
    int line;

public:
    Lexer(const string &src) : src(src), pos(0), line(0) {}

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        while (pos < src.size())
        {
            char current = src[pos];

            if (current == '/' && pos + 1 < src.size() && src[pos + 1] == '/')
            {
                pos += 2;
                while (pos < src.size() && src[pos] != '\n')
                    pos++;
                continue;
            }

            if (current == '\n')
            {
                line++;
                pos++;
                continue;
            }

            if (isspace(current))
            {
                pos++;
                continue;
            }

            if (isdigit(current))
            {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }

            if (isalpha(current))
            {
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

            switch (current)
            {
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

    string consumeNumber()
    {
        size_t start = pos;
        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.'))
            pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord()
    {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }
};

struct SymbolEntry
{
    string name;
    TokenType datatype;
    string value;
    bool isFunction = false;
};

class SymbolTable
{
private:
    vector<Token> tokens;
    map<string, SymbolEntry> table;

public:
    SymbolTable(const vector<Token> &tokens) : tokens(tokens) {}

    void addEntry(const string &name, TokenType datatype, const string &value = "")
    {
        table[name] = SymbolEntry{name, datatype, value};
    }

    bool exists(const string &name) const
    {
        return table.find(name) != table.end();
    }

    void makeTable()
    {
        for (size_t i = 0; i < tokens.size(); i++)
        {
            const auto &token = tokens[i];
            if (token.type == T_INT || token.type == T_FLOAT)
            {
                if (i + 1 < tokens.size() && tokens[i + 1].type == T_ID)
                {
                    const auto &varName = tokens[i + 1].value;
                    if (!exists(varName))
                        addEntry(varName, token.type);
                    else
                    {
                        cout << "Redefinition of variable: " << varName << endl;
                        exit(1);
                    }
                }
            }
            else if (token.type == T_ID)
            {
                if (!exists(token.value))
                {
                    cout << "Undeclared variable: " << token.value << endl;
                    exit(1);
                }
            }
        }
    }
};

class IntermediateCodeGenerator
{
private:
    vector<string> instructions;
    int tempCounter = 0;
    int labelCounter = 0;

public:
    string newTemp() { return "T" + to_string(tempCounter++); }
    string newLabel() { return "L" + to_string(labelCounter++); }

    void addInstruction(const string &instr)
    {
        instructions.push_back(instr);
    }

    void printInstructions() const
    {
        for (const auto &instr : instructions)
            cout << instr << endl;
    }
};

class Parser
{
private:
    vector<Token> tokens;
    size_t pos = 0;
    IntermediateCodeGenerator &icg;
    SymbolTable &symbolTable;

public:
    Parser(const vector<Token> &tokens, IntermediateCodeGenerator &icg, SymbolTable &symbolTable)
        : tokens(tokens), icg(icg), symbolTable(symbolTable) {}

    void parseProgram()
    {
        while (tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
    }

private:
    void parseStatement()
    {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT)
        {
            parseDeclaration();
        }
        else if (tokens[pos].type == T_ID)
        {
            parseAssignment();
        }
        else if (tokens[pos].type == T_IF)
        {
            parseIfStatement();
        }
        else if (tokens[pos].type == T_LBRACE)
        {
            parseBlock();
        }
        else if (tokens[pos].type == T_FOR)
        {
            parseForStatement();
        }
        else
        {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line: " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBlock()
    {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseForStatement()
    {
        expect(T_FOR);
        expect(T_LPAREN);

        parseAssignment(); // Initial assignment
        string conditionLabel = icg.newLabel();
        string bodyLabel = icg.newLabel();
        string exitLabel = icg.newLabel();

        icg.addInstruction(conditionLabel + ":");
        string condition = parseExpression();
        icg.addInstruction("if " + condition + " goto " + bodyLabel);
        icg.addInstruction("goto " + exitLabel);
        icg.addInstruction(bodyLabel + ":");

        expect(T_SEMICOLON);
        string updateVar = expectAndReturnValue(T_ID);
        expect(T_ASSIGN);
        string updateExpr = parseExpression();

        expect(T_RPAREN);

        parseBlock();
        icg.addInstruction(updateVar + " = " + updateExpr);
        icg.addInstruction("goto " + conditionLabel);

        icg.addInstruction(exitLabel + ":");
    }

    void parseDeclaration()
    {
        expect(tokens[pos].type);
        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment()
    {
        string varName = expectAndReturnValue(T_ID);
        expect(T_ASSIGN);
        string expr = parseExpression();
        icg.addInstruction(varName + " = " + expr);
        expect(T_SEMICOLON);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        string cond = parseExpression();
        expect(T_RPAREN);

        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + cond);
        string l1 = icg.newLabel();
        string l2 = icg.newLabel();
        icg.addInstruction("if " + temp + " goto " + l1);
        icg.addInstruction("goto " + l2);
        icg.addInstruction(l1 + ":");

        parseStatement();

        if (tokens[pos].type == T_ELSE)
        {
            string l3 = icg.newLabel();
            icg.addInstruction("goto " + l3);
            icg.addInstruction(l2 + ":");
            expect(T_ELSE);
            parseStatement();
            icg.addInstruction(l3 + ":");
        }
        else
        {
            icg.addInstruction(l2 + ":");
        }
    }

    string parseExpression()
    {
        string term = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS)
        {
            TokenType op = tokens[pos++].type;
            string nextTerm = parseTerm();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term + (op == T_PLUS ? " + " : " - ") + nextTerm);
            term = temp;
        }
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT)
        {
            string opSign = tokens[pos].type == T_GT ? " > " : " < ";
            pos++;
            string nextExpr = parseExpression();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + term + opSign + nextExpr);
            term = temp;
        }
        return term;
    }

    string parseTerm()
    {
        string factor = parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
        {
            TokenType op = tokens[pos++].type;
            string nextFactor = parseFactor();
            string temp = icg.newTemp();
            icg.addInstruction(temp + " = " + factor + (op == T_MUL ? " * " : " / ") + nextFactor);
            factor = temp;
        }
        return factor;
    }

    string parseFactor()
    {
        if (tokens[pos].type == T_NUM)
        {
            return tokens[pos++].value;
        }
        else if (tokens[pos].type == T_ID)
        {
            return tokens[pos++].value;
        }
        else if (tokens[pos].type == T_LPAREN)
        {
            expect(T_LPAREN);
            string expr = parseExpression();
            expect(T_RPAREN);
            return expr;
        }
        else
        {
            cout << "Syntax error in factor!" << endl;
            exit(1);
        }
    }

    void expect(TokenType expectedType)
    {
        if (tokens[pos].type != expectedType)
        {
            cout << "Expected token " << TokenTypeString(expectedType) << ", got " << TokenTypeString(tokens[pos].type) << endl;
            exit(1);
        }
        pos++;
    }

    string expectAndReturnValue(TokenType expectedType)
    {
        expect(expectedType);
        return tokens[pos - 1].value;
    }
};

int main()
{
    string sourceCode = R"(
    int x = 10;
    float y = 3.14;
    if (x > 5) {
        x = x + 5;
    } else {
        y = y * 2;
    }
    for (int i = 0; i < 10; i++) {
        x = x + 1;
}
)";

    Lexer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    SymbolTable symbolTable(tokens);
    symbolTable.makeTable();

    IntermediateCodeGenerator icg;
    Parser parser(tokens, icg, symbolTable);
    parser.parseProgram();

    icg.printInstructions();
}
