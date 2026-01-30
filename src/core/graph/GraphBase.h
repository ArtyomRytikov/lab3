#ifndef GRAPH_BASE_H
#define GRAPH_BASE_H

#include "../structures/Dictionary.h"
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include "../structures/LinkedList.h"
#include <stdexcept>
#include <iostream>

template<typename T>
class GraphBase 
{
protected:
    bool directed;
    int vertexCount;
    int edgeCount;

public:
    GraphBase(bool isDirected = false) : directed(isDirected), vertexCount(0), edgeCount(0) {}
    virtual ~GraphBase() = default;

    // Основные операции
    virtual void AddVertex(T vertex) = 0;
    virtual void AddEdge(T from, T to, double weight = 1.0) = 0;
    virtual void RemoveVertex(T vertex) = 0;
    virtual void RemoveEdge(T from, T to) = 0;
    virtual bool HasVertex(T vertex) const = 0;
    virtual bool HasEdge(T from, T to) const = 0;
    virtual double GetEdgeWeight(T from, T to) const = 0;
    virtual void SetEdgeWeight(T from, T to, double weight) = 0;
    
    // Получение информации о графе
    virtual Sequence<T>* GetAdjacentVertices(T vertex) const = 0;
    virtual Sequence<T>* GetAllVertices() const = 0;
    virtual int GetVertexCount() const { return vertexCount; }
    virtual int GetEdgeCount() const { return edgeCount; }
    virtual bool IsDirected() const { return directed; }

    virtual void Clear() = 0;
    
    // Сериализация в строку (для вывода)
    virtual void SerializeToStream(std::ostream& os) const = 0;
    
    // Вывод графа
    void Print(std::ostream& os = std::cout) const 
    {
        if (directed) os << "Directed Graph: ";
        else os << "Undirected Graph: ";
        os << vertexCount << " vertices, " << edgeCount << " edges\n";
        
        auto vertices = GetAllVertices();
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T v = vertices->Get(i);
            auto adj = GetAdjacentVertices(v);
            os << v << ": ";
            for (int j = 0; j < adj->GetLength(); j++) 
            {
                T to = adj->Get(j);
                double weight = GetEdgeWeight(v, to);
                os << to;
                if (weight != 1.0) os << "(" << weight << ")";
                if (j < adj->GetLength() - 1) os << ", ";
            }
            os << "\n";
            delete adj;
        }
        delete vertices;
    }
};


template<typename T>
class Stack 
{
private:
    LinkedList<T> list;
    
public:
    void Push(const T& value) 
    {
        list.Prepend(value);
    }
    
    T Pop() 
    {
        if (list.GetLength() == 0) throw std::runtime_error("Stack is empty");
        T value = list.GetFirst();

        LinkedList<T> newList;
        for (int i = 1; i < list.GetLength(); i++) 
        {
            newList.Append(list.Get(i));
        }
        list = newList;
        return value;
    }
    
    T Top() const 
    {
        if (list.GetLength() == 0) throw std::runtime_error("Stack is empty");
        return list.GetFirst();
    }
    
    bool IsEmpty() const { return list.GetLength() == 0; }
    int Size() const { return list.GetLength(); }
};

template<typename T>
class Queue 
{
private:
    LinkedList<T> list;
    
public:
    void Enqueue(const T& value) 
    {
        list.Append(value);
    }
    
    T Dequeue() 
    {
        if (list.GetLength() == 0) throw std::runtime_error("Queue is empty");
        T value = list.GetFirst();
        LinkedList<T> newList;
        for (int i = 1; i < list.GetLength(); i++) {
            newList.Append(list.Get(i));
        }
        list = newList;
        return value;
    }
    
    T Front() const 
    {
        if (list.GetLength() == 0) throw std::runtime_error("Queue is empty");
        return list.GetFirst();
    }
    
    bool IsEmpty() const { return list.GetLength() == 0; }
    int Size() const { return list.GetLength(); }
};

#endif // GRAPH_BASE_H