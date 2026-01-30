#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "../graph/GraphBase.h"
#include "../graph/DirectedGraph.h"
#include "../graph/UndirectedGraph.h"
#include "../structures/priority_queue.h"
#include "../structures/Dictionary.h"
#include "../structures/Sequence.h"
#include "../structures/Set.h"
#include <limits>
#include <stdexcept>

template<typename T>
class ShortestPath 
{
private:
    // Структура для хранения информации о вершине в алгоритме Дейкстры
    struct DijkstraNode 
    {
        T vertex;
        double distance;
        
        DijkstraNode() : vertex(T()), distance(0.0) {}
        DijkstraNode(T v, double d) : vertex(v), distance(d) {}
        
        bool operator<(const DijkstraNode& other) const 
        {
            return distance > other.distance;
        }
        
        bool operator>(const DijkstraNode& other) const 
        {
            return distance < other.distance;
        }
    };

public:
    // Алгоритм Дейкстры для взвешенного графа
    static Dictionary<T, double>* Dijkstra(const GraphBase<T>& graph, T startVertex) 
    {
        if (!graph.HasVertex(startVertex)) 
        {
            throw std::runtime_error("Start vertex does not exist");
        }
    
        auto* distances = new Dictionary<T, double>();
        auto vertices = graph.GetAllVertices();
    
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            distances->Add(vertex, std::numeric_limits<double>::infinity());
        }
        distances->Add(startVertex, 0.0);
    
        Set<T> visited;
        int totalVertices = graph.GetVertexCount();
        
        for (int count = 0; count < totalVertices; count++) 
        {
            // Находим вершину с минимальным расстоянием среди непосещённых
            T u;
            double minDist = std::numeric_limits<double>::infinity();
            bool found = false;
            
            auto allVerts = graph.GetAllVertices();
            for (int i = 0; i < allVerts->GetLength(); i++) 
            {
                T vertex = allVerts->Get(i);
                if (!visited.Contains(vertex)) 
                {
                    double dist = distances->Get(vertex);
                    if (dist < minDist) 
                    {
                        minDist = dist;
                        u = vertex;
                        found = true;
                    }
                }
            }
            delete allVerts;
            
            if (!found || minDist == std::numeric_limits<double>::infinity()) 
            {
                break;
            }
            
            visited.Add(u);
            
            // Обновляем расстояния до соседей
            auto adj = graph.GetAdjacentVertices(u);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T v = adj->Get(i);
                if (!visited.Contains(v)) 
                {
                    double edgeWeight = graph.GetEdgeWeight(u, v);
                    double alt = distances->Get(u) + edgeWeight;
                    if (alt < distances->Get(v)) 
                    {
                        distances->Add(v, alt);
                    }
                }
            }
            delete adj;
        }
    
        delete vertices;
        return distances;
    }
    
    // Алгоритм Дейкстры с восстановлением пути
    static Sequence<T>* DijkstraWithPath(const GraphBase<T>& graph, T startVertex, T endVertex) 
    {
        if (!graph.HasVertex(startVertex) || !graph.HasVertex(endVertex)) {
            throw std::runtime_error("Vertices do not exist");
        }
    
        auto* distances = new Dictionary<T, double>();
        auto* previous = new Dictionary<T, T>();
        auto vertices = graph.GetAllVertices();
    
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            distances->Add(vertex, std::numeric_limits<double>::infinity());
        }
        distances->Add(startVertex, 0.0);
    
        Set<T> visited;
        int totalVertices = graph.GetVertexCount();
        
        for (int count = 0; count < totalVertices; count++) 
        {
            // Находим вершину с минимальным расстоянием среди непосещённых
            T u;
            double minDist = std::numeric_limits<double>::infinity();
            bool found = false;
            
            auto allVerts = graph.GetAllVertices();
            for (int i = 0; i < allVerts->GetLength(); i++) 
            {
                T vertex = allVerts->Get(i);
                if (!visited.Contains(vertex)) 
                {
                    double dist = distances->Get(vertex);
                    if (dist < minDist) 
                    {
                        minDist = dist;
                        u = vertex;
                        found = true;
                    }
                }
            }
            delete allVerts;
            
            if (!found || minDist == std::numeric_limits<double>::infinity()) 
            {
                break;
            }
            
            if (u == endVertex) 
            {
                break;
            }
            
            visited.Add(u);
            
            // Обновляем расстояния до соседей
            auto adj = graph.GetAdjacentVertices(u);
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T v = adj->Get(i);
                if (!visited.Contains(v)) 
                {
                    double edgeWeight = graph.GetEdgeWeight(u, v);
                    double alt = distances->Get(u) + edgeWeight;
                    if (alt < distances->Get(v)) 
                    {
                        distances->Add(v, alt);
                        previous->Add(v, u);
                    }
                }
            }
            delete adj;
        }
    
        // Восстанавливаем путь
        auto* path = new MutableArraySequence<T>();
        
        // Если конечная вершина недостижима
        if (distances->Get(endVertex) == std::numeric_limits<double>::infinity()) 
        {
            delete distances;
            delete previous;
            delete vertices;
            return path;
        }
    
        // Восстанавливаем путь от конца к началу
        T current = endVertex;
        while (true) {
            path->Prepend(current);
            if (current == startVertex) break;
            if (!previous->ContainsKey(current)) 
            {
                delete path;
                delete distances;
                delete previous;
                delete vertices;
                throw std::runtime_error("Cannot reconstruct path");
            }
            current = previous->Get(current);
        }
    
        delete distances;
        delete previous;
        delete vertices;
        return path;
    }
    
    // Алгоритм Беллмана-Форда для графов с отрицательными весами
    static Dictionary<T, double>* BellmanFord(const DirectedGraph<T>& graph, T startVertex) 
    {
        if (!graph.HasVertex(startVertex)) 
        {
            throw std::runtime_error("Start vertex does not exist");
        }
        
        auto* distances = new Dictionary<T, double>();
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            distances->Add(vertex, std::numeric_limits<double>::infinity());
        }
        distances->Add(startVertex, 0.0);
        
        for (int i = 0; i < graph.GetVertexCount() - 1; i++) 
        {
            bool changed = false;
            
            for (int j = 0; j < vertices->GetLength(); j++) 
            {
                T from = vertices->Get(j);
                auto adj = graph.GetAdjacentVertices(from);
                
                for (int k = 0; k < adj->GetLength(); k++) 
                {
                    T to = adj->Get(k);
                    double weight = graph.GetEdgeWeight(from, to);
                    double newDist = distances->Get(from) + weight;
                    
                    if (distances->Get(from) < std::numeric_limits<double>::infinity() && 
                        newDist < distances->Get(to)) 
                        {
                        distances->Add(to, newDist);
                        changed = true;
                    }
                }
                
                delete adj;
            }
            
            if (!changed) break; 
        }
        
        // Проверка на отрицательные циклы
        for (int j = 0; j < vertices->GetLength(); j++) 
        {
            T from = vertices->Get(j);
            auto adj = graph.GetAdjacentVertices(from);
            
            for (int k = 0; k < adj->GetLength(); k++) 
            {
                T to = adj->Get(k);
                double weight = graph.GetEdgeWeight(from, to);
                
                if (distances->Get(from) < std::numeric_limits<double>::infinity() && distances->Get(from) + weight < distances->Get(to)) 
                {
                    delete vertices;
                    delete distances;
                    delete adj;
                    throw std::runtime_error("Graph contains negative cycle");
                }
            }
            
            delete adj;
        }
        
        delete vertices;
        return distances;
    }
    
    // BFS для ненагруженного графа
    static Dictionary<T, double>* BFSShortestPath(const GraphBase<T>& graph, T startVertex) 
    {
        if (!graph.HasVertex(startVertex)) 
        {
            throw std::runtime_error("Start vertex does not exist");
        }
    
        auto* distances = new Dictionary<T, double>();
        auto vertices = graph.GetAllVertices();
    
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            distances->Add(vertex, std::numeric_limits<double>::infinity());
        }
        distances->Add(startVertex, 0.0);
    
        Queue<T> queue;
        queue.Enqueue(startVertex);
    
        while (!queue.IsEmpty()) 
        {
            T current = queue.Dequeue();
            double currentDist = distances->Get(current);
        
            auto adj = graph.GetAdjacentVertices(current);
        
            for (int i = 0; i < adj->GetLength(); i++) 
            {
                T neighbor = adj->Get(i);
            
                // Если вершина ещё не посещена
                if (distances->Get(neighbor) == std::numeric_limits<double>::infinity()) 
                {
                    distances->Add(neighbor, currentDist + 1.0);
                    queue.Enqueue(neighbor);
                }
            }
        
            delete adj;
        }
    
        delete vertices;
        return distances;
    }
    
    static Sequence<T>* ReconstructPath(const GraphBase<T>& graph, T startVertex, T endVertex,const Dictionary<T, double>& distances) 
    {
        if (!graph.HasVertex(startVertex) || !graph.HasVertex(endVertex)) 
        {
            throw std::runtime_error("Vertices do not exist");
        }
    
        // Проверяем, достижима ли конечная вершина
        double endDist = distances.Get(endVertex);
        if (endDist == std::numeric_limits<double>::infinity() || endDist < 0) 
        {
            return new MutableArraySequence<T>();
        }
    
        auto* path = new MutableArraySequence<T>();
        T current = endVertex;
    
        // Добавляем конечную вершину
        path->Prepend(current);
    
        while (current != startVertex) 
        {
            bool foundPrev = false;
            
            // Получаем все вершины графа
            auto allVertices = graph.GetAllVertices();
            
            // Ищем предыдущую вершину на кратчайшем пути
            for (int i = 0; i < allVertices->GetLength(); i++) 
            {
                T candidate = allVertices->Get(i);
                
                if (graph.HasEdge(candidate, current)) 
                {
                    double edgeWeight = graph.GetEdgeWeight(candidate, current);
                    double candidateDist = distances.Get(candidate);
                    
                    if (candidateDist + edgeWeight == distances.Get(current)) 
                    {
                        path->Prepend(candidate);
                        current = candidate;
                        foundPrev = true;
                        break;
                    }
                }
            }
            
            delete allVertices;
            
            // Если не нашли среди всех вершин, пробуем среди соседей
            if (!foundPrev) 
            {
                auto adj = graph.GetAdjacentVertices(current);
                for (int i = 0; i < adj->GetLength(); i++) {
                    T neighbor = adj->Get(i);
                    double edgeWeight = graph.GetEdgeWeight(neighbor, current);
                    double neighborDist = distances.Get(neighbor);
                    
                    if (neighborDist + edgeWeight == distances.Get(current)) {
                        path->Prepend(neighbor);
                        current = neighbor;
                        foundPrev = true;
                        break;
                    }
                }
                delete adj;
            }
        
            if (!foundPrev) 
            {
                delete path;
                throw std::runtime_error("Cannot reconstruct path");
            }
        }
    
        return path;
    }
    
    // Нахождение кратчайшего пути между двумя вершинами (Дейкстра + восстановление)
    static Sequence<T>* FindShortestPath(const GraphBase<T>& graph, T startVertex, T endVertex) 
    {
        // Используем DijkstraWithPath, который сразу возвращает путь
        return DijkstraWithPath(graph, startVertex, endVertex);
    }
    
    // Вычисление диаметра графа
    static double GetGraphDiameter(const GraphBase<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) return 0.0;
        
        double maxDiameter = 0.0;
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T startVertex = vertices->Get(i);
            auto distances = Dijkstra(graph, startVertex);
            
            distances->ForEach([&maxDiameter](const T& vertex, double distance) 
            {
                if (distance < std::numeric_limits<double>::infinity() && distance > maxDiameter) 
                {
                    maxDiameter = distance;
                }
            });
            
            delete distances;
        }
        
        delete vertices;
        return maxDiameter;
    }
    
    // Вычисление радиуса графа
    static double GetGraphRadius(const GraphBase<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) return 0.0;
        
        double minRadius = std::numeric_limits<double>::infinity();
        auto vertices = graph.GetAllVertices();
        
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T startVertex = vertices->Get(i);
            auto distances = Dijkstra(graph, startVertex);
            
            double eccentricity = 0.0;
            distances->ForEach([&eccentricity](const T& vertex, double distance) 
            {
                if (distance < std::numeric_limits<double>::infinity() && distance > eccentricity) 
                {
                    eccentricity = distance;
                }
            });
            
            if (eccentricity < minRadius) 
            {
                minRadius = eccentricity;
            }
            
            delete distances;
        }
        
        delete vertices;
        return (minRadius == std::numeric_limits<double>::infinity()) ? 0.0 : minRadius;
    }
    
    // Нахождение центра графа
    static Sequence<T>* FindGraphCenter(const GraphBase<T>& graph) 
    {
        if (graph.GetVertexCount() == 0) return new MutableArraySequence<T>();
        
        auto* centerVertices = new MutableArraySequence<T>();
        double minEccentricity = std::numeric_limits<double>::infinity();
        auto vertices = graph.GetAllVertices();
        
        Dictionary<T, double> eccentricities;
        
        // Вычисляем эксцентриситет для каждой вершины
        for (int i = 0; i < vertices->GetLength(); i++) 
        {
            T vertex = vertices->Get(i);
            auto distances = Dijkstra(graph, vertex);
            
            double eccentricity = 0.0;
            distances->ForEach([&eccentricity](const T& v, double distance) 
            {
                if (distance < std::numeric_limits<double>::infinity() && distance > eccentricity) 
                {
                    eccentricity = distance;
                }
            });
            
            eccentricities.Add(vertex, eccentricity);
            
            if (eccentricity < minEccentricity) 
            {
                minEccentricity = eccentricity;
            }
            
            delete distances;
        }
        
        // Находим все вершины с минимальным эксцентриситетом
        eccentricities.ForEach([&](const T& vertex, double eccentricity) 
        {
            if (eccentricity == minEccentricity) 
            {
                centerVertices->Append(vertex);
            }
        });
        
        delete vertices;
        return centerVertices;
    }
};

#endif // SHORTEST_PATH_H