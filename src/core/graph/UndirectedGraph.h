#ifndef UNDIRECTED_GRAPH_H
#define UNDIRECTED_GRAPH_H

#include "GraphBase.h"
#include "../structures/Dictionary.h"
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include <stdexcept>

template<typename T>
class UndirectedGraph : public GraphBase<T> 
{
private:
    Dictionary<T, Dictionary<T, double>> adjMatrix;
    Set<T> vertices;
    mutable MutableArraySequence<T>* cachedVertices;

    void invalidateCache() 
    {
        if (cachedVertices) 
        {
            delete cachedVertices;
            cachedVertices = nullptr;
        }
    }

public:
    UndirectedGraph() : GraphBase<T>(false), cachedVertices(nullptr) {}

    UndirectedGraph(const UndirectedGraph& other) : GraphBase<T>(false) 
    {
        vertices = other.vertices;
        adjMatrix = other.adjMatrix;
        this->vertexCount = other.vertexCount;
        this->edgeCount = other.edgeCount;
        cachedVertices = nullptr;
    }

    UndirectedGraph& operator=(const UndirectedGraph& other) 
    {
        if (this != &other) {
            Clear();
            vertices = other.vertices;
            adjMatrix = other.adjMatrix;
            this->vertexCount = other.vertexCount;
            this->edgeCount = other.edgeCount;
            invalidateCache();
        }
        return *this;
    }

    ~UndirectedGraph() override 
    {
        invalidateCache();
    }

    void AddVertex(T vertex) override 
    {
        if (!vertices.Contains(vertex)) 
        {
            vertices.Add(vertex);
            adjMatrix.Add(vertex, Dictionary<T, double>());
            this->vertexCount++;
            invalidateCache();
        }
    }

    void AddEdge(T from, T to, double weight = 1.0) override 
    {
        AddVertex(from);
        AddVertex(to);
        
        // Для неориентированного графа добавляем ребро в обе стороны
        Dictionary<T, double> adjDictFrom = adjMatrix.Get(from);
        Dictionary<T, double> adjDictTo = adjMatrix.Get(to);
        
        // Проверяем, существует ли уже ребро
        bool edgeExists = adjDictFrom.ContainsKey(to);
        
        if (!edgeExists) {
            this->edgeCount++; // В неориентированном графе считаем ребро один раз
        }
        
        // Добавляем в обе стороны
        adjDictFrom.Add(to, weight);
        adjDictTo.Add(from, weight);
        
        // Сохраняем обратно
        adjMatrix.Add(from, adjDictFrom);
        adjMatrix.Add(to, adjDictTo);
        
        invalidateCache();
    }

    void RemoveVertex(T vertex) override 
    {
        if (!HasVertex(vertex)) return;
        
        // Удаляем все рёбра, связанные с вершиной
        Dictionary<T, double> adjDict = adjMatrix.Get(vertex);
        
        // Удаляем рёбра из других вершин
        adjDict.ForEachKey([&](const T& neighbor) 
        {
            Dictionary<T, double> neighborDict = adjMatrix.Get(neighbor);
            if (neighborDict.ContainsKey(vertex)) 
            {
                neighborDict.Remove(vertex);
                adjMatrix.Add(neighbor, neighborDict);
            }
        });
        
        // Удаляем вершину
        this->edgeCount -= adjDict.GetCount();
        adjMatrix.Remove(vertex);
        vertices.Remove(vertex);
        this->vertexCount--;
        invalidateCache();
    }

    void RemoveEdge(T from, T to) override 
    {
        if (HasEdge(from, to)) 
        {
            // Удаляем в обе стороны
            Dictionary<T, double> adjDictFrom = adjMatrix.Get(from);
            Dictionary<T, double> adjDictTo = adjMatrix.Get(to);
            
            adjDictFrom.Remove(to);
            adjDictTo.Remove(from);
            
            adjMatrix.Add(from, adjDictFrom);
            adjMatrix.Add(to, adjDictTo);
            
            this->edgeCount--;
            invalidateCache();
        }
    }

    bool HasVertex(T vertex) const override 
    {
        return vertices.Contains(vertex);
    }

    bool HasEdge(T from, T to) const override 
    {
        if (!HasVertex(from) || !HasVertex(to)) return false;
        
        if (!adjMatrix.ContainsKey(from)) return false;
        
        Dictionary<T, double> adjDict = adjMatrix.Get(from);
        return adjDict.ContainsKey(to);
    }

    double GetEdgeWeight(T from, T to) const override 
    {
        if (!HasEdge(from, to)) 
        {
            throw std::runtime_error("Edge does not exist");
        }
        
        Dictionary<T, double> adjDict = adjMatrix.Get(from);
        return adjDict.Get(to);
    }

    void SetEdgeWeight(T from, T to, double weight) override 
    {
        if (!HasEdge(from, to))
        {
            throw std::runtime_error("Edge does not exist");
        }
        
        // Обновляем в обе стороны
        Dictionary<T, double> adjDictFrom = adjMatrix.Get(from);
        Dictionary<T, double> adjDictTo = adjMatrix.Get(to);
        
        adjDictFrom.Add(to, weight);
        adjDictTo.Add(from, weight);
        
        adjMatrix.Add(from, adjDictFrom);
        adjMatrix.Add(to, adjDictTo);
    }

    Sequence<T>* GetAdjacentVertices(T vertex) const override 
    {
        if (!HasVertex(vertex)) {
            throw std::runtime_error("Vertex does not exist");
        }
        
        auto* result = new MutableArraySequence<T>();
        
        if (adjMatrix.ContainsKey(vertex)) 
        {
            Dictionary<T, double> adjDict = adjMatrix.Get(vertex);
            adjDict.ForEachKey([result](const T& key) 
            {
                result->Append(key);
            });
        }
        
        return result;
    }

    Sequence<T>* GetAllVertices() const override 
    {
        if (!cachedVertices) {
            cachedVertices = new MutableArraySequence<T>();
            vertices.ForEach([this](const T& vertex) 
            {
                cachedVertices->Append(vertex);
            });
        }
        return new MutableArraySequence<T>(*cachedVertices);
    }

    void Clear() override 
    {
        vertices.Clear();
        adjMatrix.Clear();
        this->vertexCount = 0;
        this->edgeCount = 0;
        invalidateCache();
    }

    void SerializeToStream(std::ostream& os) const override 
    {
        os << "U " << this->vertexCount << " " << this->edgeCount << "\n";
        
        // Сохраняем вершины
        vertices.ForEach([&](const T& vertex) 
        {
            os << vertex << "\n";
        });
        
        // Сохраняем рёбра
        auto allVertices = GetAllVertices();
        Set<std::pair<T, T>> savedEdges;
        
        for (int i = 0; i < allVertices->GetLength(); i++) 
        {
            T from = allVertices->Get(i);
            if (adjMatrix.ContainsKey(from)) {
                Dictionary<T, double> adjDict = adjMatrix.Get(from);
                adjDict.ForEach([&](const T& to, double weight) 
                {
                    // Сохраняем ребро только один раз
                    if (from < to) 
                    {
                        os << from << " " << to << " " << weight << "\n";
                    }
                });
            }
        }
        
        delete allVertices;
    }

    // Статический метод для создания графа из потока
    static UndirectedGraph<T>* DeserializeFromStream(std::istream& is) 
    {
        std::string type;
        int vCount, eCount;
        
        if (!(is >> type >> vCount >> eCount) || type != "U") 
        {
            throw std::runtime_error("Invalid graph format");
        }
        
        auto* graph = new UndirectedGraph<T>();
        
        // Читаем вершины
        for (int i = 0; i < vCount; i++) 
        {
            T vertex;
            if (!(is >> vertex)) 
            {
                delete graph;
                throw std::runtime_error("Failed to read vertex");
            }
            graph->AddVertex(vertex);
        }
        
        // Читаем рёбра
        for (int i = 0; i < eCount; i++) 
        {
            T from, to;
            double weight;
            if (!(is >> from >> to >> weight)) 
            {
                delete graph;
                throw std::runtime_error("Failed to read edge");
            }
            graph->AddEdge(from, to, weight);
        }
        
        return graph;
    }
};

#endif // UNDIRECTED_GRAPH_H