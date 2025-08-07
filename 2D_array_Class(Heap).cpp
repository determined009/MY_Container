#include<iostream>
#include<bits/stdc++.h>
#include<stdexcept>
using namespace std;
// template<typmname T>
class _2d_array{
    // int *ptr[3]; // ptr is an array of size 3 whose type is pointers that points to interger
    int** p=nullptr; //  p  is a pointer to  another pointer // remember we can index a pointer as if i was an aarray 
    int rows{};
    int cols{};
        class row_proxy{  // have access to private data of 2d array
            int* ptr_row;
            size_t col_no;
            public:
            row_proxy(int *row_ptr,size_t col_no):ptr_row(row_ptr),col_no(col_no){}
            // column Access
            int& operator[](size_t col){
                if(col>=col_no)   throw std::out_of_range("Column overflow");
                else if(col<0)    throw std::out_of_range("Invalid !! (Negative indexing not allowed)");
                    return ptr_row[col];
            }
            const int& operator[](size_t col)const{  // constant version
                if(col>=col_no)   throw std::out_of_range("Column overflow");
                else if(col<0)    throw std::out_of_range("Invalid !! (Negative indexing not allowed)");
                    return ptr_row[col];
            }
        };
    public:
    _2d_array(int r,int c){  // constructor  
        if(r <=0 || c<=0 && r>100 || c>100){
        cout<<"Invalid Dimensions\n"; return ;}
        rows=r; cols=c;
        cout<<"Allocating mem for "<<r<<" * "<<c<<"Matrix\n";
         p= new int*[r];  //  array of pointer one for each row .. p is now an array of pointers p-> to 1 row ,  (p+1) to 2 ro
        for(int i=0;i<r;i++)  // allocate memory for each roe ie 1 D array to which p points
             p[i] =  new int[c](); // zero initialize 
    }

        _2d_array(){// paremetrized constructor

    }
    ~_2d_array(){  // Destructor
        for(int i=0;i<rows;i++){
            delete [] p[i] ;  // delete memory for each roe one by one 
        }
        delete [] p;  // delete array of pointers ;
    }
    void print(){
        for(int r=0;r<rows;r++){
            cout<<"[";
            for(int c=0;c<cols;c++){
                cout<<*(*(p+r)+c)<<" ";
            }
            cout<<"]\n";
        }
    }
    row_proxy operator[](int row){
        if(row>=rows)   throw std::out_of_range("ROW out ound");
        return row_proxy(p[row],cols);
            // return *(*(p+r)+c)
    }
    const row_proxy operator[](int row)const{
        if(row>=rows)   throw std::out_of_range("ROW out of bound");
        return row_proxy(p[row],cols);
    }
    int operator()(int c,int r){
        if((r>rows || cols>c) || (r<0 || c<0) ) throw std::out_of_range("INvalid Indexing!");
        return *(*(p+c)+r);
    }
};
int main(){
 _2d_array m{2,3}={[1,2,3],[4,5,6]};
//  m= 
 cout<<m(1,3)<<endl;
    // m={1,2,3,4,5,6,7,8,9};
    // m.print();
    return 0;
}