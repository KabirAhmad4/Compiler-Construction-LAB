#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string readFileToString(const string& filePath) {
    ifstream file(filePath);
    
    if (!file.is_open()) {
        cout << "Failed to open the file!" << endl;
        return "";  
    }

    string fileContent = "";
    char c;
    while (file.get(c)) { 
        fileContent += c;
    }

    file.close();
    return fileContent;
}

void printContent(const string& content) {
    cout << "File Content:" << endl;
    cout << content << endl;
}


bool isSpecialChar(char c, const char specialChars[], int specialCharCount) {
    for (int i = 0; i < specialCharCount; i++) {
        if (c == specialChars[i]) {
            return true;
        }
    }
    return false;
}

vector<string> tokenize(const string& fileContent, const char specialChars[], int specialCharCount) {
    vector<string> tokens;
    string currentToken = "";

    for (int i = 0; i < fileContent.length()-1; i++) {
        char c = fileContent[i];


        if (isSpecialChar(c, specialChars, specialCharCount)) {

            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken = ""; 
            }

            tokens.push_back(string(1, c));
        } else {
            currentToken += c;
        }
    }


    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

int main() {

    string filePath = "E:\\Study\\Semester 7\\Compiler Construction\\week4\\code.txt";


    string fileContent = readFileToString(filePath);
    
    char specialChars[] = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '-', '=', '+', '[', ']', '{', '}', ';', ':', ',', '.', '/', '?'};
    int specialCharCount = sizeof(specialChars) / sizeof(specialChars[0]);

    vector<string> tokens = tokenize(fileContent, specialChars, specialCharCount);

    cout << "\nTokens:\n";
    for (const string& token : tokens) {
        cout << token << endl;
    }

    return 0;
}
