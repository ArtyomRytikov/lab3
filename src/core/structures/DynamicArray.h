#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>

template <class T>
class DynamicArray 
{
private:
    T* items;
    int size;

public:
    DynamicArray() : items(nullptr), size(0) {}
    DynamicArray(int size) : size(size) 
    {
        if (size < 0) throw std::invalid_argument("Размер не может быть отрицательным");
        items = new T[size]();
    }
    DynamicArray(T* items, int count) : size(count) 
    {
        if (count < 0) throw std::invalid_argument("Количество не может быть отрицательным");
        this->items = new T[size];
        for (int i = 0; i < size; i++) 
        {
            this->items[i] = items[i];
        }
    }
    DynamicArray(const DynamicArray<T>& other) : size(other.size) 
    {
        items = new T[size];
        for (int i = 0; i < size; i++) 
        {
            items[i] = other.items[i];
        }
    }
    DynamicArray(DynamicArray<T>&& other) noexcept : items(other.items), size(other.size) 
    {
        other.items = nullptr;
        other.size = 0;
    }
    ~DynamicArray() 
    {
        delete[] items;
    }
    DynamicArray<T>& operator=(const DynamicArray<T>& other) 
    {
        if (this != &other) 
        {
            delete[] items;
            size = other.size;
            items = new T[size];
            for (int i = 0; i < size; i++) 
            {
                items[i] = other.items[i];
            }
        }
        return *this;
    }
    DynamicArray<T>& operator=(DynamicArray<T>&& other)  
    {
        if (this != &other) 
        {
            delete[] items;
            items = other.items;
            size = other.size;
            other.items = nullptr;
            other.size = 0;
        }
        return *this;
    }
    T& operator[](int index) 
    {
        if (index < 0 || index >= size) throw std::out_of_range("Индекс вне диапазона");
        return items[index];
    }
    const T& operator[](int index) const 
    {
        if (index < 0 || index >= size) throw std::out_of_range("Индекс вне диапазона");
        return items[index];
    }
    T& Get(int index) 
    {
        return operator[](index);
    }
    const T& Get(int index) const 
    {
        return operator[](index);
    }
    int GetSize() const 
    {
        return size;
    }
    void Set(int index, T value) 
    {
        operator[](index) = value;
    }
    void Resize(int newSize) 
    {
        if (newSize < 0) throw std::invalid_argument("Размер не может быть отрицательным");
        if (newSize == size) return;
        T* newItems = new T[newSize]();
        int copySize;
        if (newSize < size)
        {
            copySize = newSize;
        }
        else
        {
            copySize = size;
        }
        for (int i = 0; i < copySize; i++) 
        {
            newItems[i] = items[i];
        }
        delete[] items;
        items = newItems;
        size = newSize;
    }
    void InsertAt(T item, int index) 
    {
        if (index < 0 || index > size) throw std::out_of_range("Индекс вне диапазона");
        Resize(size + 1);
        for (int i = size - 1; i > index; i--) 
        {
            items[i] = items[i - 1];
        }
        items[index] = item;
    }
};

#endif