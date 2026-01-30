#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "binary_tree.h"
#include <functional>

template <typename T>
class PriorityQueue 
{
private:
    BinaryTree<T> tree;
    std::function<bool(const T&, const T&)> comparator;

public:
    PriorityQueue(std::function<bool(const T&, const T&)> comp = std::less<T>()) : comparator(comp) {}

    void Enqueue(const T& value) 
    {
        tree.insert(value);
    }

    T Dequeue() {
        if (tree.isEmpty()) 
        {
            throw std::runtime_error("Priority queue is empty");
        }

        T result;
        bool first = true;
        
        tree.traverseInOrder([&](const T& value) 
        {
            if (first || comparator(result, value)) 
            {
                result = value;
                first = false;
            }
        });

        tree.remove(result);
        return result;
    }

    T Peek() const {
        if (tree.isEmpty()) 
        {
            throw std::runtime_error("Priority queue is empty");
        }

        T result;
        bool first = true;
        
        tree.traverseInOrder([&](const T& value) 
        {
            if (first || comparator(result, value)) 
            {
                result = value;
                first = false;
            }
        });

        return result;
    }

    bool IsEmpty() const 
    {
        return tree.isEmpty();
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

    void Clear() 
    {
        tree.clear();
    }
};

#endif