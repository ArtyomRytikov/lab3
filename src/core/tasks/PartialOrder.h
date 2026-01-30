#ifndef PARTIAL_ORDER_H
#define PARTIAL_ORDER_H

#include "../graph/DirectedGraph.h"
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include <stdexcept>
#include "TopologicalSort.h"


template<typename T>
class PartialOrder 
{
private:
    // Функция для проверки антисимметричности
    static bool CheckAntisymmetry(const DirectedGraph<T>& graph)
    {
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T u = vertices->Get(i);
            auto adj = graph.GetAdjacentVertices(u);
            
            for (int j = 0; j < adj->GetLength(); j++) 
            {
                T v = adj->Get(j);
                if (graph.HasEdge(v, u)) 
                {
                    delete adj;
                    delete vertices;
                    return false;
                }
            }
            
            delete adj;
        }
        
        delete vertices;
        return true;
    }
    
    // Функция для нахождения достижимых вершин из start
    static Set<T>* GetReachableVertices(const DirectedGraph<T>& graph, T start) 
    {
        auto* reachable = new Set<T>();
        if (!graph.HasVertex(start)) 
        {
            return reachable;
        }
        
        Queue<T> queue;
        queue.Enqueue(start);
        reachable->Add(start);
        
        while (!queue.IsEmpty()) 
        {
            T current = queue.Dequeue();
            
            auto adj = graph.GetAdjacentVertices(current);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T neighbor = adj->Get(i);
                if (!reachable->Contains(neighbor)) 
                {
                    reachable->Add(neighbor);
                    queue.Enqueue(neighbor);
                }
            }
            delete adj;
        }
        
        return reachable;
    }
    
    static bool IsLessOrEqual(const DirectedGraph<T>& partialOrder, T a, T b) 
    {
        if (a == b) return true;
        
        auto reachable = GetReachableVertices(partialOrder, a);
        bool result = reachable->Contains(b);
        delete reachable;
        return result;
    }
    
    // Находит максимальные элементы в подмножестве
    static Sequence<T>* FindMaximalElementsInSubset(const DirectedGraph<T>& partialOrder, Sequence<T>* subset) 
    {
        auto* result = new MutableArraySequence<T>();
    
        for (int i = 0; i < subset->GetLength(); i++) 
        {
            T candidate = subset->Get(i);
            bool isMaximal = true;
            
            for (int j = 0; j < subset->GetLength(); j++) 
            {
                T other = subset->Get(j);
                if (candidate != other) 
                {
                    if (IsLessOrEqual(partialOrder, candidate, other) && !IsLessOrEqual(partialOrder, other, candidate)) 
                    {
                        isMaximal = false;
                        break;
                    }
                }
            }
            
            if (isMaximal) 
            {
                bool alreadyAdded = false;
                for (int k = 0; k < result->GetLength(); k++) 
                {
                    if (IsLessOrEqual(partialOrder, result->Get(k), candidate) && IsLessOrEqual(partialOrder, candidate, result->Get(k))) 
                    {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) 
                {
                    result->Append(candidate);
                }
            }
        }
        
        return result;
    }
    
    // Находит минимальные элементы в подмножестве
    static Sequence<T>* FindMinimalElementsInSubset(const DirectedGraph<T>& partialOrder, Sequence<T>* subset) 
    {
        auto* result = new MutableArraySequence<T>();

        for (int i = 0; i < subset->GetLength(); i++) 
        {
            T candidate = subset->Get(i);
            bool isMinimal = true;
            
            for (int j = 0; j < subset->GetLength(); j++) 
            {
                T other = subset->Get(j);
                if (candidate != other) 
                {

                    if (IsLessOrEqual(partialOrder, other, candidate) && !IsLessOrEqual(partialOrder, candidate, other)) 
                    {
                        isMinimal = false;
                        break;
                    }
                }
            }
            
            if (isMinimal) 
            {
                bool alreadyAdded = false;
                for (int k = 0; k < result->GetLength(); k++) 
                {
                    if (IsLessOrEqual(partialOrder, result->Get(k), candidate) && IsLessOrEqual(partialOrder, candidate, result->Get(k))) 
                    {
                        alreadyAdded = true;ы
                        break;
                    }
                }
                if (!alreadyAdded) 
            {
                    result->Append(candidate);
                }
            }
        }
        return result;
    }

public:
    // Проверка, является ли граф отношением частичного порядка
    static bool IsPartialOrder(const DirectedGraph<T>& graph) 
    {      
        // Проверка антисимметричности
        if (!CheckAntisymmetry(graph)) 
        {
            return false; 
        }
        
        // Проверка ацикличности (частичный порядок должен быть ациклическим)
        try 
        {
            auto topological = TopologicalSort<T>::KahnSort(graph);
            delete topological;
            return true;
        } 
        catch (const std::runtime_error&) 
        {
            return false;
        }
    }
    
    // Построение диаграммы Хассе из отношения частичного порядка
    static DirectedGraph<T>* BuildHasseDiagram(const DirectedGraph<T>& partialOrder) 
    {
        if (!IsPartialOrder(partialOrder)) 
        {
            throw std::runtime_error("Graph is not a partial order");
        }
    
        auto* hasse = new DirectedGraph<T>();
        auto vertices = partialOrder.GetAllVertices();
    
        // Копируем все вершины
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            hasse->AddVertex(vertices->Get(i));
        }
    
        // Для каждой пары вершин проверяем, есть ли прямое покрытие
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            for (int j = 0; j < vertices->GetLength(); j++) 
            {
                if (i == j) continue;
            
                T u = vertices->Get(i);
                T v = vertices->Get(j);
            
                if (partialOrder.HasEdge(u, v)) 
                {
                    bool isCovering = true;
                
                    for (int k = 0; k < vertices->GetLength(); k++) 
                    {
                        if (k == i || k == j) continue;
                    
                        T w = vertices->Get(k);
                    
                        if (partialOrder.HasEdge(u, w) && partialOrder.HasEdge(w, v)) 
                        {
                            isCovering = false;
                            break;
                        }
                    }
                
                    if (isCovering) 
                    {
                        hasse->AddEdge(u, v);
                    }
                }
            }
        }
    
        delete vertices;
        return hasse;
    }
    
    // Нахождение минимальных элементов
    static Sequence<T>* FindMinimalElements(const DirectedGraph<T>& partialOrder) 
    {
        if (!IsPartialOrder(partialOrder)) 
        {
            throw std::runtime_error("Graph is not a partial order");
        }
        
        auto* minimal = new MutableArraySequence<T>();
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            bool isMinimal = true;
            
            for (int j = 0; j < vertices->GetLength(); j++) 
            {
                T other = vertices->Get(j);
                if (other != vertex && partialOrder.HasEdge(other, vertex)) 
                {
                    isMinimal = false;
                    break;
                }
            }
            
            if (isMinimal) 
            {
                minimal->Append(vertex);
            }
        }
        
        delete vertices;
        return minimal;
    }
    
    // Нахождение максимальных элементов
    static Sequence<T>* FindMaximalElements(const DirectedGraph<T>& partialOrder) 
    {
        if (!IsPartialOrder(partialOrder)) 
        {
            throw std::runtime_error("Graph is not a partial order");
        }
        
        auto* maximal = new MutableArraySequence<T>();
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            bool isMaximal = true;
            
            for (int j = 0; j < vertices->GetLength(); j++) 
            {
                T other = vertices->Get(j);
                if (other != vertex && partialOrder.HasEdge(vertex, other)) 
                {
                    isMaximal = false;
                    break;
                }
            }
            
            if (isMaximal) 
            {
                maximal->Append(vertex);
            }
        }
        
        delete vertices;
        return maximal;
    }
    
    // Нахождение наименьшего элемента
    static T FindLeastElement(const DirectedGraph<T>& partialOrder) 
     {
        auto minimal = FindMinimalElements(partialOrder);
        
        if (minimal->GetLength() != 1) {
            delete minimal;
            throw std::runtime_error("No unique least element");
        }
        
        T result = minimal->Get(0);
        delete minimal;
        return result;
    }
    
    // Нахождение наибольшего элемента
    static T FindGreatestElement(const DirectedGraph<T>& partialOrder) 
    {
        auto maximal = FindMaximalElements(partialOrder);
        
        if (maximal->GetLength() != 1) 
        {
            delete maximal;
            throw std::runtime_error("No unique greatest element");
        }
        
        T result = maximal->Get(0);
        delete maximal;
        return result;
    }
    
    // Проверка, является ли элемент минимальным
    static bool IsMinimalElement(const DirectedGraph<T>& partialOrder, T element) 
    {
        if (!partialOrder.HasVertex(element)) {
            throw std::runtime_error("Element does not exist");
        }
        
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) {
            T other = vertices->Get(i);
            if (other != element && partialOrder.HasEdge(other, element))
            {
                delete vertices;
                return false;
            }
        }
        
        delete vertices;
        return true;
    }
    
    // Проверка, является ли элемент максимальным
    static bool IsMaximalElement(const DirectedGraph<T>& partialOrder, T element) 
    {
        if (!partialOrder.HasVertex(element)) 
        {
            throw std::runtime_error("Element does not exist");
        }
        
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) {
            T other = vertices->Get(i);
            if (other != element && partialOrder.HasEdge(element, other)) 
            {
                delete vertices;
                return false;
            }
        }
        
        delete vertices;
        return true;
    }
    
    // Нахождение нижней грани двух элементов
    static Sequence<T>* FindInfimum(const DirectedGraph<T>& partialOrder, T a, T b) 
    {
        if (!partialOrder.HasVertex(a) || !partialOrder.HasVertex(b)) 
        {
            throw std::runtime_error("Elements do not exist");
        }
        
        if (a == b) 
        {
            auto* result = new MutableArraySequence<T>();
            result->Append(a);
            return result;
        }
        
        // Находим все общие нижние грани
        auto* allLowerBounds = new MutableArraySequence<T>();
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T candidate = vertices->Get(i);
            
            if (IsLessOrEqual(partialOrder, candidate, a) && IsLessOrEqual(partialOrder, candidate, b)) 
            {
                allLowerBounds->Append(candidate);
            }
        }
        
        delete vertices;
        
        if (allLowerBounds->GetLength() == 0) 
        {
            return allLowerBounds;
        }
        
        auto* result = FindMaximalElementsInSubset(partialOrder, allLowerBounds);
        delete allLowerBounds;
        
        return result;
    }
    
    // Нахождение верхней грани двух элементов
    static Sequence<T>* FindSupremum(const DirectedGraph<T>& partialOrder, T a, T b)
    {
        if (!partialOrder.HasVertex(a) || !partialOrder.HasVertex(b)) 
        {
            throw std::runtime_error("Elements do not exist");
        }
        
        if (a == b) 
        {
            auto* result = new MutableArraySequence<T>();
            result->Append(a);
            return result;
        }
        
        // Находим все общие верхние грани
        auto* allUpperBounds = new MutableArraySequence<T>();
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T candidate = vertices->Get(i);
            
            if (IsLessOrEqual(partialOrder, a, candidate) && IsLessOrEqual(partialOrder, b, candidate)) 
            {
                allUpperBounds->Append(candidate);
            }
        }
        
        delete vertices;
        
        if (allUpperBounds->GetLength() == 0) 
        {
            return allUpperBounds;
        }
        
        auto* result = FindMinimalElementsInSubset(partialOrder, allUpperBounds);
        delete allUpperBounds;
        
        return result;
    }
    
    // Проверка, является ли решёткой
    static bool IsLattice(const DirectedGraph<T>& partialOrder) 
    {
        if (!IsPartialOrder(partialOrder)) {
            return false;
        }
        
        auto vertices = partialOrder.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) {
            for (int j = i + 1; j < vertices->GetLength(); j++) 
            {
                T a = vertices->Get(i);
                T b = vertices->Get(j);
                
                auto inf = FindInfimum(partialOrder, a, b);
                auto sup = FindSupremum(partialOrder, a, b);
                
                bool hasSingleInf = (inf->GetLength() == 1);
                bool hasSingleSup = (sup->GetLength() == 1);
                
                delete inf;
                delete sup;
                
                if (!hasSingleInf || !hasSingleSup) 
                {
                    delete vertices;
                    return false;
                }
            }
        }
        
        delete vertices;
        return true;
    }
    
    // Получение уровней частично упорядоченного множества
    static Sequence<Sequence<T>*>* GetLevels(const DirectedGraph<T>& partialOrder) 
    {
        if (!IsPartialOrder(partialOrder)) 
        {
            throw std::runtime_error("Graph is not a partial order");
        }
        
        auto* levels = new MutableArraySequence<Sequence<T>*>();
        auto vertices = partialOrder.GetAllVertices();
        
        // Сначала находим минимальные элементы
        auto minimal = FindMinimalElements(partialOrder);
        levels->Append(minimal);
        
        // Создаём множество уже размещённых вершин
        Set<T> placedVertices;
        for (int i = 0; i < minimal->GetLength(); i++) 
        {
            placedVertices.Add(minimal->Get(i));
        }
        
        while (placedVertices.GetCount() < partialOrder.GetVertexCount()) 
        {
            auto* nextLevel = new MutableArraySequence<T>();
            
            // Ищем вершины, все предшественники которых уже размещены
            for (int i = 0; i < vertices->GetLength(); i++) 
            {
                T vertex = vertices->Get(i);
                
                if (placedVertices.Contains(vertex)) continue;
                
                // Проверяем, все ли предшественники размещены
                bool allPredecessorsPlaced = true;
                auto reachable = GetReachableVertices(partialOrder, vertex);
                
                // Находим прямых предшественников
                for (int j = 0; j < vertices->GetLength(); j++) 
                {
                    T other = vertices->Get(j);
                    if (other != vertex && partialOrder.HasEdge(other, vertex)) 
                    {
                        if (!placedVertices.Contains(other)) 
                        {
                            allPredecessorsPlaced = false;
                            break;
                        }
                    }
                }
                
                delete reachable;
                
                if (allPredecessorsPlaced)
                {
                    nextLevel->Append(vertex);
                }
            }
            
            // Добавляем вершины этого уровня в множество размещённых
            for (int i = 0; i < nextLevel->GetLength(); i++) 
            {
                placedVertices.Add(nextLevel->Get(i));
            }
            
            levels->Append(nextLevel);
        }
        
        delete vertices;
        return levels;
    }
};

#endif // PARTIAL_ORDER_H