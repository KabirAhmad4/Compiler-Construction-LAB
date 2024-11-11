#include<iostream>
using namespace std;
int main() {

    class Info{
        public: 
        string name;
        int age;
        string roll_no;
        char section; 

        Info(string name,int age, string roll_no, char section ) {  // Constructor
        this->name = name;
        this->age = age;
        this->roll_no =roll_no;
        this->section= section;
        }
    };

    Info x= Info("Kabir",21,"2021-CS-4",'A');
    cout<<x.age;


    return 0;
} 