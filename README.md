# Лабораторная работа №3 по курсу "Информатика (организация и поиск данных)"

## Выполненные задания

### Структуры данных графов

**М-1.2. Неориентированный граф** (рейтинг: 6)  
*Реализовано в: `src/core/structures/graph/UndirectedGraph.h`*

**М-1.1. Ориентированный граф** (рейтинг: 5)  
*Реализовано в: `src/core/structures/graph/DirectedGraph.h`*

**Общая база графов**  
*Реализовано в: `src/core/structures/graph/GraphBase.h`*


### Алгоритмы на графах

**М-2.6. Поиск компонент связности неориентированного графа** (рейтинг: 4)  
*Реализовано в: `src/core/tasks/ConnectedComponents.h`*

**М-2.2.1. Поиск кратчайших путей на ориентированном/неориентированном графе** (рейтинг: 5)  
*Реализовано в: `src/core/tasks/ShortestPath.h`*

**М-2.8. Топологическое упорядочение** (рейтинг: 6)  
*Реализовано в: `src/core/tasks/TopologicalSort.h`*

**М-2.13. Построение частичного порядка, определение экстремальных характеристик** (рейтинг: 7)  
*Реализовано в: `src/core/tasks/PartialOrder.h`*

**М-3.1. Генерация графов заданной топологии и размера** (рейтинг: 10)  
*Встроена в классы графов*


### Структуры данных и алгоритмы поиска

**M-3. Бинарное дерево** (рейтинг: 15)
- Реализовано в файле `src/core/structures/binary_tree.h`

**M-5. Хеш-таблица (dictionary)** (рейтинг: 5)
- Реализовано в файле `src/core/structures/dictionary.h`

**DynamicArray (динамический массив)**
- Реализовано в файле `DynamicArray.h`

**Sequence (последовательность)** 
- Реализовано в файле `src/core/structures/Sequence.h`

**LinkedList (связанный список)**
- Реализовано в файле `src/core/structures/Linked_list.h`

**PriorityQueue (очередь с приоритетами)** (рейтинг: 5)
- Реализовано в файле `src/core/structures/priority_queue.h`

**Set (множество)** (рейтинг: 5)
- Реализовано в файле `src/core/structures/set.h`

**SortedSequence (сортированная последовательность)**
- Реализовано в файле `src/core/structures/sorted_sequence.h`

**Дополнительные реализации:**
- Итераторы для всех структур данных

## Структура проекта

```
lab23/
├── .vscode/                    # Конфигурация VS Code
├── build/                      # Сборка проекта
├── src/                        # Исходный код
│   ├── core/                   # Основная логика проекта
│   │   ├── structures/         # Реализации структур данных
│   │   │   ├── binary_tree.h   # Бинарное дерево поиска
│   │   │   ├── dictionary.h    # Хеш-таблица
│   │   │   ├── sequence.h      # Динамический массив
│   │   │   ├── linked_list.h   # Связанный список
│   │   │   ├── priority_queue.h # Очередь с приоритетами
│   │   │   ├── set.h           # Множество
│   │   │   └── sorted_sequence.h # Сортированная последовательность
│   │   ├── graph/
│   │   │   ├──GraphBase.h      # Базовая структура графа
│   │   │   ├──DirectedGraph.h  # Ориентированный граф
│   │   │   └──UndirectedGraph.h # Неориентированный граф
│   │   ├── tasks/              # Реализации задач
│   │   │   ├── histogram.h     # Построение гистограммы
│   │   │   ├── most_frequent_substrings.h # Частые подпоследовательности
│   │   │   └── task_prioritizer.h # Приоритезация задач
│   ├── main.cpp               # Основная программа с графическим интерфейсом
│   ├── tests.cpp              # Тесты для всех компонентов
│   ├── test.exe               # Исполняемый файл тестов
│   ├── gtest/                 # Google Test framework
│   └──CMakeLists.txt          # Файл сборки CMake
```

## Подробное описание компонентов

### 1. Структуры графов (`src/structures/graph/`)

#### **GraphBase.h** - Базовый класс графа
```cpp
template<typename Vertex, typename Weight = int>
class GraphBase {
public:
    virtual void addVertex(const Vertex& v);
    virtual void addEdge(const Vertex& from, const Vertex& to, Weight weight = 1);
    virtual void removeVertex(const Vertex& v);
    virtual void removeEdge(const Vertex& from, const Vertex& to);
    virtual bool hasVertex(const Vertex& v) const;
    virtual bool hasEdge(const Vertex& from, const Vertex& to) const;
    virtual Weight getEdgeWeight(const Vertex& from, const Vertex& to) const;
    virtual Sequence<Vertex> getVertices() const;
    virtual Sequence<Vertex> getNeighbors(const Vertex& v) const;
    virtual int getVertexCount() const;
    virtual int getEdgeCount() const;
    virtual bool isDirected() const = 0;
};
```

#### **DirectedGraph.h** - Ориентированный граф
```cpp
template<typename Vertex, typename Weight = int>
class DirectedGraph : public GraphBase<Vertex, Weight> {
public:
    DirectedGraph();
    // Специфичные для ориентированных графов методы
    Sequence<Vertex> getPredecessors(const Vertex& v) const;
    int getInDegree(const Vertex& v) const;
    int getOutDegree(const Vertex& v) const;
    bool isAcyclic() const;  // Проверка на ацикличность
    bool isDirected() const override { return true; }
};
```

#### **UndirectedGraph.h** - Неориентированный граф
```cpp
template<typename Vertex, typename Weight = int>
class UndirectedGraph : public GraphBase<Vertex, Weight> {
public:
    UndirectedGraph();
    // Специфичные для неориентированных графов методы
    int getDegree(const Vertex& v) const;
    bool isConnected() const;  // Проверка связности
    bool isDirected() const override { return false; }
};
```

### 2. Алгоритмы на графах (`src/tasks/`)

#### **ConnectedComponents.h** - Компоненты связности
```cpp
class ConnectedComponents {
public:
    template<typename Vertex, typename Weight>
    static Sequence<Sequence<Vertex>> findComponents(const UndirectedGraph<Vertex, Weight>& graph);
    
    template<typename Vertex, typename Weight>
    static bool isConnected(const UndirectedGraph<Vertex, Weight>& graph);
    
    template<typename Vertex, typename Weight>
    static int countComponents(const UndirectedGraph<Vertex, Weight>& graph);
};
```

#### **ShortestPath.h** - Поиск кратчайших путей
```cpp
class ShortestPath {
public:
    // Алгоритм Дейкстры для графов с неотрицательными весами
    template<typename Vertex, typename Weight>
    static Dictionary<Vertex, Weight> dijkstra(
        const GraphBase<Vertex
