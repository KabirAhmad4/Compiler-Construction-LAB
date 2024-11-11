#include <iostream>
#include <cctype>
#include <string>
#include <vector>

enum TokenType { KEYWORD, IDENTIFIER, NUMBER, OPERATOR, PUNCTUATION, UNKNOWN };

struct Token {
    TokenType type;
    std::string value;
};

bool isKeyword(const std::string &word) {
    // Simple example: Add your language's keywords here
    std::vector<std::string> keywords = {"if", "else", "for", "while"};
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

std::vector<Token> tokenize(const std::string &input) {
    std::vector<Token> tokens;
    std::string token;
    for (size_t i = 0; i < input.size(); i++) {
        if (isspace(input[i])) {
            continue;
        } else if (isalpha(input[i])) {
            token.clear();
            while (isalpha(input[i]) || isdigit(input[i])) {
                token += input[i++];
            }
            i--; // Move back to the correct position
            if (isKeyword(token)) {
                tokens.push_back({KEYWORD, token});
            } else {
                tokens.push_back({IDENTIFIER, token});
            }
        } else if (isdigit(input[i])) {
            token.clear();
            while (isdigit(input[i])) {
                token += input[i++];
            }
            i--;
            tokens.push_back({NUMBER, token});
        } else if (ispunct(input[i])) {
            tokens.push_back({PUNCTUATION, std::string(1, input[i])});
        } else {
            tokens.push_back({UNKNOWN, std::string(1, input[i])});
        }
    }
    return tokens;
}

void printTokens(const std::vector<Token> &tokens) {
    for (const auto &token : tokens) {
        std::cout << "Token: " << token.value << ", Type: " << token.type << std::endl;
    }
}

int main() {
    std::string code = "if (x == 10) { y = 20; }";
    std::vector<Token> tokens = tokenize(code);
    printTokens(tokens);
    return 0;
}
