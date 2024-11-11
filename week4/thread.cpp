#include <iostream>
#include <thread>
using namespace std;

void print1to5() {
    for (int i = 1; i <= 5; ++i)
        cout << i << " ";
    cout << endl;
}

void print6to10() {
    for (int i = 6; i <= 10; ++i)
        cout << i << " ";
    cout << endl;
}

int main() {
    thread t1(print1to5);
    thread t2(print6to10);

    t1.join();
    t2.join();

    return 0;
}
