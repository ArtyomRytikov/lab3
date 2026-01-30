#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "DynamicArray.h"
#include "LinkedList.h"

template <class T>
class IIterator 
{
public:
    virtual ~IIterator() = default;
    virtual T GetCurrentItem() const = 0;
    virtual bool HasNext() const = 0;
    virtual void Next() = 0;
    virtual void Reset() = 0;
};


template <class T>
class ICollection 
{
public:
    virtual ~ICollection() = default;
    virtual T Get(size_t index) const = 0;
    virtual size_t GetCount() const = 0;
    virtual IIterator<T>* CreateIterator() const = 0;
    ICollection() = default;
    ICollection(const ICollection<T>& other) = default;
};

template <class T>
class Sequence : public ICollection<T> 
{
public:
    virtual ~Sequence() = default;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    
    T Get(size_t index) const override 
    {
        return Get(static_cast<int>(index));
    }
    
    size_t GetCount() const override 
    {
        return static_cast<size_t>(GetLength());
    }
    
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual IIterator<T>* CreateIterator() const = 0;
};

template <class T>
class ArrayIterator : public IIterator<T> 
{
private:
    const ImmutableArraySequence<T>* sequence;
    int currentIndex;

public:
    ArrayIterator(const ImmutableArraySequence<T>* seq) : sequence(seq), currentIndex(0) {}

    T GetCurrentItem() const override 
    {
        if (currentIndex < 0 || currentIndex >= sequence->GetLength()) 
        {
            throw std::out_of_range("Iterator is out of range");
        }
        return sequence->Get(currentIndex);
    }

    bool HasNext() const override 
    {
        return currentIndex < sequence->GetLength();
    }

    void Next() override 
    {
        if (HasNext())
        {
            currentIndex++;
        }
    }

    void Reset() override 
    {
        currentIndex = 0;
    }
};

template <class T>
class ListIterator : public IIterator<T> 
{
private:
    const ImmutableListSequence<T>* sequence;
    int currentIndex;

public:
    ListIterator(const ImmutableListSequence<T>* seq) : sequence(seq), currentIndex(0) {}

    T GetCurrentItem() const override 
    {
        if (currentIndex < 0 || currentIndex >= sequence->GetLength()) 
        {
            throw std::out_of_range("Iterator is out of range");
        }
        return sequence->Get(currentIndex);
    }

    bool HasNext() const override 
    {
        return currentIndex < sequence->GetLength();
    }

    void Next() override 
    {
        if (HasNext()) 
        {
            currentIndex++;
        }
    }

    void Reset() override 
    {
        currentIndex = 0;
    }
};

template <class T>
class ImmutableArraySequence : public Sequence<T> 
{
protected:
    DynamicArray<T> array;

public:
    ImmutableArraySequence() : array() {}
    ImmutableArraySequence(int size) : array(size) {}
    ImmutableArraySequence(T* items, int count) : array(items, count) {}
    ImmutableArraySequence(const DynamicArray<T>& arr) : array(arr) {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : array(other.array) {}
    
    T GetFirst() const override 
    {
        if (array.GetSize() == 0) throw std::out_of_range("Последовательность пуста");
        return array.Get(0);
    }
    
    T GetLast() const override 
    {
        if (array.GetSize() == 0) throw std::out_of_range("Последовательность пуста");
        return array.Get(array.GetSize() - 1);
    }

    T Get(int index) const override 
    {
        return array.Get(index);
    }
    
    int GetLength() const override 
    {
        return array.GetSize();
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        if (startIndex < 0 || endIndex >= array.GetSize() || startIndex > endIndex) 
        {
            throw std::out_of_range("Неверный диапазон индексов");
        }
        
        int subLength = endIndex - startIndex + 1;
        T* subItems = new T[subLength];
        for (int i = 0; i < subLength; i++) 
        {
            subItems[i] = array.Get(startIndex + i);
        }
        
        Sequence<T>* result = new ImmutableArraySequence<T>(subItems, subLength);
        delete[] subItems;
        return result;
    }
    
    IIterator<T>* CreateIterator() const override 
    {
        return new ArrayIterator<T>(this);
    }
};

template <class T>
class MutableArraySequence : public ImmutableArraySequence<T> 
{
public:
    using ImmutableArraySequence<T>::ImmutableArraySequence;
    
    MutableArraySequence() : ImmutableArraySequence<T>() {}
    MutableArraySequence(int size) : ImmutableArraySequence<T>(size) {}
    MutableArraySequence(T* items, int count) : ImmutableArraySequence<T>(items, count) {}
    MutableArraySequence(const DynamicArray<T>& arr) : ImmutableArraySequence<T>(arr) {}
    
    T& GetRef(int index) 
    {
        return this->array.Get(index);
    }
    
    void Set(int index, T value) 
    {
        this->array.Set(index, value);
    }
    
    void Append(T item) 
    {
        this->array.InsertAt(item, this->array.GetSize());
    }
    
    void Prepend(T item) 
    {
        this->array.InsertAt(item, 0);
    }
    
    void InsertAt(T item, int index) 
    {
        this->array.InsertAt(item, index);
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        if (startIndex < 0 || endIndex >= this->array.GetSize() || startIndex > endIndex) 
        {
            throw std::out_of_range("Неверный диапазон индексов");
        }
        
        int subLength = endIndex - startIndex + 1;
        T* subItems = new T[subLength];
        for (int i = 0; i < subLength; i++) 
        {
            subItems[i] = this->array.Get(startIndex + i);
        }
        
        Sequence<T>* result = new MutableArraySequence<T>(subItems, subLength);
        delete[] subItems;
        return result;
    }
    
    IIterator<T>* CreateIterator() const override 
    {
        return new ArrayIterator<T>(static_cast<const ImmutableArraySequence<T>*>(this));
    }
};

template <class T>
class ImmutableListSequence : public Sequence<T> 
{
protected:
    LinkedList<T> list;

public:
    ImmutableListSequence() : list() {}
    ImmutableListSequence(T* items, int count) : list(items, count) {}
    ImmutableListSequence(const LinkedList<T>& lst) : list(lst) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : list(other.list) {}
    
    T GetFirst() const override 
    {
        return list.GetFirst();
    }
    
    T GetLast() const override 
    {
        return list.GetLast();
    }
    
    T Get(int index) const override 
    {
        return list.Get(index);
    }
    
    int GetLength() const override 
    {
        return list.GetLength();
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        LinkedList<T> subList = list.GetSubList(startIndex, endIndex);
        return new ImmutableListSequence<T>(subList);
    }
    
    IIterator<T>* CreateIterator() const override 
    {
        return new ListIterator<T>(this);
    }
};

template <class T>
class MutableListSequence : public ImmutableListSequence<T> 
{
public:
    using ImmutableListSequence<T>::ImmutableListSequence;
    
    T& GetRef(int index) 
    {
        return this->list.Get(index);
    }
    
    void Set(int index, T value) 
    {
        this->list.Get(index) = value;
    }
    
    void Append(T item) 
    {
        this->list.Append(item);
    }
    
    void Prepend(T item) 
    {
        this->list.Prepend(item);
    }
    
    void InsertAt(T item, int index) 
    {
        this->list.InsertAt(item, index);
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        LinkedList<T> subList = this->list.GetSubList(startIndex, endIndex);
        return new MutableListSequence<T>(subList);
    }
    
    IIterator<T>* CreateIterator() const override 
    {
        return new ListIterator<T>(static_cast<const ImmutableListSequence<T>*>(this));
    }
};

#endif