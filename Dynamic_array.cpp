// #ifndef Dynamic_array
// #define Dynamic_array
// #pragma once
// #include"string header.h"
#include<iostream>
#include<stdexcept>
#include<cstring>
#include<climits>
#include<concepts>
#include<utility>
#include<type_traits>
using std::cout,std::endl;
 namespace concepts{
    template<typename type,typename U>
    concept is_lvalue_non_copyable = std::is_lvalue_reference_v<U> &&  // if lvalue and  copy ctor os deleted ERROR !!
                         !std::is_copy_constructible_v<std::remove_reference_t<type>> && 
                         std::is_move_constructible_v<std::remove_reference_t<type>>;
    template<typename type,typename U>
    concept is_rvalue_moveable = std::is_rvalue_reference_v<U> && 
                        std::is_nothrow_move_constructible_v<std::remove_reference_t<type>>;
    }                    
template<class T=int>
class aray{
    T* ptr=nullptr;
    int size{};
    int capacity{};
    public:
    aray(){
        size=0; reserve(2);
        cout<<"\nDefault constructor of aray class \n";
        }
    // aray(std::initializer_list<T> lst)=delete; 
    // :size(lst.size()),capacity(lst.size()?lst.size():2){ // work left over
    //     cout<<"Paremetrized CONSTRUCTOR "<<endl;
    //     ptr= new T [capacity];
    //     if(!ptr)
    //         throw std::bad_alloc();
    //     int i=0;
    //     cout<<"Works Begin\n";
    //     for(const T& ele: lst ){  // still uses copy constructor need optimization
    //         ptr[i++]=ele;  //  call copy assignment operator of data type T 
    //     }
    // }
    // #if 0
                
    template<typename... Args>  // parametre pack
    aray (Args&&... args){
        cout<<"In Parametric Constructor\n";
        size=sizeof...(args);
        capacity=size?size:2;
        reserve(capacity);
        // ptr=static_cast<T*>(operator new (capacity*sizeof(T)));
        // if(!ptr) throw std::bad_alloc();
        // int i=0;
        // ( new(ptr+i++) T(std::forward<Args>(args)),... ); //fold over comma
        // ptr=static_cast<T*>(operator new (capacity*sizeof(T)));
        // if(!ptr) throw std::bad_alloc();
        int index=0;
        try{
            // lambda 
           ([&]<typename Arg_type>(Arg_type&& arg){
                // using Argtype=decltype(args);
                if constexpr(concepts::is_lvalue_non_copyable<T,Arg_type>)
                    (void) new(ptr + index++) T(std::move(arg));

                else if constexpr (concepts::is_rvalue_moveable<T,Arg_type>)
                    (void) new(ptr + index++) T(std::move(arg));

                else
                    (void) new(ptr+ index++) T(std::forward<Arg_type>(arg));
           }(std::forward<Args>(args)),...); // IIFE (Immediately Invoked Function Expression)

            // (new (ptr+index ++ ) T(std::forward<elet>(args)),...); // fold expression over comma 
        }
        catch(...){
            for(size_t i=0;i<index;i++){
                ptr[i].~T();
            }
            operator delete(ptr);
            cout<<"Error in constructing elements\n";
            throw;
        }
        cout<<"Exiting Parametric Constructor\n";
    }
    // #endif
    aray(const T& obj) noexcept{ // copy constructor
    cout<<"Copy Constructor to gen copy of :"<<obj<<endl;
        ptr= static_cast<T*>(operator new (sizeof(T)*obj.capacity));
        if(!ptr) throw std::bad_alloc();
        for(int i=0;i<obj.size;i++){
            new(&ptr[i]) T(obj.ptr[i]);
        }
        size=obj.size;
        capacity=obj.capacity;
    }
    aray& operator=(const aray& obj){ // copy assignment operator
        cout<<"ASSIGNMENT OPEARTOR "<<obj<<endl;
        if(this==&obj) return *this;
        for(int i=0;i<obj.size;i++){
            ptr[i].~T();  // release prevois held resourses
        }
        operator delete(ptr); // We are done with mem pointer by ptr
        ptr=static_cast<T*>(operator new(obj.size*sizeof(T)));
        if(!ptr) throw std::bad_alloc();
        for(int i=0;i<obj.size;i++){
            (void) new(&ptr[i]) T(obj.ptr[i]); // call copy ctor of T
        } 
        size=obj.size; capacity=obj.capacity; 
        return *this;
    }
    aray& operator=(aray&& obj)noexcept{   //------------------- MOve Assignment operator
    cout<<"ASSIGNMENT OPEARTOR "<<obj<<endl;
        if(this==&obj) // avoid self assignment
            return *this; // free momory that is previously pointer to by ptr 
        if constexpr(std::is_trivially_destructible_v<T> ){
            operator delete(ptr); // no need to call dctor
            ptr=nullptr; 
        }
        else{
            for(int i=0;i<size;i++)
                ptr[i].~T();
            operator delete(ptr);    
            ptr= nullptr;
        }
        ptr= static_cast<T*>(operator new(obj.size*sizeof(T)));
        if(!ptr) throw std::bad_alloc();
        // Move data 
        for(int i=0;i<obj.size;i++)
            ptr[i]=std::move(obj.ptr[i]);
        size=obj.size;
        capacity=obj.capacity;
        return *this;
    }
    aray(aray&& obj) noexcept // --------- MOVE CONSTRUCTOR
    { // steal resources of obj and gives ownership to ptr and set 
    // obj.ptr to NULL to Avoid the deletion of resources 
    // now being owned by ptr opon the execution of destructor of obj
    // AKA HEIST 
        cout<<"IN MOVE CONSTRUCTOR for STEALING(MOVING ) "<<obj<<endl;
        ptr=obj.ptr;  // steal resources
        size=obj.size;
        capacity=obj.capacity;
        obj.ptr=nullptr; obj.size=0;obj.capacity=0;  // Left in empty state
    }
    ~aray(){ // Destructor
    // cout<<"\nDESTRUCTOR Called for aray "<<*this<<endl;
        if constexpr (std::is_trivially_destructible_v<T>) {
            operator delete(ptr); // no need to call dcot
            ptr=nullptr; size=0; capacity=0; // reset to empty state
            return;
        }
        for(int i=0;i<size;i++){
            ptr[i].~T();
        }
        operator delete(ptr);
    }
    // Utility Functions
    // template<typename K, typename... Args>
    // concept constructible = std::constructible_from<K, Args...>;
    // Removed invalid concept declaration; use std::convertible_to directly in requires clause.

    template<typename... Arg>
    void emplace_back(Arg&&... parametre) 
        requires (std::is_constructible_v<T, Arg...> || std::is_convertible_v<Arg..., T>)
    {
        cout<<"In emplace_back\n";
        // size=1;
        if(size==capacity) reserve(capacity*2); 
        // if constexper(std::is_const_v<std::remove_reference_t<Arg>>...) // if const obj uses copy ctor
        //     // if const obj uses copy ctor
        //     new(ptr+size) T(Args...);
        // else
        //     new(ptr+size) T(std::move((parametre)...));//  directly at the end of vector
        new(ptr+size) T(std::forward<Arg>(parametre)...); // 
        size++;
        cout<<"Exiting emplace_back\n";
    }
    void push_back(T&&  ele){   //   this Version copies data(uses c ctor)
        cout<<"In Push_back f&& \n";
        if(size==capacity)
            reserve(capacity*2);
        cout<<"Next \n";
        // ptr[size++]=std::forward<F>(ele); // conditionally casts to rvalue_ref and call move ctor
        new(ptr+size)  T(std::forward<T>(ele)); size++;
    }
    void reserve(int new_cap) { // Preallocate memory
        if( new_cap<=capacity) return ;
            // Allocate raw memory (no constructions)
            // if constexpr(std::is_trivially__v<T>) {
            //     T* temp=new T[new_cap];
            //     std::memcpy(temp, ptr, size * sizeof(T));
            //     delete [] ptr;
            //     ptr=temp;  capacity=new_cap; return;
            // }
            // else if constexper(std::is_nothrow_move_constructible_v<T>){
            //     T* temp = static_cast<T*>(operator new(new_cap * sizeof(T))); // allocate Raw
            //     for(int i = 0; i < size; i++) {
            //         new(temp+i) T(std::move(ptr[i])); // move construction
            //         ptr[i].~T(); // class dctor on move from objects
            //     }
            //     operator delete(ptr); // gives chuck of mem pointed by ptr back to operating system
            T* temp=static_cast<T*>(operator new (new_cap*sizeof(T)));
            if(!temp) throw std::bad_alloc();
            if constexpr (std::is_trivially_copyable_v<T>){
                    std::memcpy(temp,ptr,size*sizeof(T));
            }
            else{
                int i=0;
                try{
                    for(int i=0;i<size;i++)
                        new(temp+i) T(std::move_if_noexcept(ptr[i]));
                }
                catch(...){
                    for(int j=0;j<i;j++)
                        temp[j].~T();
                    operator delete(temp);
                    throw; // rethroe to caller 
                }
            }
            for(int i=0;i<size;i++)
                ptr[i].~T();
            capacity=new_cap;
            operator delete(ptr);
            ptr=temp;
    }
    void push_(T i){
        if(size==capacity)
            reserve(capacity*2);
        ptr[size++]=i; // calls copy ctor of T
    }
    int get_size(){return size;}
    int get_cap(){return capacity;}
    void clear(){
        if constexpr (!std::is_trivially_destructible_v<T>)
            for(int i=0;i<size;i++)
                ptr[i].~T(); // call drec of object T(beavuse in case if it it self handle resourses)
        operator delete(ptr);// Just give back the memory to sys poibterd by ptr (Even the element it self have been)
        ptr= nullptr;size=0; capacity=0;
    }
    friend std::ostream& operator<<(std::ostream& out,const aray& ar){
        if(!ar.ptr){
            cout<<"Empty Vector\n";
        }
        for(int i=0;i<ar.size;i++){
           out<< ar.ptr[i]<<" ";
        }
        return out;
    }
    T& operator[](int index){
        if(index<0 || index >size)
            throw std::out_of_range("--->Index Out Of Bound Or INVALID INDEX.");
        return ptr[index]; 
    }
    static void swap(aray& lhv,aray& rhv){
        aray temp=std::move(lhv);
        lhv=std::move(rhv);
        rhv=std::move(temp);
    }
    void reverse(){
        T* l=&ptr[0];
        T* r=&ptr[size-1];
        while(l<r) // compare  addresses 
        {
            T temp=*l;
            *l=*r;
            *r=temp;
            l++;r--;
        }
    }
    void rotate_r(int k=0){
        if(size==0 || k==0)
        k=k%size;
        if constexpr( std::is_integral_v<T>){
        int base=100000;
        for(int i=0;i<size;i++){
            int new_index=(i+k)%size; // to avoid unneccessary rotation // ptr[i]=ptr[i]+(base * ptr[(i+k)]);
            ptr[new_index] += base * (ptr[i] % base);
        }
        for(int i=0;i<size;i++)
        ptr[i]/=base;
        }
        else{
            // std::unique_ptr<T[]> temp(new T[size
            T* temp=new T[size]; if(!temp) throw std::bad_alloc();
            for(size_t i=0;i<size;i++){
                temp[(i+k)%size]=std::move(ptr[i]);
            }
            delete [] ptr;
           ptr=temp;
        }
    }
    bool find(T key){
        if(size==0) return false;
        for(int i=0;i<size;i++){
            if(ptr[i]==key) return true;
        }
        return false;
    }
        
};  
// aray foo(){
//     return {10,20,30};
// }
int main()
{
    // aray arr={1,2,3,4,5,6,7};
    aray<std::string> s{"Hellow","World "};
    // aray a1=std::move(arr);
    // aray a1=aray({10,20,30}); // Copy elision
    // aray a1= std::move(aray({10,20,30,40})); // we can steal (MOVE ) temp nameless aray into a1
    // arr.reserve(10); // Preallocate memory for expected size
    s.push_back("James Bond ");
    s.push_back("Jk rowling ");
    // s[2]=str("world change ");
    s.emplace_back("I am Here Aren't YOU");
    for(int i=0;i<s.get_size();i++){
        cout<<s[i]<<" ";
    }
    return 0;
}
// #endif // MY_HEADER_H