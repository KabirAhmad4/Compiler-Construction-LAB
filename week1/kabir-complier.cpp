#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Error: Invalid number of arguments. Usage: mycompiler <filename>" << endl;
        return 1;
    }

    string filename = argv[1];

    ifstream MyReadFile(filename);

    if (!MyReadFile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return 1;
    }

    string myText;
    while (getline(MyReadFile, myText)) {
        cout << myText << endl;
    }

    MyReadFile.close();

    return 0;
}
