#ifndef TOPOLOGICAL_SORT_H
#define TOPOLOGICAL_SORT_H

#include "../graph/DirectedGraph.h"
#include "../graph/GraphBase.h" 
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include <stdexcept>

template<typename T>
class TopologicalSort 
{
public:
    // Метод для топологической сортировки
    static Sequence<T>* KahnSort(const DirectedGraph<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) 
        {
            return new MutableArraySequence<T>();
        }
        
        Dictionary<T, int> inDegree;
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            inDegree.Add(vertex, 0);
        }
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T from = vertices->Get(i);
            auto adj = graph.GetAdjacentVertices(from);
            
            for (int j = 0; j < adj->GetLength(); j++) 
            {
                T to = adj->Get(j);
                int currentDegree = inDegree.Get(to);
                inDegree.Add(to, currentDegree + 1);
            }
            
            delete adj;
        }
        
        delete vertices;
        
        Queue<T> queue;
        inDegree.ForEachKey([&queue, &inDegree](const T& vertex) 
        {
            if (inDegree.Get(vertex) == 0) 
            {
                queue.Enqueue(vertex);
            }
        });
        
        // 3. Алгоритм Кана
        auto* result = new MutableArraySequence<T>();
        int visitedCount = 0;
        
        while (!queue.IsEmpty()) 
        {
            T current = queue.Dequeue();
            result->Append(current);
            visitedCount++;
            
            auto adj = graph.GetAdjacentVertices(current);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T neighbor = adj->Get(i);
                int newDegree = inDegree.Get(neighbor) - 1;
                inDegree.Add(neighbor, newDegree);
                
                if (newDegree == 0) 
                {
                    queue.Enqueue(neighbor);
                }
            }
            
            delete adj;
        }
        
        if (visitedCount != graph.GetVertexCount()) 
        {
            delete result;
            throw std::runtime_error("Graph contains a cycle - topological sort not possible");
        }
        
        return result;
    }
    
    // Метод для топологической сортировки
    static Sequence<T>* DFSSort(const DirectedGraph<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) {
            return new MutableArraySequence<T>();
        }
        
        Set<T> visited;
        Set<T> tempMark;
        Stack<T> resultStack;
        auto vertices = graph.GetAllVertices();
        
        std::function<void(T)> dfs = [&](T vertex) 
        {
            if (tempMark.Contains(vertex))
            {
                throw std::runtime_error("Graph contains a cycle - topological sort not possible");
            }
            
            if (visited.Contains(vertex)) return;
            
            tempMark.Add(vertex);
            
            auto adj = graph.GetAdjacentVertices(vertex);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                dfs(adj->Get(i));
            }
            delete adj;
            
            tempMark.Remove(vertex);
            visited.Add(vertex);
            resultStack.Push(vertex);
        };
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            if (!visited.Contains(vertex)) 
            {
                dfs(vertex);
            }
        }
        
        delete vertices;
        
        auto* result = new MutableArraySequence<T>();
        while (!resultStack.IsEmpty()) 
        {
            result->Append(resultStack.Pop());
        }
        
        return result;
    }
    
    // Проверка, является ли граф ациклическим
    static bool IsAcyclic(const DirectedGraph<T>& graph) 
    {
        try 
        {
            auto result = KahnSort(graph);
            delete result;
            return true;
        } 
        catch (const std::runtime_error&) 
        {
            return false;
        }
    }
    
    // Нахождение всех источников
    static Sequence<T>* FindSources(const DirectedGraph<T>& graph) 
    {
        auto* result = new MutableArraySequence<T>();
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            bool isSource = true;
            for (int j = 0; j < vertices->GetLength(); j++) 
            {
                T other = vertices->Get(j);
                if (other != vertex && graph.HasEdge(other, vertex)) 
                {
                    isSource = false;
                    break;
                }
            }
            
            if (isSource) 
            {
                result->Append(vertex);
            }
        }
        
        delete vertices;
        return result;
    }
    
    // Нахождение всех стоков
    static Sequence<T>* FindSinks(const DirectedGraph<T>& graph) 
    {
        auto* result = new MutableArraySequence<T>();
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            auto adj = graph.GetAdjacentVertices(vertex);
            bool isSink = (adj->GetLength() == 0);
            delete adj;
            
            if (isSink) 
            {
                result->Append(vertex);
            }
        }
        
        delete vertices;
        return result;
    }
};

#endif // TOPOLOGICAL_SORT_H