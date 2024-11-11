//2021-CS-4
//Kabir Ahmad
// Task 2

#include<iostream>
#include<cctype>
#include<algorithm>
#include<queue>
#include<string>
#include<unordered_set>
using namespace std;


 
int main(){

 queue<string> ticketQueue;
 int tn=1000;

 for(int i=0; i <3;i++){
    ticketQueue.push(to_string(tn+1));
    cout<<"Visitor with Ticket Number "+ to_string(tn+1)+"joined the line"<<endl;
 }

string x= ticketQueue.front();
cout<<"Processed Visitor with Ticket "+ x; 
string x= ticketQueue.back();
cout<<" Visitor with Ticket "+ x+"joined the line."; 


 return 0;
}
