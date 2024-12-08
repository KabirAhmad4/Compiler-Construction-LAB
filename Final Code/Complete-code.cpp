#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <stack> 
#include <stdexcept>
// #define AND &&

using namespace std;

// Token Types Enumeration
enum TokenType {
    PLUS,
    MINUS, 
    MULTIPLY, 
    DIVIDE, 
    MODULO, 
    ASSIGN, 
    EQUAL, 
    NOT_EQUAL, 
    LESS_THAN, 
    GREATER_THAN, 
    LESS_EQUAL, 
    GREATER_EQUAL, 
    IF, 
    ELSE, 
    WHILE, 
    FOR, 
    RETURN, 
    INT, 
    FLOAT, 
    DOUBLE, 
    CHAR, 
    STRING, 
    VOID,
    IDENTIFIER, 
    INTEGER_LITERAL, 
    FLOAT_LITERAL, 
    STRING_LITERAL,
    SEMICOLON, 
    COMMA, 
    LEFT_PAREN, 
    RIGHT_PAREN, 
    LEFT_BRACE, 
    RIGHT_BRACE,
    LOGICAL_AND, 
    LOGICAL_OR, 
    LOGICAL_NOT, 
    END_OF_FILE, 
    UNKNOWN,
    BREAK, 
    CONTINUE, 
    SWITCH, 
    CASE, 
    DEFAULT, 
    PUBLIC, 
    PRIVATE, 
    PROTECTED,
    TRY, 
    CATCH, 
    THROW, 
    REFERENCE, 
    DEREFERENCE, 
    MEMBER_ACCESS , 
    OR,
    AND, 
    STRUCT
};

// Token Structure
struct Token {
    TokenType type;
    string value;
    int line, column;

    string toString() const {
        static const unordered_map<TokenType, string> typeNames = {
            {OR, "OR"},
            {AND, "AND"},
            {PLUS, "PLUS"},
            {MINUS, "MINUS"}, 
            {MULTIPLY, "MULTIPLY"},
            {DIVIDE, "DIVIDE"}, 
            {MODULO, "MODULO"}, 
            {ASSIGN, "ASSIGN"},
            {EQUAL, "EQUAL"}, 
            {NOT_EQUAL, "NOT_EQUAL"}, 
            {LESS_THAN, "LESS_THAN"},
            {GREATER_THAN, "GREATER_THAN"}, 
            {LESS_EQUAL, "LESS_EQUAL"},
            {GREATER_EQUAL, "GREATER_EQUAL"}, 
            {IF, "IF"}, 
            {ELSE, "ELSE"},
            {WHILE, "WHILE"}, 
            {FOR, "FOR"}, 
            {RETURN, "RETURN"},
            {INT, "INT"}, 
            {FLOAT, "FLOAT"}, 
            {DOUBLE, "DOUBLE"},
            {CHAR, "CHAR"}, 
            {STRING, "STRING"}, 
            {VOID, "VOID"},
            {IDENTIFIER, "IDENTIFIER"}, 
            {INTEGER_LITERAL, "INTEGER_LITERAL"},
            {FLOAT_LITERAL, "FLOAT_LITERAL"}, 
            {STRING_LITERAL, "STRING_LITERAL"},
            {SEMICOLON, "SEMICOLON"}, 
            {COMMA, "COMMA"},
            {LEFT_PAREN, "LEFT_PAREN"}, 
            {RIGHT_PAREN, "RIGHT_PAREN"},
            {LEFT_BRACE, "LEFT_BRACE"}, 
            {RIGHT_BRACE, "RIGHT_BRACE"},
            {LOGICAL_AND, "LOGICAL_AND"}, 
            {LOGICAL_OR, "LOGICAL_OR"},
            {LOGICAL_NOT, "LOGICAL_NOT"}, 
            {END_OF_FILE, "END_OF_FILE"},
            {STRUCT, "STRUCT"},
            {UNKNOWN, "UNKNOWN"},
            {BREAK, "BREAK"},
            {CONTINUE, "CONTINUE"}, 
            {SWITCH, "SWITCH"}, 
            {CASE, "CASE"}, 
            {DEFAULT, "DEFAULT"}, 
            {PUBLIC, "PUBLIC"}, 
            {PRIVATE, "PRIVATE"}, 
            {PROTECTED, "PROTECTED"}, 
            {TRY, "TRY"}, 
            {CATCH, "CATCH"}, 
            {THROW, "THROW"}, 
            {REFERENCE, "&"}, 
            {DEREFERENCE, "*"}, 
            {MEMBER_ACCESS, "->"}
        };

        auto it = typeNames.find(type);
        string typeName = it != typeNames.end() ? it->second : "UNKNOWN";
        return "Token{type: " + typeName + ", value: '" + value + "', line: " + to_string(line) + ", column: " + to_string(column) + "}";
    }
};

// Symbol Table Class
class SymbolTable {
private:
    struct Symbol {
        string identifier;
        string type;
        string value;
        string scope;
        int line;

        string toString() const {
            return identifier + " | " + type + " | " + value + " | " + scope + " | " + to_string(line);
        }
    };

    vector<Symbol> symbols;

public:
    void addEntry(const string &identifier, const string &type, const string &value, const string &scope, int line) {
        symbols.push_back({identifier, type, value, scope, line});
    }

    void print() const {
        cout << "\nSymbol Table:\n";
        cout << setw(15) << "Identifier" << " | " 
             << setw(10) << "Type" << " | " 
             << setw(10) << "Scope" << " | " 
             << setw(5) << "Line" << "\n";
        cout << string(60, '-') << "\n";
        for (const auto &symbol : symbols) {
            cout << setw(15) << symbol.identifier << " | "
                 << setw(10) << symbol.type << " | "
                 << setw(10) << symbol.scope << " | "
                 << setw(5) << symbol.line << "\n";
        }
    }
};

// Lexer Class
class Lexer {
private:
    string source;
    size_t current;
    int line, column;
    vector<Token> tokens;

public:
    SymbolTable symbolTable;
    string currentScope = "global";

    Lexer(const string &src) : source(src), current(0), line(1), column(1) {}

    vector<Token> tokenize() {
        tokens.clear();
        while (!isAtEnd()) {
            char c = advance();

            if (isspace(c)) {
                if (c == '\n') {
                    line++;
                    column = 1;
                }
                continue;
            }

            switch (c) {
                case '+': tokens.push_back(createToken(PLUS, "+")); break;
                case '-': tokens.push_back(createToken(MINUS, "-")); break;
                case '*': tokens.push_back(createToken(MULTIPLY, "*")); break;
                case '/': tokens.push_back(createToken(DIVIDE, "/")); break;
                case '%': tokens.push_back(createToken(MODULO, "%")); break;
                case '=': tokens.push_back(match('=') ? createToken(EQUAL, "==") : createToken(ASSIGN, "=")); break;
                case '<': tokens.push_back(match('=') ? createToken(LESS_EQUAL, "<=") : createToken(LESS_THAN, "<")); break;
                case '>': tokens.push_back(match('=') ? createToken(GREATER_EQUAL, ">=") : createToken(GREATER_THAN, ">")); break;
                case '!': tokens.push_back(match('=') ? createToken(NOT_EQUAL, "!=") : createToken(LOGICAL_NOT, "!")); break;
                case '&': tokens.push_back(match('&') ? createToken(AND, "&&") : createToken(REFERENCE, "&")); break;
                case '|': tokens.push_back(match('|') ? createToken(OR, "||") : createToken(UNKNOWN, "|")); break;
                case '(': tokens.push_back(createToken(LEFT_PAREN, "(")); break;
                case ')': tokens.push_back(createToken(RIGHT_PAREN, ")")); break;
                case '{': tokens.push_back(createToken(LEFT_BRACE, "{")); break;
                case '}': tokens.push_back(createToken(RIGHT_BRACE, "}")); break;
                case ';': tokens.push_back(createToken(SEMICOLON, ";")); break;
                case ',': tokens.push_back(createToken(COMMA, ",")); break;
                case '"': tokens.push_back(tokenizeStringLiteral()); break;
                // case '*': tokens.push_back(createToken(DEREFERENCE, "*")); break;
                
                // case '': 
                //     if (match('>')) tokens.push_back(createToken(MEMBER_ACCESS, "->"));
                //     break;
                default:
                    if (isdigit(c)) {
                        tokens.push_back(tokenizeNumber(c));
                    } else if (isalpha(c) || c == '_') {
                        tokens.push_back(tokenizeIdentifierOrKeyword(c));
                    } else {
                        tokens.push_back(createToken(UNKNOWN, string(1, c)));
                    }
            }
        }

        tokens.push_back({END_OF_FILE, "", line, column});
        return tokens;
    }

private:
    bool isAtEnd() const {
        return current >= source.length();
    }

    char advance() {
        column++;
        return source[current++];
    }

    bool match(char expected) {
        if (isAtEnd() || source[current] != expected) return false;
        current++;
        column++;
        return true;
    }

    Token createToken(TokenType type, const string &value = "") {
        return {type, value, line, column};
    }

    Token tokenizeNumber(char first) {
        string number(1, first);
        while (!isAtEnd() && isdigit(source[current])) {
            number += advance();
        }

        if (!isAtEnd() && source[current] == '.') {
            number += advance();
            while (!isAtEnd() && isdigit(source[current])) {
                number += advance();
            }
            return {FLOAT_LITERAL, number, line, column};
        }

        return {INTEGER_LITERAL, number, line, column};
    }

    Token tokenizeIdentifierOrKeyword(char first) {
        string identifier(1, first);
        while (!isAtEnd() && (isalnum(source[current]) || source[current] == '_')) {
            identifier += advance();
        }

        static const unordered_map<string, TokenType> keywords = {
            {"if", IF}, {"else", ELSE},   {"while", WHILE}, {"for", FOR}, {"return", RETURN},
            {"int", INT}, {"float", FLOAT}, {"double", DOUBLE},
            {"char", CHAR}, {"string", STRING}, {"void", VOID},
            {"break", BREAK}, {"continue", CONTINUE}, {"switch", SWITCH},
            {"case", CASE}, {"default", DEFAULT}, {"public", PUBLIC},
            {"private", PRIVATE}, {"protected", PROTECTED}, {"try", TRY},
            {"catch", CATCH}, {"throw", THROW} ,{"struct", STRUCT } 
        };

        auto it = keywords.find(identifier);
        if (it != keywords.end()) {
            return {it->second, identifier, line, column};
        } else {
            string value = "";
            if (!tokens.empty() && tokens.back().type == ASSIGN) {
                value = tokens.back().value; // Capture last assigned value
            }
            string type = !tokens.empty() && (
                tokens.back().type == INT || tokens.back().type == FLOAT || 
                tokens.back().type == DOUBLE || tokens.back().type == CHAR || 
                tokens.back().type == STRING || tokens.back().type == VOID ||
                tokens.back().type == TRY || tokens.back().type == CATCH ||
                tokens.back().type == THROW || tokens.back().type == BREAK ||
                tokens.back().type == CONTINUE || tokens.back().type == SWITCH ||
                tokens.back().type == DEFAULT || tokens.back().type == CASE ||
                tokens.back().type == PUBLIC || tokens.back().type == PRIVATE ||
                tokens.back().type == PROTECTED) 
                ? tokens.back().value : "";
            symbolTable.addEntry(identifier, type, value, currentScope, line);
            return {IDENTIFIER, identifier, line, column};
        }    
        }

    Token tokenizeStringLiteral() {
        string literal;
        while (!isAtEnd() && source[current] != '"') {
            if (source[current] == '\\' && !isAtEnd()) {
                // Handle escape characters
                char escaped = advance();
                switch (escaped) {
                    case 'n': literal += '\n'; break;
                    case 't': literal += '\t'; break;
                    case '\\': literal += '\\'; break;
                    case '"': literal += '"'; break;
                    default: literal += escaped; break;
                }
            } else {
                literal += advance();
            }
        }

        if (isAtEnd()) {
            // Handle unterminated string literal
            return {UNKNOWN, "", line, column};
        }

        advance();  // Skip the closing quote
        return {STRING_LITERAL, literal, line, column};
    }
};


class IntermediateCodeGenerator {
public:
    struct ThreeAddressCode {
        string op;
        string arg1;
        string arg2;
        string result;

        string toString() const {
            if (arg2.empty()) {
                return result + " = " + arg1;
            }
            return result + " = " + arg1 + " " + op + " " + arg2;
        }
    };

    vector<ThreeAddressCode> generate(const vector<Token>& tokens) {
        vector<ThreeAddressCode> intermediateCode;
        stack<string> loopLabels;  // To handle loops (continue, break)
        string switchLabel;        // To handle switch-case

        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == IDENTIFIER && i + 1 < tokens.size() && tokens[i + 1].type == ASSIGN) {
                // Assignment handling
                if (i + 2 < tokens.size()) {
                    intermediateCode.push_back({"=", tokens[i + 2].value, "", tokens[i].value});
                }
            }

            // Handle arithmetic operations (existing)
            if (tokens[i].type == PLUS || tokens[i].type == MINUS ||
                tokens[i].type == MULTIPLY || tokens[i].type == DIVIDE) {
                if (i > 0 && i + 1 < tokens.size()) {
                    intermediateCode.push_back({tokenTypeToString(tokens[i].type),
                                                tokens[i - 1].value,
                                                tokens[i + 1].value,
                                                "temp" + to_string(intermediateCode.size())});
                }
            }

            // Handle control flow (break, continue)
            if (tokens[i].type == BREAK) {
                string breakLabel = "break" + to_string(intermediateCode.size());
                intermediateCode.push_back({"goto", "", "", breakLabel});
            } 
            else if (tokens[i].type == CONTINUE) {
                string continueLabel = "continue" + to_string(intermediateCode.size());
                intermediateCode.push_back({"goto", "", "", continueLabel});
            }

            // Handle switch-case
            if (tokens[i].type == SWITCH) {
                switchLabel = "switch" + to_string(intermediateCode.size());
                intermediateCode.push_back({"switch", "", "", switchLabel});
            }

            if (tokens[i].type == CASE) {
                if (!switchLabel.empty()) {
                    intermediateCode.push_back({"case", tokens[i + 1].value, "", switchLabel});
                }
            }

            // Handle reference operator (&)
            if (tokens[i].type == AND) {
                intermediateCode.push_back({"&", tokens[i + 1].value, "", tokens[i].value});
            }

            // Handle dereferencing operator (*)
            if (tokens[i].type == DEREFERENCE) {
                intermediateCode.push_back({"*", tokens[i + 1].value, "", tokens[i].value});
            }
        }

        return intermediateCode;
    }

private:
    string tokenTypeToString(TokenType type) {
        switch (type) {
        case PLUS: return "+";
        case MINUS: return "-";
        case MULTIPLY: return "*";
        case DIVIDE: return "/";
        case 'AND': return "&";
        case DEREFERENCE: return "*";
        default: return "?";
        }
    }
};

class AssemblyGenerator {
public:
    string generate(const vector<IntermediateCodeGenerator::ThreeAddressCode>& intermediateCode) {
        stringstream assembly;
        assembly << ".intel_syntax noprefix\n";
        assembly << ".global main\n\n";
        assembly << "main:\n";
        assembly << "    push rbp\n";
        assembly << "    mov rbp, rsp\n\n";

        for (const auto& code : intermediateCode) {
            generateInstruction(code, assembly);
        }

        assembly << "    mov rax, 0\n";
        assembly << "    leave\n";
        assembly << "    ret\n";

        return assembly.str();
    }

private:
    void generateInstruction(const IntermediateCodeGenerator::ThreeAddressCode& code, stringstream& assembly) {
        if (code.op == "=") {
            assembly << "    # Assignment\n";
            assembly << "    mov rax, " << code.arg1 << "\n";
            assembly << "    mov [" << code.result << "], rax\n";
        }
        else if (code.op == "+") {
            assembly << "    # Addition\n";
            assembly << "    mov rax, " << code.arg1 << "\n";
            assembly << "    add rax, " << code.arg2 << "\n";
            assembly << "    mov [" << code.result << "], rax\n";
        }
        else if (code.op == "*") {
            assembly << "    # Dereferencing pointer\n";
            assembly << "    mov rax, [" << code.arg1 << "]\n";
            assembly << "    mov [" << code.result << "], rax\n";
        }
        else if (code.op == "&") {
            assembly << "    # Getting reference (address)\n";
            assembly << "    lea rax, [" << code.arg1 << "]\n";
            assembly << "    mov [" << code.result << "], rax\n";
        }
        else if (code.op == "goto") {
            assembly << "    # Jump instruction\n";
            assembly << "    jmp " << code.result << "\n";
        }
        else if (code.op == "switch") {
            assembly << "    # Switch statement\n";
            assembly << code.result << ":\n";
        }
        else if (code.op == "case") {
            assembly << "    # Case label\n";
            assembly << "    cmp rax, " << code.arg1 << "\n";
            assembly << "    je " << code.result << "\n";
        }
    }
};

// Compiler Class
class Kabir_ka_Compiler {
public:
    void compile(const string &sourceCode) {
        Lexer lexer(sourceCode);
        vector<Token> tokens = lexer.tokenize();

        // Print tokens
        cout << "Tokens:\n";
        for (const auto &token : tokens) {
            cout << token.toString() << endl;
        }

        // Print Symbol Table
        lexer.symbolTable.print();

                    // Intermediate Code Generation
            IntermediateCodeGenerator intermediateGenerator;
            auto intermediateCode = intermediateGenerator.generate(tokens);

            // Print Intermediate Code
            cout << "\nIntermediate Code:\n";
            for (const auto &code : intermediateCode)
            {
                cout << code.toString() << endl;
            }

            // Assembly Code Generation
            AssemblyGenerator assemblyGenerator;
            string assemblyCode = assemblyGenerator.generate(intermediateCode);

            // Print Assembly Code
            cout << "\nAssembly Code:\n";
            cout << assemblyCode << endl;
        
        
    }
};

int main() {
    string sourceCode = R"(

int main() {
  
    int a = 10; 
    float b = 5.5f; 
    double c = 20.99; 
    char d = 'A'; 
    string str = "Hello, C++!"; 

    int sum = a + 5; 
    int diff = a - 3; 
    int product = a * 2; 
    int quotient = a / 2; 
    int remainder = a % 3; 


    a = 15; 


    if (a == 15) { 
        cout << "a is 15" << endl;
    }
    if (a != 10) { 
        cout << "a is not 10" << endl;
    }
    if (a < 20) { 
        cout << "a is less than 20" << endl;
    }
    if (a > 5) { 
        cout << "a is greater than 5" << endl;
    }
    if (a <= 15) { 
        cout << "a is less than or equal to 15" << endl;
    }
    if (a >= 15) { 
        cout << "a is greater than or equal to 15" << endl;
    }


    if (a > 10 && b < 6) { 
        cout << "a > 10 and b < 6" << endl;
    }
    if (a > 10 || b < 6) { 
        cout << "a > 10 or b < 6" << endl;
    }
    if (!(a > 10)) { 
        cout << "a is not greater than 10" << endl;
    }


    if (a == 15) {
        cout << "Inside IF block" << endl;
    } else { 
        cout << "Inside ELSE block" << endl;
    }

    while (b < 6) { 
        b += 1.0;
    }

    for (int i = 0; i < 5; i++) { 
        cout << "i = " << i << endl;
    }

    // Switch statement
    switch (a) { 
        case 10:
            cout << "Case 10" << endl;
            break;
        case 15:
            cout << "Case 15" << endl;
            break;
        default: 
            cout << "Default case" << endl;
            break;
    }

    
    for (int i = 0; i < 5; i++) {
        if (i == 3) { 
            break;
        }
        if (i == 1) { 
            continue;
        }
        cout << "i = " << i << endl;
    }

 
    try { // TRY
        if (a == 15) {
            throw "Error: a is 15"; 
        }
    } catch (const char* msg) { 
        cout << msg << endl;
    }


    int* ptr = &a; 
    cout << "Dereferenced value: " << *ptr << endl; 

 
    struct MyStruct {
        int value;
        MyStruct(int v) : value(v) {}
    };
    MyStruct obj(100);
    cout << "Object value: " << obj.value << endl; 

 
    testFunction(); 

    return 0; 
}


void testFunction() {
    cout << "Inside the testFunction!" << endl;
}

    )";

    Kabir_ka_Compiler Kabir_ka_Compiler;
    Kabir_ka_Compiler.compile(sourceCode);

    return 0;
}
