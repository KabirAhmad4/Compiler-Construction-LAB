//2021-CS-4
//Kabir Ahmad
// Task 1

#include<iostream>
#include<cctype>
#include<algorithm>
#include<vector>
#include<string>
#include<unordered_set>
using namespace std;

int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}


int generateRandomDigit() {
    return generateRandomNumber(0, 9);
}

string generateISBN_number(){
    int a = 978;                
    int b = 0;                   
    int c = generateRandomNumber(10, 99);  
    int d = generateRandomNumber(100000, 999999); 
    int e = generateRandomDigit();  


    string ISBN= ""+to_string(a)+"-"+to_string(b)+"-"+to_string(c)+"-"+to_string(d)+"-"+to_string(e)+"";
   
    return ISBN;
}


class Book {        
  public:          
    string ISBN;  
    string Title;  
    string Status;      
    Book(string Title){ 
      ISBN= generateISBN_number();
      Title= Title;
      Status="Available";
      cout<<"Added Book: "+Title + " ("+ ISBN +") , Status: "+ Status<<endl;   
    }
    void Update_Book(){
   
      Status="Checked out";
      cout<<"Updated Book: "+Title + " ("+ ISBN +") , Status: "+ Status<<endl;   
    }


};



 
int main(){

    vector<Book>Books;
    Books.push_back(Book("The C Programming Language "));
    Books.push_back(Book(" Learning Python "));

    Book x(" Eloquent Javascript");
    Books.push_back(x);
    x.Update_Book();
   

cout<<"Final Book List: "<<endl;
        for (int i=0; i<Books.size();i++ )
        {
            if(Books[i].Status=="Available")
            {cout<<"ISBN: "+Books[i].ISBN +" ,Title: "+Books[i].Title +", Available: True"<<endl ; 
            }else{
               cout<<"ISBN: "+Books[i].ISBN +" ,Title: "+Books[i].Title +", Available: False"<<endl;  
            }      
        }  
 
    return 0;
}