#ifndef my_string
#define my_string

#pragma once

#include<iostream>
#include<utility>
#include<stdexcept>
#include<cstring>
using std::endl,std::cout;
class str{
    char *p=nullptr;
    int len{};
    int capacity{};
    public:
    str():p(nullptr),len(0),capacity(0){}
    explicit str(const char* str){
        cout<<"CONST CHAR * ctor Called\n";
        capacity=strlen(str)+1;
        p= new char[capacity];
        if(!p) throw std::bad_alloc();
        // std::memcpy(p,str,strlen(str)); // copy Raw data(bits)
        while(*str){
            p[len]=str[len];
            len++;
        }
    }
    str(int count,char ch){  // Constructor with size and character
       if(count<=0 ) throw std::invalid_argument("Count Can not be zero");
       capacity=count;
       p=new char[capacity];
       if(!p) throw std::bad_alloc();
       while(len<count){
        p[len++]=ch;
       }
    }
    // i can my be a single chr or  const char*
    template<typename T> 
    str(std::initializer_list<T> lst){// that is why i use template 
        capacity=lst.size(); len=capacity;
        p=new char[capacity];
        if(!p) throw std::bad_alloc();
        int i=0;
        for(T ch : lst){
            p[i++]=ch;
        }
    }
     str(const str& other): len(other.len), capacity(other.capacity) { // copy constructor
    cout<<"Copy constructor for "<<*this<<"\n";;
        p=new char [other.capacity];
        int i=0;
        while(i<other.len){
            p[i]=other.p[i];
            i++;
        }
    }
     str& operator=(const str& obj){
        cout<<"Copy assignment  for str"<<*this<<"\n";
        if(this==&obj)  return *this;
        delete [] p; // release prevoius resource held by p
        capacity=obj.capacity;
        p=new char[capacity]; // allocate mem to hold new data 
        len=0;
         while(len<obj.len){
            p[len]=obj.p[len];  len++;
        }
        return *this;
    }
    explicit str(str&& obj)noexcept{   // move constructor
        cout<<"Move Ctor Of str \n";
        p=obj.p;
        len=obj.len;
        capacity=obj.capacity;
        obj.p=nullptr; obj.len=0; obj.capacity=0;
    }
    str& operator=(str&& obj)noexcept{
        cout<<"Move Assignment  of str \n";
        if(this==&obj)
            return *this;
        delete [] p;
        p=obj.p;
        len=obj.len;
        capacity=obj.capacity;
        obj.p=nullptr; obj.len=0; obj.capacity=0;
        return *this; 
    }
    ~str(){
        cout<<"Destructor of str "<<*this<<"\n";
        delete[] p;
    }
    void print(){
        for(int i=0;i<len;i++){
            cout<<p[i];
        }
        cout<<endl;
    }
    friend std::ostream& operator<<(std::ostream& out,const str& s){
        for(int i=0;i<s.len;i++){
            out<<s.p[i];
        }
        return out;
    }
    void reserve(int k){
        if(k>capacity){
            char *temp=new char[k];
            if(!temp) throw std::bad_alloc();
            memcpy(temp,p,len);
            delete [] p;
            p=temp;
            capacity=k;
        }
    }

    char& operator[](int i){
        cout<<"[] operator str\n";
        return p[i];
    }
    const char& operator[](int i)const{
        return p[i];
    }
};
// int main(){
//     str s={"Hellow"};
//     str t={"World "};
//     cout<<s<<endl;
//     cout<<t<<endl;
//     s+=t;
//     s.print();
//     // s.print();
//     // t.print();
// }

#endif // my_string