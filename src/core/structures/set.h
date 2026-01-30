#ifndef SET_H
#define SET_H

#include "binary_tree.h"
#include <functional>

template <typename T>
class Set 
{
private:
    BinaryTree<T> tree;

public:
    Set() = default;

    void Add(const T& value) 
    {
        if (!tree.contains(value)) 
        {
            tree.insert(value);
        }
    }

    void Remove(const T& value) 
    {
        tree.remove(value);
    }

    bool Contains(const T& value) const 
    {
        return tree.contains(value);
    }

    int GetCount() const 
    {
        int count = 0;
        tree.traverseInOrder([&count](const T& value) 
        {
            count++;
        });
        return count;
    }

    bool IsEmpty() const 
    {
        return tree.isEmpty();
    }

    void Clear() 
    {
        tree.clear();
    }

    template<typename Func>
    void ForEach(Func func) const 
    {
        tree.traverseInOrder(func);
    }

    Set<T> Union(const Set<T>& other) const 
    {
        Set<T> result = *this;
        other.ForEach([&result](const T& value) 
        {
            result.Add(value);
        });
        return result;
    }

    Set<T> Intersection(const Set<T>& other) const 
    {
        Set<T> result;
        this->ForEach([&other, &result](const T& value) 
        {
            if (other.Contains(value)) 
            {
                result.Add(value);
            }
        });
        return result;
    }

    Set<T> Difference(const Set<T>& other) const 
    {
        Set<T> result;
        this->ForEach([&other, &result](const T& value) 
        {
            if (!other.Contains(value)) 
            {
                result.Add(value);
            }
        });
        return result;
    }
};

#endif