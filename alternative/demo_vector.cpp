#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
//#include "vector.h"
#include <mutex>
using namespace std;
template <typename T>
class Vector{
private:
    T* data_;
    size_t capacity_;
    size_t size_;

public:
    explicit Vector(size_t capacity)
        : data_(new T[capacity]), capacity_(capacity), size_(0) {}

    ~Vector() {
        delete[] data_;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            return;
        }
        data_[size_] = value;
        ++size_;
    }

    // Sobrecarga para mantener compatibilidad con el demo actual.
    void push_back(const T& value, size_t /*unused*/) {
        push_back(value);
    }

    template <typename Func>
    void ForEach(Func func) {
        for (size_t i = 0; i < size_; ++i) {
            func(data_[i]);
        }
    }

    size_t size() const {
        return size_;
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }
};

void AddOne(int& n){
    static mutex mtx;
    scoped_lock lock(mtx);
    ++n;
}
void AddOneWithoutLock(int& n){
    ++n;
}

template <typename T>
void Add(T& n, T value){
    n += value;
}




template <typename Func>
void DemoVector(Func func){
    Vector<int> v(4);
    v.push_back(0, 0);
    v.push_back(0, 0);
    v.push_back(0, 0);
    v.push_back(0, 0);

    // Cada thread itera el vector 100,000 veces e incrementa cada elemento
    // Sin sincronizacion → race condition en los contadores
    auto worker = [&v, func](int thread_id){
        for(int i = 0; i < 100000; i++)
            v.ForEach(func);
        cout << "Thread " << thread_id << " terminado\n";
    };

    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();

    // Resultado esperado sin race condition: 4 elementos * 100000 * 5 threads = 500000
    for(int i = 0; i < v.size(); i++){
        cout << "v[" << i << "] = " << v[i] << endl;
    }
        
}

int main(){
    cout<<"Demo Vector con race condition (sin lock):"<<endl;
    DemoVector(AddOneWithoutLock);
    cout<<"Demo Vector con race condition (con lock):"<<endl;
    DemoVector(AddOne);
    

}