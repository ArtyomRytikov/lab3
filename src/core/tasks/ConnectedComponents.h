#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include "../graph/UndirectedGraph.h"
#include "../graph/GraphBase.h"
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include <stdexcept>

template<typename T>
class ConnectedComponents 
{
private:
    // Функция BFS для поиска компоненты связности
    static void BFSComponent(const UndirectedGraph<T>& graph, T startVertex, Set<T>& visited, MutableArraySequence<T>* component) 
    {
        Queue<T> queue;
        queue.Enqueue(startVertex);
        visited.Add(startVertex);
        
        while (!queue.IsEmpty()) 
        {
            T current = queue.Dequeue();
            component->Append(current);
            
            auto adj = graph.GetAdjacentVertices(current);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T neighbor = adj->Get(i);
                if (!visited.Contains(neighbor)) 
                {
                    visited.Add(neighbor);
                    queue.Enqueue(neighbor);
                }
            }
            delete adj;
        }
    }
    
    // Функция DFS для поиска компоненты связности
    static void DFSComponent(const UndirectedGraph<T>& graph, T vertex, Set<T>& visited, MutableArraySequence<T>* component) 
    {
        Stack<T> stack;
        stack.Push(vertex);
        
        while (!stack.IsEmpty()) 
        {
            T current = stack.Pop();
            
            if (visited.Contains(current)) continue;
            
            visited.Add(current);
            component->Append(current);
            
            auto adj = graph.GetAdjacentVertices(current);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T neighbor = adj->Get(i);
                if (!visited.Contains(neighbor)) 
                {
                    stack.Push(neighbor);
                }
            }
            delete adj;
        }
    }

public:
    // Нахождение всех компонент связности
    static Sequence<Sequence<T>*>* FindComponentsBFS(const UndirectedGraph<T>& graph) 
    {
        auto* components = new MutableArraySequence<Sequence<T>*>();
        Set<T> visited;
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            
            if (!visited.Contains(vertex)) 
            {
                auto* component = new MutableArraySequence<T>();
                BFSComponent(graph, vertex, visited, component);
                components->Append(component);
            }
        }
        
        delete vertices;
        return components;
    }
    
    // Нахождение всех компонент связности 
    static Sequence<Sequence<T>*>* FindComponentsDFS(const UndirectedGraph<T>& graph) 
    {
        auto* components = new MutableArraySequence<Sequence<T>*>();
        Set<T> visited;
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            
            if (!visited.Contains(vertex)) 
            {
                auto* component = new MutableArraySequence<T>();
                DFSComponent(graph, vertex, visited, component);
                components->Append(component);
            }
        }
        
        delete vertices;
        return components;
    }
    
    // Проверка, является ли граф связным
    static bool IsConnected(const UndirectedGraph<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) return true;
        
        auto components = FindComponentsBFS(graph);
        bool isConnected = (components->GetLength() == 1);
        
        for (int i = 0; i < components->GetLength(); i++) 
        {
            delete components->Get(i);
        }
        delete components;
        
        return isConnected;
    }
    
    // Нахождение компоненты связности для конкретной вершины
    static Sequence<T>* GetComponentForVertex(const UndirectedGraph<T>& graph, T vertex) 
    {
        if (!graph.HasVertex(vertex)) {
            throw std::runtime_error("Vertex does not exist in graph");
        }
        
        auto* component = new MutableArraySequence<T>();
        Set<T> visited;
        BFSComponent(graph, vertex, visited, component);
        
        return component;
    }
    
    // Нахождение размера компоненты связности
    static int GetComponentSize(const UndirectedGraph<T>& graph, T vertex) 
    {
        auto component = GetComponentForVertex(graph, vertex);
        int size = component->GetLength();
        delete component;
        return size;
    }
    
    // Подсчёт количества компонент связности
    static int CountComponents(const UndirectedGraph<T>& graph) 
    {
        auto components = FindComponentsBFS(graph);
        int count = components->GetLength();
        
        for (int i = 0; i < components->GetLength(); i++)
        {
            delete components->Get(i);
        }
        delete components;
        
        return count;
    }
    
    // Нахождение самой большой компоненты связности
    static Sequence<T>* FindLargestComponent(const UndirectedGraph<T>& graph) 
    {
        auto components = FindComponentsBFS(graph);
        
        if (components->GetLength() == 0) {
            delete components;
            return new MutableArraySequence<T>();
        }
        
        Sequence<T>* largest = nullptr;
        int maxSize = -1;
        
        for (int i = 0; i < components->GetLength(); i++) 
        {
            auto component = components->Get(i);
            if (component->GetLength() > maxSize) 
            {
                maxSize = component->GetLength();
                largest = component;
            }
        }
        
        auto* result = new MutableArraySequence<T>();
        for (int i = 0; i < largest->GetLength(); i++) 
        {
            result->Append(largest->Get(i));
        }
        
        for (int i = 0; i < components->GetLength(); i++)
        {
            delete components->Get(i);
        }
        delete components;
        
        return result;
    }
    
    // Нахождение самой маленькой компоненты связности
    static Sequence<T>* FindSmallestComponent(const UndirectedGraph<T>& graph) 
    {
        auto components = FindComponentsBFS(graph);
        
        if (components->GetLength() == 0) {
            delete components;
            return new MutableArraySequence<T>();
        }
        
        Sequence<T>* smallest = nullptr;
        int minSize = std::numeric_limits<int>::max();
        
        for (int i = 0; i < components->GetLength(); i++) 
        {
            auto component = components->Get(i);
            if (component->GetLength() < minSize) 
            {
                minSize = component->GetLength();
                smallest = component;
            }
        }
        
        auto* result = new MutableArraySequence<T>();
        for (int i = 0; i < smallest->GetLength(); i++) 
        {
            result->Append(smallest->Get(i));
        }
        
        for (int i = 0; i < components->GetLength(); i++) 
        {
            delete components->Get(i);
        }
        delete components;
        
        return result;
    }
    
    // Проверка, находятся ли две вершины в одной компоненте связности
    static bool AreConnected(const UndirectedGraph<T>& graph, T vertex1, T vertex2) 
    {
        if (!graph.HasVertex(vertex1) || !graph.HasVertex(vertex2)) 
        {
            return false;
        }
        
        auto component = GetComponentForVertex(graph, vertex1);
        bool connected = false;
        
        for (int i = 0; i < component->GetLength(); i++) 
        {
            if (component->Get(i) == vertex2) {
                connected = true;
                break;
            }
        }
        
        delete component;
        return connected;
    }
};

#endif // CONNECTED_COMPONENTS_H