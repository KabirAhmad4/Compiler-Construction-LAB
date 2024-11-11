#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
using namespace std;

int main(int argc, char* argv[]) {
    // Check if a file name is passed
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << "input.txt" << endl;
        return 1;
    }

    // Open the file provided via command line
    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    // Read file content into a string
    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    // Close the file after reading
    file.close();

    // Now, process the code using lexer and parser
    Lexer lexer(code);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    cout << "Parsing completed successfully!" << endl;
    return 0;
}