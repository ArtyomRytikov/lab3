#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H

#include "GraphBase.h"
#include "../structures/Dictionary.h"
#include "../structures/Set.h"
#include "../structures/Sequence.h"
#include <stdexcept>

template<typename T>
class DirectedGraph : public GraphBase<T> 
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
    DirectedGraph() : GraphBase<T>(true), cachedVertices(nullptr) {}

    DirectedGraph(const DirectedGraph& other) : GraphBase<T>(true) 
    {
        vertices = other.vertices;
        adjMatrix = other.adjMatrix;
        this->vertexCount = other.vertexCount;
        this->edgeCount = other.edgeCount;
        cachedVertices = nullptr;
    }

    DirectedGraph& operator=(const DirectedGraph& other) 
    {
        if (this != &other) 
        {
            Clear();
            vertices = other.vertices;
            adjMatrix = other.adjMatrix;
            this->vertexCount = other.vertexCount;
            this->edgeCount = other.edgeCount;
            invalidateCache();
        }
        return *this;
    }

    ~DirectedGraph() override 
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
        
        // Получаем словарь смежности для вершины from
        Dictionary<T, double> adjDict = adjMatrix.Get(from);
        
        // Проверяем, существует ли уже ребро
        if (!adjDict.ContainsKey(to)) 
        {
            this->edgeCount++;
        }
        
        // Добавляем или обновляем ребро
        adjDict.Add(to, weight);
        adjMatrix.Add(from, adjDict);
        invalidateCache();
    }

    void RemoveVertex(T vertex) override 
    {
        if (!HasVertex(vertex)) return;
        
        // Удаляем все исходящие рёбра
        if (adjMatrix.ContainsKey(vertex)) 
        {
            this->edgeCount -= adjMatrix.Get(vertex).GetCount();
            adjMatrix.Remove(vertex);
        }
        
        // Удаляем все входящие рёбра
        // Проходим по всем вершинам
        vertices.ForEach([&](const T& v) 
        {
            if (v != vertex && adjMatrix.ContainsKey(v) && adjMatrix.Get(v).ContainsKey(vertex)) 
            {
                adjMatrix.Get(v).Remove(vertex);
                this->edgeCount--;
            }
        });
        
        vertices.Remove(vertex);
        this->vertexCount--;
        invalidateCache();
    }

    void RemoveEdge(T from, T to) override 
    {
        if (HasEdge(from, to)) 
        {
            // Получаем текущий словарь, удаляем ребро, обновляем
            Dictionary<T, double> adjDict = adjMatrix.Get(from);
            adjDict.Remove(to);
            adjMatrix.Add(from, adjDict);
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
        
        // Проверяем, есть ли ключ 'from' в словаре
        if (!adjMatrix.ContainsKey(from)) return false;
        
        return adjMatrix.Get(from).ContainsKey(to);
    }

    double GetEdgeWeight(T from, T to) const override 
    {
        if (!HasEdge(from, to)) 
        {
            throw std::runtime_error("Edge does not exist");
        }
        return adjMatrix.Get(from).Get(to);
    }

    void SetEdgeWeight(T from, T to, double weight) override 
    {
        if (!HasEdge(from, to)) 
        {
            throw std::runtime_error("Edge does not exist");
        }
        // Получаем текущий словарь, обновляем, сохраняем обратно
        Dictionary<T, double> adjDict = adjMatrix.Get(from);
        adjDict.Add(to, weight);
        adjMatrix.Add(from, adjDict);
    }

    Sequence<T>* GetAdjacentVertices(T vertex) const override 
    {
        if (!HasVertex(vertex)) 
        {
            throw std::runtime_error("Vertex does not exist");
        }
        
        auto* result = new MutableArraySequence<T>();
        
        // Проверяем, есть ли вершина в словаре смежности
        if (adjMatrix.ContainsKey(vertex)) 
        {
            auto dict = adjMatrix.Get(vertex);
            dict.ForEachKey([result](const T& key) 
            {
                result->Append(key);
            });
        }
        
        return result;
    }

    // Получение вершин, входящих в данную
    Sequence<T>* GetIncomingVertices(T vertex) const 
    {
        if (!HasVertex(vertex)) {
            throw std::runtime_error("Vertex does not exist");
        }
        
        auto* result = new MutableArraySequence<T>();
        
        // Проходим по всем вершинам и ищем те, у которых есть ребро в vertex
        vertices.ForEach([&](const T& v) {
            if (v != vertex && HasEdge(v, vertex)) 
            {
                result->Append(v);
            }
        });
        
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
        os << "D " << this->vertexCount << " " << this->edgeCount << "\n";
        
        // Сохраняем вершины
        vertices.ForEach([&](const T& vertex) 
        {
            os << vertex << "\n";
        });
        
        // Сохраняем рёбра
        vertices.ForEach([&](const T& from) 
        {
            if (adjMatrix.ContainsKey(from)) 
            {
                auto dict = adjMatrix.Get(from);
                dict.ForEach([&](const T& to, double weight) 
                {
                    os << from << " " << to << " " << weight << "\n";
                });
            }
        });
    }

    // Статический метод для создания графа из потока
    static DirectedGraph<T>* DeserializeFromStream(std::istream& is) 
    {
        std::string type;
        int vCount, eCount;
        
        if (!(is >> type >> vCount >> eCount) || type != "D") {
            throw std::runtime_error("Invalid graph format");
        }
        
        auto* graph = new DirectedGraph<T>();
        
        // Читаем вершины
        for (int i = 0; i < vCount; i++) 
        {
            T vertex;
            if (!(is >> vertex)) {
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

#endif // DIRECTED_GRAPH_H