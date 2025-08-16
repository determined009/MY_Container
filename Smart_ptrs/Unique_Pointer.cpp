#include<iostream>
#include<type_traits>
#include<utility>
namespace smart_ptrs{
// Deleter class
    template<typename Del_type>
    class default_Deleter{
        public:
        constexpr default_Deleter()=default;
        void operator()(Del_type* p) {
            std::cout<<"Default Deleter\n";
            // if constexpr (!std::is_trivially_destructible<Del_type>::value) 
            delete p; // Free memory
        }
    };

    template<typename T,typename Deleter=default_Deleter<T>>
    class unique__ptr{
        T* p=nullptr;
        Deleter deleter;
        // using pointer=T*; // alias
        public:
        unique__ptr()=default;
        explicit unique__ptr(T* ptr){
            std::cout<<"Just Ptr ctor\n";
            p=ptr;
        }
        explicit unique__ptr(T* ptr,Deleter d):p(ptr),deleter(d){
            std::cout<<"Deleter Ctor\n";
        }
        ~unique__ptr(){
            if(p)
                deleter(p);
        }
    unique__ptr(const unique__ptr&)=delete;
    unique__ptr& operator=(const unique__ptr&)=delete;
    unique__ptr(unique__ptr&& other )noexcept :p(std::exchange(other.p,nullptr)){
        deleter=std::move(other.deleter);
    }

    unique__ptr& operator=(unique__ptr&& other)noexcept{
        if(&this->p!=&other.p){
        if(p)
            deleter(p);
        p=std::exchange(other.p,nullptr);
        deleter=std::move(other.deleter);
        }
        return *this;    
    }


        operator*() const{
            return *p;
        }
        operator->() const {
            return p;
        }
        T* get()const{
            return p;
        }
        explicit operator bool()const{ // 
            return p!=nullptr
        }
        bool operator!()const{
            return p==nullptr;
        }
    };
}
template<typename K>
class my_deleter{
    public:
    constexpr my_deleter(){std::cout<<"Ctor\n";}
    void operator()(K* p){
        std::cout<<"Calling my own deleter\n";
        delete p;
    }
};
// auto my_deleter
int main(){
    // auto lad=[](template<typename T>){delete *T;}
    smart_ptrs::unique__ptr<int> ptr1 (new int(30));
    std::cout<<*ptr1<<std::endl;
    std::cout<<sizeof(ptr1)<<std::endl;

    smart_ptrs::unique__ptr<int> ptr2(new int(100));
    std::cout<<*ptr2<<std::endl;
    std::cout<<sizeof(ptr2)<<std::endl;

    ptr2=std::move(ptr1);
    std::cout<<*ptr2;
    return 0;
}