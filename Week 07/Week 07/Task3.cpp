#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

// Token types enumeration
enum TokenType
{
    T_IF,
    T_ELSE,
    T_PLUS,
    T_MINUS,
    T_LPAREN,
    T_RPAREN,
    T_EOF,
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_STRING,
    T_BOOL,
    T_CHAR,
    T_ID,
    T_ASSIGN,
    T_SEMICOLON,
    T_NUM,
    T_STR_LITERAL
};

// Token structure to hold the type, value, and line number
struct Token
{
    TokenType type;
    string value;
    int line;

    Token(TokenType t, string v, int l) : type(t), value(v), line(l) {}
};

// Lexer class for tokenizing input and tracking line numbers
class Lexer
{
private:
    string src;
    size_t pos;
    int line;

public:
    Lexer(const string &source) : src(source), pos(0), line(1) {}

    Token consumeNumber()
    {
        string num = "";
        bool isFloat = false;
        while (pos < src.length() && (isdigit(src[pos]) || src[pos] == '.'))
        {
            if (src[pos] == '.')
            {
                if (isFloat)
                {
                    throw runtime_error("Multiple decimal points found in number at line " + to_string(line));
                }
                isFloat = true;
            }
            num += src[pos++];
        }
        return isFloat ? Token(T_FLOAT, num, line) : Token(T_NUM, num, line);
    }

    Token consumeWord()
    {
        string word = "";
        while (pos < src.length() && (isalnum(src[pos]) || src[pos] == '_'))
        {
            word += src[pos++];
        }
        // Recognize the new data types
        if (word == "int")
            return Token(T_INT, word, line);
        if (word == "float")
            return Token(T_FLOAT, word, line);
        if (word == "double")
            return Token(T_DOUBLE, word, line);
        if (word == "string")
            return Token(T_STRING, word, line);
        if (word == "bool")
            return Token(T_BOOL, word, line);
        if (word == "char")
            return Token(T_CHAR, word, line);
        return Token(T_ID, word, line);
    }

    Token consumeStringLiteral()
    {
        string literal = "";
        pos++; // Skip the opening quote
        while (pos < src.length() && src[pos] != '"')
        {
            literal += src[pos++];
        }
        if (pos >= src.length() || src[pos] != '"')
        {
            throw runtime_error("Unterminated string literal at line " + to_string(line));
        }
        pos++; // Skip the closing quote
        return Token(T_STR_LITERAL, literal, line);
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        while (pos < src.length())
        {
            char current = src[pos];
            if (isspace(current))
            {
                if (current == '\n')
                {
                    line++; // Increment line count on newline
                }
                pos++;
            }
            else if (isdigit(current))
            {
                tokens.push_back(consumeNumber());
            }
            else if (isalpha(current) || current == '_')
            {
                tokens.push_back(consumeWord());
            }
            else if (current == '"')
            {
                tokens.push_back(consumeStringLiteral());
            }
            else if (current == '+')
            {
                tokens.push_back(Token(T_PLUS, "+", line));
                pos++;
            }
            else if (current == '-')
            {
                tokens.push_back(Token(T_MINUS, "-", line));
                pos++;
            }
            else if (current == '(')
            {
                tokens.push_back(Token(T_LPAREN, "(", line));
                pos++;
            }
            else if (current == ')')
            {
                tokens.push_back(Token(T_RPAREN, ")", line));
                pos++;
            }
            else if (current == '=')
            {
                tokens.push_back(Token(T_ASSIGN, "=", line));
                pos++;
            }
            else if (current == ';')
            {
                tokens.push_back(Token(T_SEMICOLON, ";", line));
                pos++;
            }
            else
            {
                throw runtime_error("Unknown character at line " + to_string(line) + ": " + string(1, current));
            }
        }
        tokens.push_back(Token(T_EOF, "EOF", line));
        return tokens;
    }
};

// Parser class for parsing tokens and handling the new data types
class Parser
{
private:
    vector<Token> tokens;
    size_t pos;

    Token current_token()
    {
        return tokens[pos];
    }

    void advance()
    {
        pos++;
    }

    void expect(TokenType type)
    {
        if (current_token().type == type)
        {
            advance();
        }
        else
        {
            cerr << "Error at line " << current_token().line << ": Expected token type " << type<< ", but found " << current_token().value << endl;
            throw runtime_error("Syntax Error at line " + to_string(current_token().line) + ": Unexpected token.");
        }
    }

public:
    Parser(const vector<Token> &tok) : tokens(tok), pos(0) {}

    void parseProgram()
    {
        while (current_token().type != T_EOF)
        {
            parseStatement();
        }
    }

    void parseStatement()
    {
        if (current_token().type == T_INT || current_token().type == T_FLOAT || current_token().type == T_DOUBLE ||
            current_token().type == T_STRING || current_token().type == T_BOOL || current_token().type == T_CHAR)
        {
            parseDeclaration();
        }
        else if (current_token().type == T_ID)
        {
            parseAssignment();
        }
        else if (current_token().type == T_IF)
        {
            parseIfStatement();
        }
        else
        {
            throw runtime_error("Unknown statement at line " + to_string(current_token().line));
        }
    }

    void parseDeclaration()
    {
        advance(); // Skip the data type (int, float, etc.)
        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment()
    {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
    }

    void parseExpression()
    {
        parseTerm();
        while (current_token().type == T_PLUS || current_token().type == T_MINUS)
        {
            advance();
            parseTerm();
        }
    }

    void parseTerm()
    {
        parseFactor();
    }

    void parseFactor()
    {
        if (current_token().type == T_NUM || current_token().type == T_FLOAT || current_token().type == T_ID || current_token().type == T_STR_LITERAL)
        {
            advance();
        }
        else if (current_token().type == T_LPAREN)
        {
            advance();
            parseExpression();
            expect(T_RPAREN);
        }
        else
        {
            throw runtime_error("Unexpected token in factor at line " + to_string(current_token().line));
        }
    }
};

int main()
{
    // if (argc != 2)
    // {
    //     cerr << " provide file name" << endl;
    //     return 1;
    // }

    // // Open and read the source file
    // ifstream file(argv[1]);
    // if (!file)
    // {
    //     cerr << "Error: Could not open file " << argv[1] << endl;
    //     return 1;
    // }

    // stringstream buffer;
    // buffer << file.rdbuf();
    // string source_code = buffer.str();

    // Tokenizing input code
    
    string source_code = "int a; a = 5 + 3; float x; x=0.2234; char b= d; double f= 2132;";
    Lexer lexer(source_code);
    vector<Token> tokens = lexer.tokenize();

    cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        cout << "Line " << token.line << " - Type: " << token.type << ", Value: " << token.value << endl;
    }

    Parser parser(tokens);
    try
    {
        parser.parseProgram();
        cout << "Parsing completed successfully!" << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Parsing error: " << e.what() << endl;
    }

    return 0;
}