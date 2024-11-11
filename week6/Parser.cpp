
// importing Libraries  
#include<iostream>
#include<vector>
#include<string>
#include<cctype>
#include<map>

using namespace std;

enum TokenType{

T_INT, T_ID, T_NUM, T_IF,T_ELSE , T_RETURN,
T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, 
T_SEMICOLON, T_GT, T_EOF 

};

struct Token
{
    TokenType type;
    string value;
};

class lexer{
    private:
    string src;
    size_t pos; 

    public:
    lexer(const string &src){
        this->src=src;
        this->pos=0;
    }

    string consumeNumber(){
        size_t start =pos;
        while (pos<src.size() && isdigit(src[pos]))pos++;
        return src.substr(start,pos-start );
    }

    string consumeWord(){
        size_t start=pos;
        while(pos<src.size() && isalnum(src[pos])) pos++;
        return src.substr(start,pos-start);

    }

    vector<Token>tokenize(){
        vector<Token>tokens;
        while (pos < src.size()){
            char current = src[pos];
            if (isspace(current))
            {
                pos++;
                continue;
            }
            else if (isdigit(current))
            {
                tokens.push_back(Token{T_NUM, consumeNumber()});
                continue;
            }
            else if (isalpha(current))
            {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{TokenType::T_INT, word});
                else if (word == "if")
                    tokens.push_back(Token{TokenType::T_IF, word});
                else if (word == "else")
                    tokens.push_back(Token{TokenType::T_ELSE, word});
                else if (word == "return")
                    tokens.push_back(Token{TokenType::T_RETURN, word});
                else
                    tokens.push_back(Token{TokenType::T_ID, word});
                continue;
            }

        }
    }


};
