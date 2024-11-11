#include <iostream>
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
    T_ID,
    T_ASSIGN,
    T_SEMICOLON,
    T_NUM
};

// Token structure to hold the type and value
struct Token
{
    TokenType type;
    string value;

    Token(TokenType t, string v) : type(t), value(v) {}
};

// Lexer class for tokenizing input
class Lexer
{
private:
    string src;
    size_t pos;

public:
    Lexer(const string &source) : src(source), pos(0) {}

    Token consumeNumber()
    {
        string num = "";
        while (pos < src.length() && isdigit(src[pos]))
        {
            num += src[pos++];
        }
        return Token(T_NUM, num);
    }

    Token consumeWord()
    {
        string word = "";
        while (pos < src.length() && (isalnum(src[pos]) || src[pos] == '_'))
        {
            word += src[pos++];
        }
        if (word == "int")
        {
            return Token(T_INT, word); // Match the 'int' keyword
        }
        return Token(T_ID, word);
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        while (pos < src.length())
        {
            char current = src[pos];
            if (isspace(current))
            {
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
            else if (current == '+')
            {
                tokens.push_back(Token(T_PLUS, "+"));
                pos++;
            }
            else if (current == '-')
            {
                tokens.push_back(Token(T_MINUS, "-"));
                pos++;
            }
            else if (current == '(')
            {
                tokens.push_back(Token(T_LPAREN, "("));
                pos++;
            }
            else if (current == ')')
            {
                tokens.push_back(Token(T_RPAREN, ")"));
                pos++;
            }
            else if (current == '=')
            {
                tokens.push_back(Token(T_ASSIGN, "="));
                pos++;
            }
            else if (current == ';')
            {
                tokens.push_back(Token(T_SEMICOLON, ";"));
                pos++;
            }
            else
            {
                throw runtime_error("Unknown character: " + string(1, current));
            }
        }
        tokens.push_back(Token(T_EOF, "EOF"));
        return tokens;
    }
};

// Parser class for parsing tokens
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
            cerr << "Expected token type: " << type << ", but found: " << current_token().type << endl;
            throw runtime_error("Syntax Error: Unexpected token type.");
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
        if (current_token().type == T_INT)
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
            throw runtime_error("Unknown statement.");
        }
    }

    void parseDeclaration()
    {
        expect(T_INT);
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
        if (current_token().type == T_NUM || current_token().type == T_ID)
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
            throw runtime_error("Unexpected token in factor.");
        }
    }
};

int main()
{
    string sample_code = "int a; a = 5 + 3;";

    // Step 1: Tokenize the input code
    Lexer lexer(sample_code);
    vector<Token> tokens = lexer.tokenize();

    // Print token stream for debugging
    cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        cout << "Type: " << token.type << ", Value: " << token.value << endl;
    }

    // Step 2: Parse the tokenized input
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
