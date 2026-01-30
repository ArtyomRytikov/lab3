#ifdef _WIN32
    #undef main
    #define main WinMain
#endif

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/statline.h>
#include <wx/numdlg.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <limits>
#include <stack>
#include <set>

#include "core/graph/UndirectedGraph.h"
#include "core/graph/DirectedGraph.h"
#include "core/tasks/ConnectedComponents.h"
#include "core/tasks/ShortestPath.h"
#include "core/tasks/TopologicalSort.h"
#include "core/tasks/PartialOrder.h"

// ID для элементов управления
enum ControlIDs {
    ID_AddVertex = wxID_HIGHEST + 1,
    ID_AddEdge,
    ID_RemoveVertex,
    ID_RemoveEdge,
    ID_ClearGraph,
    ID_GenerateRandom,
    ID_FindComponents,
    ID_CheckConnected,
    ID_CountComponents,
    ID_FindLargestComponent,
    ID_FindShortestPath,
    ID_GetDiameter,
    ID_GetRadius,
    ID_FindCenter,
    ID_TopologicalSort,
    ID_CheckAcyclic,
    ID_FindSourcesSinks,
    ID_CheckPartialOrder,
    ID_BuildHasse,
    ID_FindMinMaxElements,
    ID_FindInfSup,
    ID_CheckLattice,
    ID_GetLevels,
    ID_GraphType,
    ID_Vertex1,
    ID_Vertex2,
    ID_StartVertex,
    ID_EndVertex,
    ID_VisualizationPanel,
    ID_RefreshGraph
};

std::string WxStringToString(const wxString& str) {
    return str.ToStdString(wxConvUTF8);
}

wxString StringToWxString(const std::string& str) {
    return wxString::FromUTF8(str.c_str());
}

// Преобразование типа графа
template<typename T>
GraphBase<T>* CreateGraph(bool isDirected) {
    if (isDirected) {
        return new DirectedGraph<T>();
    } else {
        return new UndirectedGraph<T>();
    }
}

// Простая структура для ребра
struct Edge {
    int from;
    int to;
    double weight;
    
    Edge(int f, int t, double w) : from(f), to(t), weight(w) {}
    
    bool operator==(const Edge& other) const {
        return from == other.from && to == other.to;
    }
};

// Класс для визуализации графа
class GraphVisualizationPanel : public wxPanel {
private:
    std::vector<int>* vertices;
    std::vector<Edge>* edges;
    bool* isDirected;
    std::vector<int> selectedPath;
    std::map<int, wxPoint> vertexPositions;
    std::map<int, wxColor> vertexColors;
    std::vector<Edge> hasseEdges;
    bool showHasseDiagram;
    int selectedVertex;
    wxPoint dragStart;
    bool isDragging;
    
    void CalculateVertexPositions() {
        vertexPositions.clear();
        vertexColors.clear();
        
        if (!vertices || vertices->empty()) return;
        
        int centerX = GetSize().GetWidth() / 2;
        int centerY = GetSize().GetHeight() / 2;
        int radius = std::min(centerX, centerY) - 100;
        
        int n = vertices->size();
        for (size_t i = 0; i < vertices->size(); i++) {
            int vertex = (*vertices)[i];
            double angle = 2 * M_PI * i / n;
            int x = centerX + radius * cos(angle);
            int y = centerY + radius * sin(angle);
            vertexPositions[vertex] = wxPoint(x, y);
            
            // Цвет по умолчанию
            vertexColors[vertex] = *wxBLUE;
        }
        
        // Цвет для вершин в пути
        for (int v : selectedPath) {
            if (vertexColors.find(v) != vertexColors.end()) {
                vertexColors[v] = wxColor(255, 0, 0); // Красный для вершин пути
            }
        }
    }
    
    void DrawVertex(wxDC& dc, int vertex, const wxPoint& pos) {
        wxColor color = vertexColors[vertex];
        dc.SetBrush(wxBrush(color));
        dc.SetPen(wxPen(*wxBLACK, 2));
        
        // Рисуем круг
        int radius = 20;
        dc.DrawCircle(pos, radius);
        
        // Рисуем номер вершины
        dc.SetTextForeground(*wxWHITE);
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        wxString label = wxString::Format("%d", vertex);
        wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label, pos.x - textSize.GetWidth()/2, pos.y - textSize.GetHeight()/2);
        
        // Если вершина выбрана
        if (vertex == selectedVertex) {
            dc.SetPen(wxPen(*wxRED, 3));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawCircle(pos, radius + 3);
        }
    }
    
    void DrawEdge(wxDC& dc, int from, int to, double weight, bool isHasseEdge = false) {
        if (vertexPositions.find(from) == vertexPositions.end() ||
            vertexPositions.find(to) == vertexPositions.end()) {
            return;
        }
        
        wxPoint fromPos = vertexPositions[from];
        wxPoint toPos = vertexPositions[to];
        
        // Вычисляем направление
        wxPoint dir = toPos - fromPos;
        double length = sqrt(dir.x*dir.x + dir.y*dir.y);
        if (length < 1) return;
        
        // Нормализуем
        dir.x = (dir.x * 20) / length;
        dir.y = (dir.y * 20) / length;
        
        wxPoint start = fromPos + dir;
        wxPoint end = toPos - dir;
        
        // Рисуем линию
        if (isHasseEdge) {
            dc.SetPen(wxPen(*wxRED, 3)); // Красные толстые линии для диаграммы Хассе
        } else {
            dc.SetPen(wxPen(*wxBLACK, 2));
        }
        dc.DrawLine(start, end);
        
        // Рисуем стрелку для ориентированного графа
        if (*isDirected && !isHasseEdge) { // Диаграмма Хассе не имеет стрелок
            // Вычисляем угол
            double angle = atan2(dir.y, dir.x);
            
            // Рисуем стрелку
            wxPoint arrow1(end.x - 10 * cos(angle - M_PI/6),
                          end.y - 10 * sin(angle - M_PI/6));
            wxPoint arrow2(end.x - 10 * cos(angle + M_PI/6),
                          end.y - 10 * sin(angle + M_PI/6));
            
            wxPoint arrowPoints[3] = {end, arrow1, arrow2};
            dc.SetBrush(wxBrush(*wxBLACK));
            dc.DrawPolygon(3, arrowPoints);
        }
        
        // Рисуем вес (только для не-Hasse ребер и если вес не равен 1)
        if (!isHasseEdge && weight != 1.0) {
            wxPoint mid((start.x + end.x)/2, (start.y + end.y)/2);
            wxString weightStr = wxString::Format("%.1f", weight);
            dc.SetTextForeground(*wxBLACK);
            dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
            wxSize textSize = dc.GetTextExtent(weightStr);
            dc.DrawText(weightStr, mid.x - textSize.GetWidth()/2, mid.y - textSize.GetHeight()/2 - 10);
        }
    }
    
    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        Render(dc);
    }
    
    void OnEraseBackground(wxEraseEvent& event) {
        // Пустая функция для предотвращения мерцания
    }
    
    void Render(wxDC& dc) {
        // Очищаем фон
        dc.SetBrush(wxBrush(*wxWHITE));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(GetClientRect());
        
        if (!vertices || vertices->empty()) {
            dc.SetTextForeground(*wxBLACK);
            dc.SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
            wxString message = wxT("Граф пуст");
            wxSize textSize = dc.GetTextExtent(message);
            wxPoint center(GetSize().GetWidth()/2 - textSize.GetWidth()/2,
                          GetSize().GetHeight()/2 - textSize.GetHeight()/2);
            dc.DrawText(message, center);
            return;
        }
        
        // Пересчитываем позиции если нужно
        if (vertexPositions.empty()) {
            CalculateVertexPositions();
        }
        
        // Сначала рисуем ребра
        if (edges && !showHasseDiagram) {
            for (const auto& edge : *edges) {
                DrawEdge(dc, edge.from, edge.to, edge.weight);
            }
        }
        
        // Затем рисуем ребра диаграммы Хассе
        if (showHasseDiagram && !hasseEdges.empty()) {
            for (const auto& edge : hasseEdges) {
                DrawEdge(dc, edge.from, edge.to, edge.weight, true);
            }
        }
        
        // Затем рисуем вершины
        for (const auto& pos : vertexPositions) {
            DrawVertex(dc, pos.first, pos.second);
        }
    }
    
    void OnMouseDown(wxMouseEvent& event) {
        wxPoint mousePos = event.GetPosition();
        
        // Ищем вершину под мышкой
        selectedVertex = -1;
        for (const auto& pos : vertexPositions) {
            wxPoint vertexPos = pos.second;
            double distance = sqrt(pow(mousePos.x - vertexPos.x, 2) + 
                                 pow(mousePos.y - vertexPos.y, 2));
            if (distance <= 20) {
                selectedVertex = pos.first;
                dragStart = mousePos;
                isDragging = true;
                CaptureMouse();
                Refresh();
                break;
            }
        }
        
        event.Skip();
    }
    
    void OnMouseMove(wxMouseEvent& event) {
        if (isDragging && selectedVertex != -1) {
            wxPoint mousePos = event.GetPosition();
            vertexPositions[selectedVertex] = mousePos;
            Refresh();
        }
        event.Skip();
    }
    
    void OnMouseUp(wxMouseEvent& event) {
        if (isDragging) {
            isDragging = false;
            if (HasCapture()) {
                ReleaseMouse();
            }
        }
        event.Skip();
    }
    
    void OnSize(wxSizeEvent& event) {
        CalculateVertexPositions();
        Refresh();
        event.Skip();
    }
    
public:
    GraphVisualizationPanel(wxWindow* parent, 
                          std::vector<int>* verts,
                          std::vector<Edge>* edgs,
                          bool* directed)
        : wxPanel(parent, ID_VisualizationPanel, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE),
          vertices(verts), edges(edgs), isDirected(directed),
          selectedVertex(-1), isDragging(false), showHasseDiagram(false) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetDoubleBuffered(true);
        
        Bind(wxEVT_PAINT, &GraphVisualizationPanel::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &GraphVisualizationPanel::OnEraseBackground, this);
        Bind(wxEVT_LEFT_DOWN, &GraphVisualizationPanel::OnMouseDown, this);
        Bind(wxEVT_LEFT_UP, &GraphVisualizationPanel::OnMouseUp, this);
        Bind(wxEVT_MOTION, &GraphVisualizationPanel::OnMouseMove, this);
        Bind(wxEVT_SIZE, &GraphVisualizationPanel::OnSize, this);
    }
    
    void UpdateGraph() {
        CalculateVertexPositions();
        Refresh();
    }
    
    void SetSelectedPath(const std::vector<int>& path) {
        selectedPath = path;
        CalculateVertexPositions();
        Refresh();
    }
    
    void ClearSelection() {
        selectedVertex = -1;
        selectedPath.clear();
        CalculateVertexPositions();
        Refresh();
    }
    
    void SetHasseEdges(const std::vector<Edge>& edges) {
        hasseEdges = edges;
        showHasseDiagram = !edges.empty();
        Refresh();
    }
    
    void ClearHasseDiagram() {
        hasseEdges.clear();
        showHasseDiagram = false;
        Refresh();
    }
    
    bool IsShowingHasseDiagram() const {
        return showHasseDiagram;
    }
};

// Простые алгоритмы на графах (локальная реализация)
class SimpleGraphAlgorithms {
public:
    // Алгоритм Дейкстры
    static std::vector<int> Dijkstra(const std::vector<int>& vertices,
                                    const std::vector<Edge>& edges,
                                    int start, int end,
                                    bool isDirected = false) {
        // Создаем граф в виде списка смежности
        std::map<int, std::vector<std::pair<int, double>>> adj;
        for (const auto& vertex : vertices) {
            adj[vertex] = std::vector<std::pair<int, double>>();
        }
        
        for (const auto& edge : edges) {
            adj[edge.from].push_back({edge.to, edge.weight});
            // Для неориентированного графа добавляем обратное ребро
            if (!isDirected) {
                adj[edge.to].push_back({edge.from, edge.weight});
            }
        }
        
        // Инициализация
        std::map<int, double> dist;
        std::map<int, int> prev;
        std::priority_queue<std::pair<double, int>,
                           std::vector<std::pair<double, int>>,
                           std::greater<std::pair<double, int>>> pq;
        
        for (const auto& vertex : vertices) {
            dist[vertex] = std::numeric_limits<double>::infinity();
        }
        dist[start] = 0.0;
        pq.push({0.0, start});
        
        // Основной цикл
        while (!pq.empty()) {
            auto [currentDist, current] = pq.top();
            pq.pop();
            
            if (currentDist > dist[current]) continue;
            
            if (adj.find(current) != adj.end()) {
                for (const auto& [neighbor, weight] : adj[current]) {
                    double newDist = currentDist + weight;
                    if (newDist < dist[neighbor]) {
                        dist[neighbor] = newDist;
                        prev[neighbor] = current;
                        pq.push({newDist, neighbor});
                    }
                }
            }
        }
        
        // Восстановление пути
        std::vector<int> path;
        if (dist[end] == std::numeric_limits<double>::infinity()) {
            return path; // Путь не существует
        }
        
        for (int at = end; at != start; at = prev[at]) {
            path.push_back(at);
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        
        return path;
    }
    
    // Вычисление диаметра графа
    static double GetGraphDiameter(const std::vector<int>& vertices,
                                  const std::vector<Edge>& edges,
                                  bool isDirected = false) {
        double maxDiameter = 0.0;
        
        for (int start : vertices) {
            auto distances = GetAllDistances(vertices, edges, start, isDirected);
            
            for (const auto& [vertex, dist] : distances) {
                if (dist < std::numeric_limits<double>::infinity() && dist > maxDiameter) {
                    maxDiameter = dist;
                }
            }
        }
        
        return maxDiameter;
    }
    
    // Вычисление радиуса графа
    static double GetGraphRadius(const std::vector<int>& vertices,
                                const std::vector<Edge>& edges,
                                bool isDirected = false) {
        double minRadius = std::numeric_limits<double>::infinity();
        
        for (int start : vertices) {
            auto distances = GetAllDistances(vertices, edges, start, isDirected);
            
            double eccentricity = 0.0;
            for (const auto& [vertex, dist] : distances) {
                if (dist < std::numeric_limits<double>::infinity() && dist > eccentricity) {
                    eccentricity = dist;
                }
            }
            
            if (eccentricity < minRadius) {
                minRadius = eccentricity;
            }
        }
        
        return (minRadius == std::numeric_limits<double>::infinity()) ? 0.0 : minRadius;
    }
    
    // Нахождение центра графа
    static std::vector<int> FindGraphCenter(const std::vector<int>& vertices,
                                           const std::vector<Edge>& edges,
                                           bool isDirected = false) {
        std::vector<int> centerVertices;
        double minEccentricity = std::numeric_limits<double>::infinity();
        std::map<int, double> eccentricities;
        
        // Вычисляем эксцентриситет для каждой вершины
        for (int vertex : vertices) {
            auto distances = GetAllDistances(vertices, edges, vertex, isDirected);
            
            double eccentricity = 0.0;
            for (const auto& [v, dist] : distances) {
                if (dist < std::numeric_limits<double>::infinity() && dist > eccentricity) {
                    eccentricity = dist;
                }
            }
            
            eccentricities[vertex] = eccentricity;
            
            if (eccentricity < minEccentricity) {
                minEccentricity = eccentricity;
            }
        }
        
        // Находим все вершины с минимальным эксцентриситетом
        for (const auto& [vertex, eccentricity] : eccentricities) {
            if (eccentricity == minEccentricity) {
                centerVertices.push_back(vertex);
            }
        }
        
        return centerVertices;
    }
    
    // Топологическая сортировка (алгоритм Кана)
    static std::vector<int> TopologicalSortKahn(const std::vector<int>& vertices,
                                               const std::vector<Edge>& edges,
                                               bool isDirected) {
        if (!isDirected) {
            return std::vector<int>();
        }
        
        // Создаем граф в виде списка смежности
        std::map<int, std::vector<int>> adj;
        std::map<int, int> inDegree;
        
        for (const auto& vertex : vertices) {
            adj[vertex] = std::vector<int>();
            inDegree[vertex] = 0;
        }
        
        for (const auto& edge : edges) {
            adj[edge.from].push_back(edge.to);
            inDegree[edge.to]++;
        }
        
        // Очередь вершин с нулевой входящей степенью
        std::queue<int> zeroInDegree;
        for (const auto& [vertex, degree] : inDegree) {
            if (degree == 0) {
                zeroInDegree.push(vertex);
            }
        }
        
        std::vector<int> result;
        
        while (!zeroInDegree.empty()) {
            int vertex = zeroInDegree.front();
            zeroInDegree.pop();
            result.push_back(vertex);
            
            for (int neighbor : adj[vertex]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    zeroInDegree.push(neighbor);
                }
            }
        }
        
        // Проверка на циклы
        if (result.size() != vertices.size()) {
            return std::vector<int>(); // Граф содержит циклы
        }
        
        return result;
    }
    
    // Проверка ацикличности
    static bool IsAcyclic(const std::vector<int>& vertices,
                         const std::vector<Edge>& edges,
                         bool isDirected) {
        if (!isDirected) return true; // Неориентированный граф всегда ацикличен для этой проверки
        
        std::map<int, std::vector<int>> adj;
        std::map<int, int> visited; // 0 - не посещена, 1 - в стеке, 2 - обработана
        
        for (const auto& vertex : vertices) {
            adj[vertex] = std::vector<int>();
            visited[vertex] = 0;
        }
        
        for (const auto& edge : edges) {
            adj[edge.from].push_back(edge.to);
        }
        
        for (const auto& vertex : vertices) {
            if (visited[vertex] == 0) {
                if (HasCycleDFS(vertex, adj, visited)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // Нахождение источников и стоков
    static std::pair<std::vector<int>, std::vector<int>> FindSourcesAndSinks(
        const std::vector<int>& vertices,
        const std::vector<Edge>& edges,
        bool isDirected) {
        
        std::vector<int> sources;
        std::vector<int> sinks;
        
        if (!isDirected) {
            return {sources, sinks};
        }
        
        std::map<int, int> outDegree;
        std::map<int, int> inDegree;
        
        for (const auto& vertex : vertices) {
            outDegree[vertex] = 0;
            inDegree[vertex] = 0;
        }
        
        for (const auto& edge : edges) {
            outDegree[edge.from]++;
            inDegree[edge.to]++;
        }
        
        for (const auto& vertex : vertices) {
            if (inDegree[vertex] == 0) {
                sources.push_back(vertex);
            }
            if (outDegree[vertex] == 0) {
                sinks.push_back(vertex);
            }
        }
        
        return {sources, sinks};
    }
    
    // Компоненты связности для неориентированных графов (BFS)
    static std::vector<std::vector<int>> FindConnectedComponents(
        const std::vector<int>& vertices,
        const std::vector<Edge>& edges) {
        
        // Создаем граф в виде списка смежности
        std::map<int, std::vector<int>> adj;
        std::map<int, bool> visited;
        
        for (const auto& vertex : vertices) {
            adj[vertex] = std::vector<int>();
            visited[vertex] = false;
        }
        
        // Добавляем ребра в обе стороны для неориентированного графа
        for (const auto& edge : edges) {
            adj[edge.from].push_back(edge.to);
            adj[edge.to].push_back(edge.from);
        }
        
        std::vector<std::vector<int>> components;
        
        for (const auto& vertex : vertices) {
            if (!visited[vertex]) {
                std::vector<int> component;
                std::queue<int> q;
                
                q.push(vertex);
                visited[vertex] = true;
                
                while (!q.empty()) {
                    int current = q.front();
                    q.pop();
                    component.push_back(current);
                    
                    for (int neighbor : adj[current]) {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            q.push(neighbor);
                        }
                    }
                }
                
                if (!component.empty()) {
                    components.push_back(component);
                }
            }
        }
        
        return components;
    }
    
    // Проверка связности
    static bool IsConnected(const std::vector<int>& vertices,
                           const std::vector<Edge>& edges) {
        auto components = FindConnectedComponents(vertices, edges);
        return components.size() == 1;
    }
    
    // Подсчет компонент связности
    static int CountComponents(const std::vector<int>& vertices,
                              const std::vector<Edge>& edges) {
        auto components = FindConnectedComponents(vertices, edges);
        return components.size();
    }
    
    // Нахождение наибольшей компоненты
    static std::vector<int> FindLargestComponent(const std::vector<int>& vertices,
                                                const std::vector<Edge>& edges) {
        auto components = FindConnectedComponents(vertices, edges);
        
        if (components.empty()) {
            return std::vector<int>();
        }
        
        auto largest = *std::max_element(components.begin(), components.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
                return a.size() < b.size();
            });
        
        return largest;
    }
    
private:
    // Получение всех расстояний от стартовой вершины
    static std::map<int, double> GetAllDistances(const std::vector<int>& vertices,
                                                const std::vector<Edge>& edges,
                                                int start,
                                                bool isDirected = false) {
        std::map<int, std::vector<std::pair<int, double>>> adj;
        for (const auto& vertex : vertices) {
            adj[vertex] = std::vector<std::pair<int, double>>();
        }
        
        for (const auto& edge : edges) {
            adj[edge.from].push_back({edge.to, edge.weight});
            if (!isDirected) {
                adj[edge.to].push_back({edge.from, edge.weight});
            }
        }
        
        std::map<int, double> dist;
        std::priority_queue<std::pair<double, int>,
                           std::vector<std::pair<double, int>>,
                           std::greater<std::pair<double, int>>> pq;
        
        for (const auto& vertex : vertices) {
            dist[vertex] = std::numeric_limits<double>::infinity();
        }
        dist[start] = 0.0;
        pq.push({0.0, start});
        
        while (!pq.empty()) {
            auto [currentDist, current] = pq.top();
            pq.pop();
            
            if (currentDist > dist[current]) continue;
            
            if (adj.find(current) != adj.end()) {
                for (const auto& [neighbor, weight] : adj[current]) {
                    double newDist = currentDist + weight;
                    if (newDist < dist[neighbor]) {
                        dist[neighbor] = newDist;
                        pq.push({newDist, neighbor});
                    }
                }
            }
        }
        
        return dist;
    }
    
    static bool HasCycleDFS(int vertex,
                           const std::map<int, std::vector<int>>& adj,
                           std::map<int, int>& visited) {
        visited[vertex] = 1; // В стеке
        
        if (adj.find(vertex) != adj.end()) {
            for (int neighbor : adj.at(vertex)) {
                if (visited[neighbor] == 1) {
                    return true; // Нашли цикл
                }
                if (visited[neighbor] == 0) {
                    if (HasCycleDFS(neighbor, adj, visited)) {
                        return true;
                    }
                }
            }
        }
        
        visited[vertex] = 2; // Обработана
        return false;
    }
};

// Простая реализация алгоритмов для частичного порядка
class SimplePartialOrder {
public:
    // Вычисление транзитивного замыкания (алгоритм Уоршелла)
    static std::map<int, std::set<int>> TransitiveClosure(const std::vector<int>& vertices,
                                                         const std::vector<Edge>& edges) {
        std::map<int, std::set<int>> closure;
        
        // Инициализация
        for (int v : vertices) {
            closure[v] = std::set<int>();
        }
        
        // Прямые ребра
        for (const auto& edge : edges) {
            closure[edge.from].insert(edge.to);
        }
        
        // Рефлексивность (каждая вершина достижима из себя)
        for (int v : vertices) {
            closure[v].insert(v);
        }
        
        // Алгоритм Уоршелла
        for (int k : vertices) {
            for (int i : vertices) {
                if (closure[i].count(k) > 0) {
                    for (int j : vertices) {
                        if (closure[k].count(j) > 0) {
                            closure[i].insert(j);
                        }
                    }
                }
            }
        }
        
        return closure;
    }
    
    // Проверка, является ли граф отношением частичного порядка
    static bool IsPartialOrder(const std::vector<int>& vertices,
                              const std::vector<Edge>& edges,
                              bool isDirected) {
        if (!isDirected) return false;
        
        // Проверка антисимметричности
        for (const auto& edge1 : edges) {
            for (const auto& edge2 : edges) {
                if (edge1.from == edge2.to && edge1.to == edge2.from) {
                    if (edge1.from != edge1.to) { // Исключаем петли
                        return false; // Нашли симметричные ребра
                    }
                }
            }
        }
        
        // Проверка ацикличности через DFS
        return SimpleGraphAlgorithms::IsAcyclic(vertices, edges, isDirected);
    }
    
    // Построение диаграммы Хассе
    static std::vector<Edge> BuildHasseDiagram(const std::vector<int>& vertices,
                                              const std::vector<Edge>& edges) {
        std::vector<Edge> hasseEdges;
        
        // Вычисляем транзитивное замыкание
        auto closure = TransitiveClosure(vertices, edges);
        
        // Для каждого ребра проверяем, является ли оно покрывающим
        for (const auto& edge : edges) {
            bool isCovering = true;
            
            // Проверяем все возможные промежуточные вершины
            for (int z : vertices) {
                if (z != edge.from && z != edge.to) {
                    // x < z в транзитивном замыкании?
                    bool x_less_z = (closure[edge.from].count(z) > 0);
                    // z < y в транзитивном замыкании?
                    bool z_less_y = (closure[z].count(edge.to) > 0);
                    
                    // Проверяем, что z строго между x и y
                    if (x_less_z && z_less_y) {
                        // Проверяем, что x != z и z != y
                        bool x_not_equal_z = (edge.from != z);
                        bool z_not_equal_y = (z != edge.to);
                        
                        // Проверяем, что z не равно ни x, ни y, и достижимость строгая
                        if (x_not_equal_z && z_not_equal_y) {
                            // Проверяем, что z действительно промежуточная вершина
                            // (не должно быть равенства x=z или z=y)
                            isCovering = false;
                            break;
                        }
                    }
                }
            }
            
            if (isCovering) {
                // Убедимся, что это ребро еще не добавлено
                if (std::find(hasseEdges.begin(), hasseEdges.end(), edge) == hasseEdges.end()) {
                    hasseEdges.push_back(edge);
                }
            }
        }
        
        return hasseEdges;
    }
    
    // Нахождение минимальных элементов
    static std::vector<int> FindMinimalElements(const std::vector<int>& vertices,
                                               const std::vector<Edge>& edges) {
        std::vector<int> minimalElements;
        
        // Вычисляем транзитивное замыкание для более точного определения
        auto closure = TransitiveClosure(vertices, edges);
        
        for (int vertex : vertices) {
            bool isMinimal = true;
            
            // Проверяем, нет ли элемента, который строго меньше данного
            for (int other : vertices) {
                if (other != vertex) {
                    // other < vertex?
                    bool other_less_vertex = (closure[other].count(vertex) > 0);
                    // vertex < other?
                    bool vertex_less_other = (closure[vertex].count(other) > 0);
                    
                    // Если other меньше vertex и vertex не меньше other (строгое неравенство)
                    if (other_less_vertex && !vertex_less_other) {
                        isMinimal = false;
                        break;
                    }
                }
            }
            
            if (isMinimal) {
                minimalElements.push_back(vertex);
            }
        }
        
        return minimalElements;
    }
    
    // Нахождение максимальных элементов
    static std::vector<int> FindMaximalElements(const std::vector<int>& vertices,
                                               const std::vector<Edge>& edges) {
        std::vector<int> maximalElements;
        
        // Вычисляем транзитивное замыкание для более точного определения
        auto closure = TransitiveClosure(vertices, edges);
        
        for (int vertex : vertices) {
            bool isMaximal = true;
            
            // Проверяем, нет ли элемента, который строго больше данного
            for (int other : vertices) {
                if (other != vertex) {
                    // vertex < other?
                    bool vertex_less_other = (closure[vertex].count(other) > 0);
                    // other < vertex?
                    bool other_less_vertex = (closure[other].count(vertex) > 0);
                    
                    // Если vertex меньше other и other не меньше vertex (строгое неравенство)
                    if (vertex_less_other && !other_less_vertex) {
                        isMaximal = false;
                        break;
                    }
                }
            }
            
            if (isMaximal) {
                maximalElements.push_back(vertex);
            }
        }
        
        return maximalElements;
    }
    
    // Вычисление уровней вершин для диаграммы Хассе
    static std::map<int, int> CalculateHasseLevels(const std::vector<int>& vertices,
                                                  const std::vector<Edge>& hasseEdges) {
        std::map<int, int> levels;
        
        // Инициализируем все вершины уровнем 0
        for (int vertex : vertices) {
            levels[vertex] = 0;
        }
        
        // Вычисляем уровни: уровень вершины = максимальный уровень предшественников + 1
        bool changed;
        do {
            changed = false;
            for (const auto& edge : hasseEdges) {
                int fromLevel = levels[edge.from];
                int toLevel = levels[edge.to];
                
                if (toLevel <= fromLevel) {
                    levels[edge.to] = fromLevel + 1;
                    changed = true;
                }
            }
        } while (changed);
        
        return levels;
    }
};

// Класс для панели визуализации во вкладке частичного порядка
class PartialOrderVisualizationPanel : public wxPanel {
private:
    std::vector<int>* vertices;
    std::vector<Edge>* edges;
    bool* isDirected;
    std::map<int, wxPoint> vertexPositions;
    std::map<int, wxColor> vertexColors;
    std::vector<Edge> hasseEdges;
    
    void CalculateVertexPositions() {
        vertexPositions.clear();
        vertexColors.clear();
        
        if (!vertices || vertices->empty()) return;
        
        int panelWidth = GetSize().GetWidth();
        int panelHeight = GetSize().GetHeight();
        
        if (!hasseEdges.empty()) {
            // Вычисляем уровни для диаграммы Хассе
            auto levels = SimplePartialOrder::CalculateHasseLevels(*vertices, hasseEdges);
            
            // Находим максимальный уровень
            int maxLevel = 0;
            for (const auto& [vertex, level] : levels) {
                if (level > maxLevel) maxLevel = level;
            }
            
            // Группируем вершины по уровням
            std::map<int, std::vector<int>> verticesByLevel;
            for (const auto& [vertex, level] : levels) {
                verticesByLevel[level].push_back(vertex);
            }
            
            // Размещаем вершины на уровнях
            int levelSpacing = (maxLevel > 0) ? panelHeight / (maxLevel + 2) : panelHeight / 2;
            
            for (const auto& [level, vertexList] : verticesByLevel) {
                int y = levelSpacing * (level + 1);
                int vertexSpacing = panelWidth / (vertexList.size() + 1);
                
                for (size_t i = 0; i < vertexList.size(); i++) {
                    int x = vertexSpacing * (i + 1);
                    vertexPositions[vertexList[i]] = wxPoint(x, y);
                    vertexColors[vertexList[i]] = *wxBLUE;
                }
            }
        } else {
            // Если нет диаграммы Хассе, размещаем по кругу
            int centerX = panelWidth / 2;
            int centerY = panelHeight / 2;
            int radius = std::min(centerX, centerY) - 50;
            
            int n = vertices->size();
            for (size_t i = 0; i < vertices->size(); i++) {
                int vertex = (*vertices)[i];
                double angle = 2 * M_PI * i / n;
                int x = centerX + radius * cos(angle);
                int y = centerY + radius * sin(angle);
                vertexPositions[vertex] = wxPoint(x, y);
                vertexColors[vertex] = *wxBLUE;
            }
        }
    }
    
    void DrawVertex(wxDC& dc, int vertex, const wxPoint& pos) {
        wxColor color = vertexColors[vertex];
        dc.SetBrush(wxBrush(color));
        dc.SetPen(wxPen(*wxBLACK, 2));
        
        // Рисуем круг
        int radius = 20;
        dc.DrawCircle(pos, radius);
        
        // Рисуем номер вершины
        dc.SetTextForeground(*wxWHITE);
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        wxString label = wxString::Format("%d", vertex);
        wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label, pos.x - textSize.GetWidth()/2, pos.y - textSize.GetHeight()/2);
    }
    
    void DrawEdge(wxDC& dc, int from, int to, double weight, bool isHasseEdge = true) {
        if (vertexPositions.find(from) == vertexPositions.end() ||
            vertexPositions.find(to) == vertexPositions.end()) {
            return;
        }
        
        wxPoint fromPos = vertexPositions[from];
        wxPoint toPos = vertexPositions[to];
        
        // Вычисляем направление
        wxPoint dir = toPos - fromPos;
        double length = sqrt(dir.x*dir.x + dir.y*dir.y);
        if (length < 1) return;
        
        // Нормализуем
        dir.x = (dir.x * 20) / length;
        dir.y = (dir.y * 20) / length;
        
        wxPoint start = fromPos + dir;
        wxPoint end = toPos - dir;
        
        // Рисуем линию
        if (isHasseEdge) {
            dc.SetPen(wxPen(wxColor(255, 0, 0), 3)); // Красные толстые линии для диаграммы Хассе
        } else {
            dc.SetPen(wxPen(*wxBLACK, 2));
        }
        dc.DrawLine(start, end);
        
        // Для диаграммы Хассе не рисуем стрелки
        // Диаграмма Хассе показывает покрывающие отношения без указания направления
    }
    
    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        Render(dc);
    }
    
    void OnEraseBackground(wxEraseEvent& event) {
        // Пустая функция для предотвращения мерцания
    }
    
    void Render(wxDC& dc) {
        // Очищаем фон
        dc.SetBrush(wxBrush(*wxWHITE));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(GetClientRect());
        
        if (!vertices || vertices->empty()) {
            dc.SetTextForeground(*wxBLACK);
            dc.SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
            wxString message = wxT("Граф пуст");
            wxSize textSize = dc.GetTextExtent(message);
            wxPoint center(GetSize().GetWidth()/2 - textSize.GetWidth()/2,
                          GetSize().GetHeight()/2 - textSize.GetHeight()/2);
            dc.DrawText(message, center);
            return;
        }
        
        // Пересчитываем позиции если нужно
        if (vertexPositions.empty()) {
            CalculateVertexPositions();
        }
        
        // Рисуем ребра диаграммы Хассе
        if (!hasseEdges.empty()) {
            for (const auto& edge : hasseEdges) {
                DrawEdge(dc, edge.from, edge.to, edge.weight, true);
            }
        }
        
        // Затем рисуем вершины
        for (const auto& pos : vertexPositions) {
            DrawVertex(dc, pos.first, pos.second);
        }
        
        // Подписываем уровни, если есть диаграмма Хассе
        if (!hasseEdges.empty()) {
            auto levels = SimplePartialOrder::CalculateHasseLevels(*vertices, hasseEdges);
            if (!levels.empty()) {
                dc.SetTextForeground(*wxBLACK);
                dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                
                for (const auto& [vertex, level] : levels) {
                    if (vertexPositions.find(vertex) != vertexPositions.end()) {
                        wxPoint pos = vertexPositions[vertex];
                        wxString levelStr = wxString::Format(wxT("Уровень %d"), level);
                        wxSize textSize = dc.GetTextExtent(levelStr);
                        dc.DrawText(levelStr, pos.x - textSize.GetWidth()/2, pos.y + 25);
                    }
                }
            }
        }
    }
    
    void OnSize(wxSizeEvent& event) {
        CalculateVertexPositions();
        Refresh();
        event.Skip();
    }
    
public:
    PartialOrderVisualizationPanel(wxWindow* parent, 
                                  std::vector<int>* verts,
                                  std::vector<Edge>* edgs,
                                  bool* directed)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE),
          vertices(verts), edges(edgs), isDirected(directed) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetDoubleBuffered(true);
        
        Bind(wxEVT_PAINT, &PartialOrderVisualizationPanel::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &PartialOrderVisualizationPanel::OnEraseBackground, this);
        Bind(wxEVT_SIZE, &PartialOrderVisualizationPanel::OnSize, this);
    }
    
    void UpdateGraph() {
        CalculateVertexPositions();
        Refresh();
    }
    
    void SetHasseEdges(const std::vector<Edge>& edges) {
        hasseEdges = edges;
        CalculateVertexPositions();
        Refresh();
    }
    
    void ClearHasseDiagram() {
        hasseEdges.clear();
        CalculateVertexPositions();
        Refresh();
    }
    
    std::vector<Edge> GetHasseEdges() const {
        return hasseEdges;
    }
};

// Главное окно
class MainWindow : public wxFrame {
private:
    // Основные элементы
    wxNotebook* notebook;
    wxNotebook* algorithmsNotebook;
    wxChoice* graphTypeChoice;
    wxTextCtrl* vertexInput;
    wxTextCtrl* vertex1Input;
    wxTextCtrl* vertex2Input;
    wxTextCtrl* startVertexInput;
    wxTextCtrl* endVertexInput;
    wxTextCtrl* weightInput;
    wxListCtrl* verticesList;
    wxListCtrl* edgesList;
    wxListCtrl* resultsList;
    wxTextCtrl* logText;
    GraphVisualizationPanel* graphVisualization;
    PartialOrderVisualizationPanel* partialOrderVisualization;
    
    // Графы
    std::unique_ptr<GraphBase<int>> currentGraph;
    bool isDirected;
    
    // Данные
    std::vector<int> vertices;
    std::vector<Edge> edges;
    
public:
    MainWindow(const wxString& title);
    
private:
    // Создание панелей
    wxPanel* CreateGraphPanel(wxWindow* parent);
    wxPanel* CreateAlgorithmsPanel(wxWindow* parent);
    wxPanel* CreateVisualizationPanel(wxWindow* parent);
    wxPanel* CreateConnectivityPanel(wxWindow* parent);
    wxPanel* CreateShortestPathPanel(wxWindow* parent);
    wxPanel* CreateTopologicalSortPanel(wxWindow* parent);
    wxPanel* CreatePartialOrderPanel(wxWindow* parent);
    
    // Обработчики событий
    void OnAddVertex(wxCommandEvent& event);
    void OnAddEdge(wxCommandEvent& event);
    void OnRemoveVertex(wxCommandEvent& event);
    void OnRemoveEdge(wxCommandEvent& event);
    void OnClearGraph(wxCommandEvent& event);
    void OnGenerateRandom(wxCommandEvent& event);
    void OnGraphTypeChanged(wxCommandEvent& event);
    void OnRefreshGraph(wxCommandEvent& event);
    
    // Алгоритмы
    void OnFindComponents(wxCommandEvent& event);
    void OnCheckConnected(wxCommandEvent& event);
    void OnCountComponents(wxCommandEvent& event);
    void OnFindLargestComponent(wxCommandEvent& event);
    void OnFindShortestPath(wxCommandEvent& event);
    void OnGetDiameter(wxCommandEvent& event);
    void OnGetRadius(wxCommandEvent& event);
    void OnFindCenter(wxCommandEvent& event);
    void OnTopologicalSort(wxCommandEvent& event);
    void OnCheckAcyclic(wxCommandEvent& event);
    void OnFindSourcesSinks(wxCommandEvent& event);
    void OnCheckPartialOrder(wxCommandEvent& event);
    void OnBuildHasse(wxCommandEvent& event);
    void OnFindMinMaxElements(wxCommandEvent& event);
    
    // Меню
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    // Вспомогательные методы
    void UpdateGraphLists();
    void LogMessage(const wxString& message);
    void ClearResults();
    void InitializeGraph();
    void ShowResultsInList(const std::vector<int>& seq, const wxString& title);
    void ShowComponentsResults(const std::vector<std::vector<int>>& components);
    void UpdateVisualization();
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(wxID_EXIT, MainWindow::OnExit)
    EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
    EVT_BUTTON(ID_AddVertex, MainWindow::OnAddVertex)
    EVT_BUTTON(ID_AddEdge, MainWindow::OnAddEdge)
    EVT_BUTTON(ID_RemoveVertex, MainWindow::OnRemoveVertex)
    EVT_BUTTON(ID_RemoveEdge, MainWindow::OnRemoveEdge)
    EVT_BUTTON(ID_ClearGraph, MainWindow::OnClearGraph)
    EVT_BUTTON(ID_GenerateRandom, MainWindow::OnGenerateRandom)
    EVT_BUTTON(ID_RefreshGraph, MainWindow::OnRefreshGraph)
    EVT_CHOICE(ID_GraphType, MainWindow::OnGraphTypeChanged)
    EVT_BUTTON(ID_FindComponents, MainWindow::OnFindComponents)
    EVT_BUTTON(ID_CheckConnected, MainWindow::OnCheckConnected)
    EVT_BUTTON(ID_CountComponents, MainWindow::OnCountComponents)
    EVT_BUTTON(ID_FindLargestComponent, MainWindow::OnFindLargestComponent)
    EVT_BUTTON(ID_FindShortestPath, MainWindow::OnFindShortestPath)
    EVT_BUTTON(ID_GetDiameter, MainWindow::OnGetDiameter)
    EVT_BUTTON(ID_GetRadius, MainWindow::OnGetRadius)
    EVT_BUTTON(ID_FindCenter, MainWindow::OnFindCenter)
    EVT_BUTTON(ID_TopologicalSort, MainWindow::OnTopologicalSort)
    EVT_BUTTON(ID_CheckAcyclic, MainWindow::OnCheckAcyclic)
    EVT_BUTTON(ID_FindSourcesSinks, MainWindow::OnFindSourcesSinks)
    EVT_BUTTON(ID_CheckPartialOrder, MainWindow::OnCheckPartialOrder)
    EVT_BUTTON(ID_BuildHasse, MainWindow::OnBuildHasse)
    EVT_BUTTON(ID_FindMinMaxElements, MainWindow::OnFindMinMaxElements)
wxEND_EVENT_TABLE()

// Конструктор главного окна
MainWindow::MainWindow(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1400, 900)),
      isDirected(false), graphVisualization(nullptr), partialOrderVisualization(nullptr) {
    
    SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
    
    // Создаем меню
    wxMenuBar* menuBar = new wxMenuBar;
    
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("Выход\tAlt-X"), wxT("Выход из приложения"));
    menuBar->Append(fileMenu, wxT("Файл"));
    
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("О программе\tF1"), wxT("Информация о программе"));
    menuBar->Append(helpMenu, wxT("Справка"));
    
    SetMenuBar(menuBar);
    
    // Создаем статус бар
    CreateStatusBar(2);
    SetStatusText(wxT("Лабораторная работа №3 - Алгоритмы на графах"), 0);
    SetStatusText(wxT("Готово"), 1);
    
    // Создаем основную панель
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Панель выбора типа графа
    wxPanel* controlPanel = new wxPanel(mainPanel, wxID_ANY);
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    
    controlSizer->Add(new wxStaticText(controlPanel, wxID_ANY, wxT("Тип графа:")), 
                      0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    
    graphTypeChoice = new wxChoice(controlPanel, ID_GraphType, wxDefaultPosition, wxSize(150, -1));
    graphTypeChoice->Append(wxT("Неориентированный"));
    graphTypeChoice->Append(wxT("Ориентированный"));
    graphTypeChoice->SetSelection(0);
    controlSizer->Add(graphTypeChoice, 0, wxRIGHT, 20);
    
    // Кнопка обновления графа
    wxButton* btnRefreshGraph = new wxButton(controlPanel, ID_RefreshGraph, wxT("Обновить граф"));
    controlSizer->Add(btnRefreshGraph, 0, wxRIGHT, 20);
    
    controlPanel->SetSizer(controlSizer);
    mainSizer->Add(controlPanel, 0, wxEXPAND | wxALL, 5);
    
    // Создаем notebook для переключения между панелями
    notebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
    
    // Добавляем вкладки
    notebook->AddPage(CreateGraphPanel(notebook), wxT("Управление графом"), true);
    notebook->AddPage(CreateAlgorithmsPanel(notebook), wxT("Алгоритмы"), false);
    notebook->AddPage(CreateVisualizationPanel(notebook), wxT("Визуализация"), false);
    
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    
    // Панель лога
    wxStaticBox* logBox = new wxStaticBox(mainPanel, wxID_ANY, wxT("Лог операций"));
    wxStaticBoxSizer* logSizer = new wxStaticBoxSizer(logBox, wxVERTICAL);
    
    logText = new wxTextCtrl(logBox, wxID_ANY, wxT(""), 
                            wxDefaultPosition, wxSize(-1, 100),
                            wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
    logSizer->Add(logText, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(logSizer, 0, wxEXPAND | wxALL, 5);
    
    mainPanel->SetSizer(mainSizer);
    
    // Инициализируем граф
    InitializeGraph();
    
    // Центрируем окно
    Centre();
}

// Инициализация графа
void MainWindow::InitializeGraph() {
    currentGraph.reset(CreateGraph<int>(isDirected));
    vertices.clear();
    edges.clear();
    UpdateGraphLists();
    LogMessage(wxT("Граф инициализирован"));
}

// Обновление списков вершин и ребер
void MainWindow::UpdateGraphLists() {
    verticesList->DeleteAllItems();
    edgesList->DeleteAllItems();
    
    // Вершины
    for (size_t i = 0; i < vertices.size(); i++) {
        long index = verticesList->InsertItem(verticesList->GetItemCount(), 
                                             wxString::Format(wxT("%d"), vertices[i]));
        verticesList->SetItem(index, 1, wxString::Format(wxT("Вершина %d"), vertices[i]));
    }
    
    // Ребра
    for (size_t i = 0; i < edges.size(); i++) {
        long index = edgesList->InsertItem(edgesList->GetItemCount(), 
                                          wxString::Format(wxT("%d -> %d"), edges[i].from, edges[i].to));
        edgesList->SetItem(index, 1, wxString::Format(wxT("%.2f"), edges[i].weight));
        
        if (isDirected) {
            edgesList->SetItem(index, 2, wxT("Ориентированное"));
        } else {
            edgesList->SetItem(index, 2, wxT("Неориентированное"));
        }
    }
    
    wxString stats = wxString::Format(wxT("Вершин: %d, Ребер: %d"), 
                                      (int)vertices.size(), (int)edges.size());
    SetStatusText(stats, 1);
    
    // Обновляем визуализацию
    UpdateVisualization();
}

void MainWindow::UpdateVisualization() {
    if (graphVisualization) {
        graphVisualization->UpdateGraph();
    }
    if (partialOrderVisualization) {
        partialOrderVisualization->UpdateGraph();
    }
}

// Запись в лог
void MainWindow::LogMessage(const wxString& message) {
    wxDateTime now = wxDateTime::Now();
    wxString timestamp = now.Format(wxT("%H:%M:%S"));
    logText->AppendText(wxString::Format(wxT("[%s] %s\n"), timestamp, message));
}

// Очистка результатов
void MainWindow::ClearResults() {
    resultsList->DeleteAllItems();
    resultsList->ClearAll();
}

// Отображение результатов в списке
void MainWindow::ShowResultsInList(const std::vector<int>& seq, const wxString& title) {
    ClearResults();
    resultsList->AppendColumn(title, wxLIST_FORMAT_LEFT, 300);
    
    for (size_t i = 0; i < seq.size(); i++) {
        resultsList->InsertItem(i, wxString::Format(wxT("%d"), seq[i]));
    }
}

// Отображение компонент связности
void MainWindow::ShowComponentsResults(const std::vector<std::vector<int>>& components) {
    ClearResults();
    resultsList->AppendColumn(wxT("Компонента"), wxLIST_FORMAT_LEFT, 150);
    resultsList->AppendColumn(wxT("Вершины"), wxLIST_FORMAT_LEFT, 300);
    resultsList->AppendColumn(wxT("Размер"), wxLIST_FORMAT_CENTER, 80);
    
    for (size_t i = 0; i < components.size(); i++) {
        wxString verticesStr;
        for (size_t j = 0; j < components[i].size(); j++) {
            verticesStr += wxString::Format(wxT("%d"), components[i][j]);
            if (j < components[i].size() - 1) verticesStr += wxT(", ");
        }
        
        long index = resultsList->InsertItem(i, wxString::Format(wxT("Компонента %d"), (int)i + 1));
        resultsList->SetItem(index, 1, verticesStr);
        resultsList->SetItem(index, 2, wxString::Format(wxT("%d"), (int)components[i].size()));
    }
}

// ПАНЕЛЬ УПРАВЛЕНИЯ ГРАФОМ
wxPanel* MainWindow::CreateGraphPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Левая панель: управление
    wxPanel* leftPanel = new wxPanel(panel);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    
    // Заголовок левой панели
    wxStaticText* leftTitle = new wxStaticText(leftPanel, wxID_ANY, wxT("Управление графом"));
    leftTitle->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    leftSizer->Add(leftTitle, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    // Добавление вершины
    wxPanel* addVertexPanel = new wxPanel(leftPanel);
    wxStaticBoxSizer* addVertexSizer = new wxStaticBoxSizer(wxVERTICAL, addVertexPanel, wxT("Добавить вершину"));
    
    wxBoxSizer* vertexRow = new wxBoxSizer(wxHORIZONTAL);
    vertexRow->Add(new wxStaticText(addVertexPanel, wxID_ANY, wxT("Вершина:")), 
                    0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    vertexInput = new wxTextCtrl(addVertexPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(80, -1));
    vertexRow->Add(vertexInput, 0, wxRIGHT, 10);
    
    wxButton* btnAddVertex = new wxButton(addVertexPanel, ID_AddVertex, wxT("Добавить"));
    vertexRow->Add(btnAddVertex, 0);
    
    addVertexSizer->Add(vertexRow, 0, wxEXPAND | wxALL, 5);
    addVertexPanel->SetSizer(addVertexSizer);
    leftSizer->Add(addVertexPanel, 0, wxEXPAND | wxALL, 5);
    
    // Добавление ребра
    wxPanel* addEdgePanel = new wxPanel(leftPanel);
    wxStaticBoxSizer* addEdgeSizer = new wxStaticBoxSizer(wxVERTICAL, addEdgePanel, wxT("Добавить ребро"));
    
    wxBoxSizer* edgeRow1 = new wxBoxSizer(wxHORIZONTAL);
    edgeRow1->Add(new wxStaticText(addEdgePanel, wxID_ANY, wxT("Из:")), 
                  0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    vertex1Input = new wxTextCtrl(addEdgePanel, ID_Vertex1, wxT(""), wxDefaultPosition, wxSize(60, -1));
    edgeRow1->Add(vertex1Input, 0, wxRIGHT, 10);
    
    edgeRow1->Add(new wxStaticText(addEdgePanel, wxID_ANY, wxT("В:")), 
                  0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    vertex2Input = new wxTextCtrl(addEdgePanel, ID_Vertex2, wxT(""), wxDefaultPosition, wxSize(60, -1));
    edgeRow1->Add(vertex2Input, 0, wxRIGHT, 10);
    
    edgeRow1->Add(new wxStaticText(addEdgePanel, wxID_ANY, wxT("Вес:")), 
                  0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    weightInput = new wxTextCtrl(addEdgePanel, wxID_ANY, wxT("1.0"), wxDefaultPosition, wxSize(60, -1));
    edgeRow1->Add(weightInput, 0);
    
    addEdgeSizer->Add(edgeRow1, 0, wxEXPAND | wxBOTTOM, 5);
    
    wxButton* btnAddEdge = new wxButton(addEdgePanel, ID_AddEdge, wxT("Добавить ребро"));
    addEdgeSizer->Add(btnAddEdge, 0, wxALIGN_CENTER | wxBOTTOM, 5);
    
    addEdgePanel->SetSizer(addEdgeSizer);
    leftSizer->Add(addEdgePanel, 0, wxEXPAND | wxALL, 5);
    
    // Управление графом
    wxPanel* managePanel = new wxPanel(leftPanel);
    wxStaticText* manageTitle = new wxStaticText(managePanel, wxID_ANY, wxT("Управление графом"));
    manageTitle->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    
    wxGridSizer* manageGrid = new wxGridSizer(2, 5, 5);
    wxButton* btnRemoveVertex = new wxButton(managePanel, ID_RemoveVertex, wxT("Удалить вершину"));
    wxButton* btnRemoveEdge = new wxButton(managePanel, ID_RemoveEdge, wxT("Удалить ребро"));
    wxButton* btnClearGraph = new wxButton(managePanel, ID_ClearGraph, wxT("Очистить граф"));
    wxButton* btnGenerateRandom = new wxButton(managePanel, ID_GenerateRandom, wxT("Случайный"));
    
    manageGrid->Add(btnRemoveVertex, 0, wxEXPAND);
    manageGrid->Add(btnRemoveEdge, 0, wxEXPAND);
    manageGrid->Add(btnClearGraph, 0, wxEXPAND);
    manageGrid->Add(btnGenerateRandom, 0, wxEXPAND);
    
    wxBoxSizer* manageSizer = new wxBoxSizer(wxVERTICAL);
    manageSizer->Add(manageTitle, 0, wxALIGN_CENTER | wxBOTTOM, 10);
    manageSizer->Add(manageGrid, 0, wxEXPAND);
    
    managePanel->SetSizer(manageSizer);
    leftSizer->Add(managePanel, 0, wxEXPAND | wxALL, 5);
    
    leftPanel->SetSizer(leftSizer);
    mainSizer->Add(leftPanel, 0, wxEXPAND | wxALL, 5);
    
    // Правая панель: списки вершин и ребер
    wxPanel* rightPanel = new wxPanel(panel);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    
    // Список вершин
    wxPanel* verticesPanel = new wxPanel(rightPanel);
    wxStaticBoxSizer* verticesSizer = new wxStaticBoxSizer(wxVERTICAL, verticesPanel, wxT("Вершины графа"));
    
    verticesList = new wxListCtrl(verticesPanel, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
                                  wxLC_REPORT | wxLC_SINGLE_SEL);
    verticesList->AppendColumn(wxT("ID"), wxLIST_FORMAT_LEFT, 80);
    verticesList->AppendColumn(wxT("Описание"), wxLIST_FORMAT_LEFT, 200);
    verticesSizer->Add(verticesList, 1, wxEXPAND | wxALL, 5);
    
    verticesPanel->SetSizer(verticesSizer);
    rightSizer->Add(verticesPanel, 1, wxEXPAND | wxALL, 5);
    
    // Список ребер
    wxPanel* edgesPanel = new wxPanel(rightPanel);
    wxStaticBoxSizer* edgesSizer = new wxStaticBoxSizer(wxVERTICAL, edgesPanel, wxT("Рёбра графа"));
    
    edgesList = new wxListCtrl(edgesPanel, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
                               wxLC_REPORT | wxLC_SINGLE_SEL);
    edgesList->AppendColumn(wxT("Ребро"), wxLIST_FORMAT_LEFT, 120);
    edgesList->AppendColumn(wxT("Вес"), wxLIST_FORMAT_CENTER, 80);
    edgesList->AppendColumn(wxT("Тип"), wxLIST_FORMAT_LEFT, 100);
    edgesSizer->Add(edgesList, 1, wxEXPAND | wxALL, 5);
    
    edgesPanel->SetSizer(edgesSizer);
    rightSizer->Add(edgesPanel, 1, wxEXPAND | wxALL, 5);
    
    rightPanel->SetSizer(rightSizer);
    mainSizer->Add(rightPanel, 1, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ АЛГОРИТМОВ
wxPanel* MainWindow::CreateAlgorithmsPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Создаем notebook для алгоритмов
    algorithmsNotebook = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
    
    // Добавляем вкладки
    algorithmsNotebook->AddPage(CreateConnectivityPanel(algorithmsNotebook), wxT("Компоненты связности"));
    algorithmsNotebook->AddPage(CreateShortestPathPanel(algorithmsNotebook), wxT("Кратчайшие пути"));
    algorithmsNotebook->AddPage(CreateTopologicalSortPanel(algorithmsNotebook), wxT("Топологическая сортировка"));
    algorithmsNotebook->AddPage(CreatePartialOrderPanel(algorithmsNotebook), wxT("Частичный порядок"));
    
    // Панель результатов
    wxPanel* resultsPanel = new wxPanel(panel);
    wxStaticBoxSizer* resultsSizer = new wxStaticBoxSizer(wxVERTICAL, resultsPanel, wxT("Результаты"));
    
    resultsList = new wxListCtrl(resultsPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, 300),
                                 wxLC_REPORT | wxLC_SINGLE_SEL);
    
    resultsSizer->Add(resultsList, 1, wxEXPAND | wxALL, 5);
    resultsPanel->SetSizer(resultsSizer);
    
    mainSizer->Add(algorithmsNotebook, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(resultsPanel, 0, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ КОМПОНЕНТ СВЯЗНОСТИ
wxPanel* MainWindow::CreateConnectivityPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, wxT("Компоненты связности (для неориентированных графов)"));
    title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);
    
    wxButton* btnFindComponents = new wxButton(panel, ID_FindComponents, wxT("Найти все компоненты"));
    wxButton* btnCheckConnected = new wxButton(panel, ID_CheckConnected, wxT("Проверить связность"));
    wxButton* btnCountComponents = new wxButton(panel, ID_CountComponents, wxT("Посчитать компоненты"));
    wxButton* btnFindLargest = new wxButton(panel, ID_FindLargestComponent, wxT("Найти наибольшую компоненту"));
    
    mainSizer->Add(btnFindComponents, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(btnCheckConnected, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(btnCountComponents, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(btnFindLargest, 0, wxEXPAND | wxALL, 5);
    mainSizer->AddStretchSpacer();
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ КРАТЧАЙШИХ ПУТЕЙ
wxPanel* MainWindow::CreateShortestPathPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, wxT("Кратчайшие пути и метрики графа"));
    title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);
    
    // Ввод вершин для поиска пути
    wxPanel* pathInputPanel = new wxPanel(panel);
    wxStaticBoxSizer* pathInputSizer = new wxStaticBoxSizer(wxVERTICAL, pathInputPanel, wxT("Поиск кратчайшего пути"));
    
    wxBoxSizer* inputRow = new wxBoxSizer(wxHORIZONTAL);
    inputRow->Add(new wxStaticText(pathInputPanel, wxID_ANY, wxT("Из:")), 
                  0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    startVertexInput = new wxTextCtrl(pathInputPanel, ID_StartVertex, wxT(""), wxDefaultPosition, wxSize(60, -1));
    inputRow->Add(startVertexInput, 0, wxRIGHT, 15);
    
    inputRow->Add(new wxStaticText(pathInputPanel, wxID_ANY, wxT("В:")), 
                  0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    endVertexInput = new wxTextCtrl(pathInputPanel, ID_EndVertex, wxT(""), wxDefaultPosition, wxSize(60, -1));
    inputRow->Add(endVertexInput, 0);
    
    pathInputSizer->Add(inputRow, 0, wxALIGN_CENTER | wxALL, 10);
    
    wxButton* btnFindPath = new wxButton(pathInputPanel, ID_FindShortestPath, wxT("Найти кратчайший путь"));
    pathInputSizer->Add(btnFindPath, 0, wxALIGN_CENTER | wxALL, 5);
    
    pathInputPanel->SetSizer(pathInputSizer);
    mainSizer->Add(pathInputPanel, 0, wxEXPAND | wxALL, 5);
    
    // Метрики графа
    wxPanel* metricsPanel = new wxPanel(panel);
    wxStaticBoxSizer* metricsSizer = new wxStaticBoxSizer(wxVERTICAL, metricsPanel, wxT("Метрики графа"));
    
    wxButton* btnGetDiameter = new wxButton(metricsPanel, ID_GetDiameter, wxT("Диаметр графа"));
    wxButton* btnGetRadius = new wxButton(metricsPanel, ID_GetRadius, wxT("Радиус графа"));
    wxButton* btnFindCenter = new wxButton(metricsPanel, ID_FindCenter, wxT("Центр графа"));
    
    metricsSizer->Add(btnGetDiameter, 0, wxEXPAND | wxALL, 5);
    metricsSizer->Add(btnGetRadius, 0, wxEXPAND | wxALL, 5);
    metricsSizer->Add(btnFindCenter, 0, wxEXPAND | wxALL, 5);
    
    metricsPanel->SetSizer(metricsSizer);
    mainSizer->Add(metricsPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->AddStretchSpacer();
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ ТОПОЛОГИЧЕСКОЙ СОРТИРОВКИ
wxPanel* MainWindow::CreateTopologicalSortPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, wxT("Топологическая сортировка (для ориентированных графов)"));
    title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);
    
    wxButton* btnTopologicalSort = new wxButton(panel, ID_TopologicalSort, wxT("Топологическая сортировка"));
    wxButton* btnCheckAcyclic = new wxButton(panel, ID_CheckAcyclic, wxT("Проверить ацикличность"));
    wxButton* btnFindSourcesSinks = new wxButton(panel, ID_FindSourcesSinks, wxT("Найти источники/стоки"));
    
    mainSizer->Add(btnTopologicalSort, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(btnCheckAcyclic, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(btnFindSourcesSinks, 0, wxEXPAND | wxALL, 5);
    mainSizer->AddStretchSpacer();
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ ЧАСТИЧНОГО ПОРЯДКА
wxPanel* MainWindow::CreatePartialOrderPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Левая панель: кнопки управления
    wxPanel* leftPanel = new wxPanel(panel);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(leftPanel, wxID_ANY, wxT("Частичный порядок (для ориентированных графов)"));
    title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    leftSizer->Add(title, 0, wxALIGN_CENTER | wxALL, 10);
    
    wxButton* btnCheckPartialOrder = new wxButton(leftPanel, ID_CheckPartialOrder, wxT("Проверить частичный порядок"));
    wxButton* btnBuildHasse = new wxButton(leftPanel, ID_BuildHasse, wxT("Построить диаграмму Хассе"));
    wxButton* btnFindMinMax = new wxButton(leftPanel, ID_FindMinMaxElements, wxT("Найти мин/макс элементы"));
    
    leftSizer->Add(btnCheckPartialOrder, 0, wxEXPAND | wxALL, 5);
    leftSizer->Add(btnBuildHasse, 0, wxEXPAND | wxALL, 5);
    leftSizer->Add(btnFindMinMax, 0, wxEXPAND | wxALL, 5);
    leftSizer->AddStretchSpacer();
    
    leftPanel->SetSizer(leftSizer);
    mainSizer->Add(leftPanel, 0, wxEXPAND | wxALL, 5);
    
    // Правая панель: визуализация
    wxPanel* rightPanel = new wxPanel(panel);
    wxStaticBoxSizer* rightSizer = new wxStaticBoxSizer(wxVERTICAL, rightPanel, wxT("Диаграмма Хассе"));
    
    partialOrderVisualization = new PartialOrderVisualizationPanel(rightPanel, &vertices, &edges, &isDirected);
    rightSizer->Add(partialOrderVisualization, 1, wxEXPAND | wxALL, 5);
    
    rightPanel->SetSizer(rightSizer);
    mainSizer->Add(rightPanel, 1, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ПАНЕЛЬ ВИЗУАЛИЗАЦИИ
wxPanel* MainWindow::CreateVisualizationPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Создаем панель для визуализации графа
    graphVisualization = new GraphVisualizationPanel(panel, &vertices, &edges, &isDirected);
    mainSizer->Add(graphVisualization, 1, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(mainSizer);
    return panel;
}

// ОБРАБОТЧИКИ СОБЫТИЙ МЕНЮ
void MainWindow::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainWindow::OnAbout(wxCommandEvent& event) {
    wxMessageBox(
        wxT("Лабораторная работа №3 - Алгоритмы на графах\n\n")
        wxT("Функции программы:\n")
        wxT("1. Создание и редактирование графов (ориентированных и неориентированных)\n")
        wxT("2. Визуализация графов\n")
        wxT("3. Алгоритмы на графах:\n")
        wxT("   - Компоненты связности\n")
        wxT("   - Кратчайшие пути\n")
        wxT("   - Топологическая сортировка\n")
        wxT("   - Частичные порядки и диаграммы Хассе\n\n"),
        wxT("О программе"),
        wxOK | wxICON_INFORMATION,
        this
    );
}

// ОБРАБОТЧИКИ СОБЫТИЙ УПРАВЛЕНИЯ ГРАФОМ

void MainWindow::OnAddVertex(wxCommandEvent& event) {
    wxString input = vertexInput->GetValue();
    if (input.IsEmpty()) {
        wxMessageBox(wxT("Введите номер вершины!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    long vertex;
    if (!input.ToLong(&vertex)) {
        wxMessageBox(wxT("Номер вершины должен быть числом!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    
    // Проверяем, существует ли уже такая вершина
    if (std::find(vertices.begin(), vertices.end(), vertex) != vertices.end()) {
        wxMessageBox(wxString::Format(wxT("Вершина %d уже существует!"), vertex), 
                    wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    vertices.push_back(vertex);
    currentGraph->AddVertex(vertex);
    UpdateGraphLists();
    vertexInput->Clear();
    
    LogMessage(wxString::Format(wxT("Добавлена вершина: %d"), vertex));
}

void MainWindow::OnAddEdge(wxCommandEvent& event) {
    wxString fromStr = vertex1Input->GetValue();
    wxString toStr = vertex2Input->GetValue();
    wxString weightStr = weightInput->GetValue();
    
    if (fromStr.IsEmpty() || toStr.IsEmpty()) {
        wxMessageBox(wxT("Введите номера вершин!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    long from, to;
    double weight = 1.0;
    
    if (!fromStr.ToLong(&from) || !toStr.ToLong(&to)) {
        wxMessageBox(wxT("Номера вершин должны быть числами!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    
    if (!weightStr.ToDouble(&weight)) {
        weight = 1.0;
    }
    
    // Проверяем существование вершин
    if (std::find(vertices.begin(), vertices.end(), from) == vertices.end()) {
        currentGraph->AddVertex(from);
        vertices.push_back(from);
    }
    
    if (std::find(vertices.begin(), vertices.end(), to) == vertices.end()) {
        currentGraph->AddVertex(to);
        vertices.push_back(to);
    }
    
    // Добавляем ребро
    try {
        currentGraph->AddEdge(from, to, weight);
        edges.push_back(Edge(from, to, weight));
        
        // Для неориентированного графа добавляем обратное ребро
        if (!isDirected) {
            edges.push_back(Edge(to, from, weight));
        }
        
        UpdateGraphLists();
        vertex1Input->Clear();
        vertex2Input->Clear();
        weightInput->SetValue(wxT("1.0"));
        
        LogMessage(wxString::Format(wxT("Добавлено ребро: %d -> %d (вес: %.2f)"), from, to, weight));
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при добавлении ребра: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnRemoveVertex(wxCommandEvent& event) {
    wxString input = wxGetTextFromUser(wxT("Введите номер вершины для удаления:"), 
                                       wxT("Удаление вершины"));
    if (input.IsEmpty()) return;
    
    long vertex;
    if (!input.ToLong(&vertex)) {
        wxMessageBox(wxT("Номер вершины должен быть числом!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    
    // Удаляем из вектора вершин
    auto it = std::find(vertices.begin(), vertices.end(), vertex);
    if (it == vertices.end()) {
        wxMessageBox(wxString::Format(wxT("Вершина %d не найдена!"), vertex), 
                    wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    vertices.erase(it);
    
    // Удаляем все связанные ребра
    edges.erase(std::remove_if(edges.begin(), edges.end(),
        [vertex](const Edge& edge) {
            return edge.from == vertex || edge.to == vertex;
        }), edges.end());
    
    // Удаляем из графа
    try {
        currentGraph->RemoveVertex(vertex);
        UpdateGraphLists();
        LogMessage(wxString::Format(wxT("Удалена вершина: %d"), vertex));
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при удалении вершины: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnRemoveEdge(wxCommandEvent& event) {
    wxString fromStr = wxGetTextFromUser(wxT("Введите начальную вершину:"), 
                                        wxT("Удаление ребра"));
    if (fromStr.IsEmpty()) return;
    
    wxString toStr = wxGetTextFromUser(wxT("Введите конечную вершину:"), 
                                      wxT("Удаление ребра"));
    if (toStr.IsEmpty()) return;
    
    long from, to;
    if (!fromStr.ToLong(&from) || !toStr.ToLong(&to)) {
        wxMessageBox(wxT("Номера вершин должны быть числами!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    
    // Ищем и удаляем ребро
    bool found = false;
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        if (it->from == from && it->to == to) {
            edges.erase(it);
            found = true;
            break;
        }
    }
    
    if (!found) {
        wxMessageBox(wxString::Format(wxT("Ребро %d -> %d не найдено!"), from, to), 
                    wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    // Удаляем из графа
    try {
        currentGraph->RemoveEdge(from, to);
        UpdateGraphLists();
        LogMessage(wxString::Format(wxT("Удалено ребро: %d -> %d"), from, to));
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при удалении ребра: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnClearGraph(wxCommandEvent& event) {
    wxMessageDialog dialog(this,
        wxT("Вы действительно хотите очистить граф?"),
        wxT("Подтверждение"),
        wxYES_NO | wxICON_QUESTION);
    
    // Устанавливаем русские надписи на кнопках
    dialog.SetYesNoLabels(wxT("Да"), wxT("Нет"));
    
    int answer = dialog.ShowModal();
    if (answer != wxID_YES) return;
    
    InitializeGraph();
    LogMessage(wxT("Граф очищен"));
}

void MainWindow::OnGenerateRandom(wxCommandEvent& event) {
    wxNumberEntryDialog vertexDialog(this, 
        wxT("Введите количество вершин:"), 
        wxT("Случайный граф"), 
        wxT("Количество вершин"), 10, 1, 100);
    
    if (vertexDialog.ShowModal() != wxID_OK) return;
    int vertexCount = vertexDialog.GetValue();
    
    wxNumberEntryDialog edgeDialog(this, 
        wxT("Введите вероятность ребра (1-100):"), 
        wxT("Случайный граф"), 
        wxT("Вероятность %"), 30, 1, 100);
    
    if (edgeDialog.ShowModal() != wxID_OK) return;
    int edgeProbability = edgeDialog.GetValue();
    
    // Очищаем текущий граф
    InitializeGraph();
    
    // Добавляем вершины
    for (int i = 1; i <= vertexCount; i++) {
        vertices.push_back(i);
        currentGraph->AddVertex(i);
    }
    
    // Добавляем случайные ребра
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 1; i <= vertexCount; i++) {
        for (int j = i + 1; j <= vertexCount; j++) {
            if (std::rand() % 100 < edgeProbability) {
                double weight = 1.0 + (std::rand() % 100) / 10.0;
                
                currentGraph->AddEdge(i, j, weight);
                edges.push_back(Edge(i, j, weight));
                
                if (!isDirected) {
                    // Для неориентированного графа добавляем обратное ребро
                    edges.push_back(Edge(j, i, weight));
                }
            }
        }
    }
    
    UpdateGraphLists();
    
    LogMessage(wxString::Format(wxT("Сгенерирован случайный граф: %d вершин, %d ребер"), 
                                vertexCount, (int)edges.size()));
}

void MainWindow::OnGraphTypeChanged(wxCommandEvent& event) {
    int selection = graphTypeChoice->GetSelection();
    bool newIsDirected = (selection == 1);
    
    if (newIsDirected != isDirected) {
        wxMessageDialog dialog(this,
            wxT("Изменение типа графа приведет к потере текущих данных. Продолжить?"),
            wxT("Подтверждение"),
            wxYES_NO | wxICON_QUESTION);
        
        // Устанавливаем русские надписи на кнопках
        dialog.SetYesNoLabels(wxT("Да"), wxT("Нет"));
        
        int answer = dialog.ShowModal();
        
        if (answer == wxID_YES) {
            isDirected = newIsDirected;
            InitializeGraph();
            LogMessage(wxString::Format(wxT("Тип графа изменен на: %s"), 
                        isDirected ? wxT("ориентированный") : wxT("неориентированный")));
        } else {
            graphTypeChoice->SetSelection(isDirected ? 1 : 0);
        }
    }
}

void MainWindow::OnRefreshGraph(wxCommandEvent& event) {
    if (graphVisualization) {
        graphVisualization->ClearSelection();
        graphVisualization->ClearHasseDiagram();
        LogMessage(wxT("Визуализация графа обновлена"));
    }
    if (partialOrderVisualization) {
        partialOrderVisualization->ClearHasseDiagram();
    }
}

// АЛГОРИТМЫ

void MainWindow::OnFindComponents(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (isDirected) {
            wxMessageBox(wxT("Компоненты связности определены только для неориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto components = SimpleGraphAlgorithms::FindConnectedComponents(vertices, edges);
        
        ShowComponentsResults(components);
        LogMessage(wxT("Найдены компоненты связности (BFS)"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при поиске компонент связности: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnCheckConnected(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (isDirected) {
            wxMessageBox(wxT("Проверка связности определена только для неориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        bool isConnected = SimpleGraphAlgorithms::IsConnected(vertices, edges);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Результат проверки связности"), wxLIST_FORMAT_LEFT, 300);
        
        if (isConnected) {
            resultsList->InsertItem(0, wxT("Граф является связным"));
        } else {
            resultsList->InsertItem(0, wxT("Граф НЕ является связным"));
        }
        
        LogMessage(wxString::Format(wxT("Проверка связности: %s"), 
                    isConnected ? wxT("связный") : wxT("несвязный")));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при проверке связности: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnCountComponents(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (isDirected) {
            wxMessageBox(wxT("Подсчет компонент определен только для неориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        int count = SimpleGraphAlgorithms::CountComponents(vertices, edges);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Количество компонент связности"), wxLIST_FORMAT_LEFT, 300);
        resultsList->InsertItem(0, wxString::Format(wxT("Количество компонент связности: %d"), count));
        
        LogMessage(wxString::Format(wxT("Подсчитано компонент связности: %d"), count));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при подсчете компонент: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnFindLargestComponent(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (isDirected) {
            wxMessageBox(wxT("Поиск наибольшей компоненты определен только для неориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto largestComponent = SimpleGraphAlgorithms::FindLargestComponent(vertices, edges);
        
        ShowResultsInList(largestComponent, wxT("Наибольшая компонента связности"));
        LogMessage(wxT("Найдена наибольшая компонента связности"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при поиске наибольшей компоненты: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnFindShortestPath(wxCommandEvent& event) {
    wxString startStr = startVertexInput->GetValue();
    wxString endStr = endVertexInput->GetValue();
    
    if (startStr.IsEmpty() || endStr.IsEmpty()) {
        wxMessageBox(wxT("Введите начальную и конечную вершины!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    long start, end;
    if (!startStr.ToLong(&start) || !endStr.ToLong(&end)) {
        wxMessageBox(wxT("Номера вершин должны быть числами!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
        return;
    }
    
    try {
        // Проверяем существование вершин
        if (std::find(vertices.begin(), vertices.end(), start) == vertices.end() ||
            std::find(vertices.begin(), vertices.end(), end) == vertices.end()) {
            wxMessageBox(wxT("Одна или обе вершины не существуют в графе!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        // Используем нашу простую реализацию Дейкстры
        auto path = SimpleGraphAlgorithms::Dijkstra(vertices, edges, start, end, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Кратчайший путь"), wxLIST_FORMAT_LEFT, 300);
        
        if (path.empty()) {
            resultsList->InsertItem(0, wxString::Format(wxT("Путь из %d в %d не существует"), start, end));
            LogMessage(wxString::Format(wxT("Путь из %d в %d не существует"), start, end));
        } else {
            wxString pathStr = wxT("Путь: ");
            for (size_t i = 0; i < path.size(); i++) {
                pathStr += wxString::Format(wxT("%d"), path[i]);
                if (i < path.size() - 1) pathStr += wxT(" -> ");
            }
            resultsList->InsertItem(0, pathStr);
            
            // Показываем путь на визуализации
            if (graphVisualization) {
                graphVisualization->SetSelectedPath(path);
            }
            
            // Вычисляем длину пути
            double pathLength = 0.0;
            for (size_t i = 0; i < path.size() - 1; i++) {
                // Ищем вес ребра
                for (const auto& edge : edges) {
                    if (edge.from == path[i] && edge.to == path[i + 1]) {
                        pathLength += edge.weight;
                        break;
                    }
                }
            }
            resultsList->InsertItem(1, wxString::Format(wxT("Длина пути: %.2f"), pathLength));
            
            LogMessage(wxString::Format(wxT("Найден кратчайший путь из %d в %d, длина: %.2f"), 
                        start, end, pathLength));
        }
        
    } catch (const std::exception& e) {
        wxString errorMsg = wxString(wxT("Ошибка при поиске кратчайшего пути: ")) + e.what();
        wxMessageBox(errorMsg, wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnGetDiameter(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        double diameter = SimpleGraphAlgorithms::GetGraphDiameter(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Диаметр графа"), wxLIST_FORMAT_LEFT, 300);
        resultsList->InsertItem(0, wxString::Format(wxT("Диаметр графа: %.2f"), diameter));
        
        LogMessage(wxString::Format(wxT("Вычислен диаметр графа: %.2f"), diameter));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при вычислении диаметра: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnGetRadius(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        double radius = SimpleGraphAlgorithms::GetGraphRadius(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Радиус графа"), wxLIST_FORMAT_LEFT, 300);
        resultsList->InsertItem(0, wxString::Format(wxT("Радиус графа: %.2f"), radius));
        
        LogMessage(wxString::Format(wxT("Вычислен радиус графа: %.2f"), radius));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при вычислении радиуса: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnFindCenter(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        auto centerVertices = SimpleGraphAlgorithms::FindGraphCenter(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Центр графа"), wxLIST_FORMAT_LEFT, 300);
        
        if (centerVertices.empty()) {
            resultsList->InsertItem(0, wxT("Центр графа не найден"));
        } else {
            wxString centerStr = wxT("Центр графа: ");
            for (size_t i = 0; i < centerVertices.size(); i++) {
                centerStr += wxString::Format(wxT("%d"), centerVertices[i]);
                if (i < centerVertices.size() - 1) centerStr += wxT(", ");
            }
            resultsList->InsertItem(0, centerStr);
        }
        
        LogMessage(wxT("Найден центр графа"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при поиске центра графа: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnTopologicalSort(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Топологическая сортировка определена только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto sorted = SimpleGraphAlgorithms::TopologicalSortKahn(vertices, edges, isDirected);
        
        if (sorted.empty()) {
            ClearResults();
            resultsList->AppendColumn(wxT("Топологическая сортировка"), wxLIST_FORMAT_LEFT, 300);
            resultsList->InsertItem(0, wxT("Граф содержит циклы! Топологическая сортировка невозможна."));
            LogMessage(wxT("Граф содержит циклы, топологическая сортировка невозможна"));
        } else {
            ShowResultsInList(sorted, wxT("Топологическая сортировка"));
            LogMessage(wxT("Выполнена топологическая сортировка"));
        }
        
    } catch (const std::exception& e) {
        wxString errorMsg = wxString(wxT("Ошибка при топологической сортировке: ")) + e.what();
        wxMessageBox(errorMsg, wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnCheckAcyclic(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Проверка ацикличности определена только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        bool isAcyclic = SimpleGraphAlgorithms::IsAcyclic(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Проверка ацикличности"), wxLIST_FORMAT_LEFT, 300);
        
        if (isAcyclic) {
            resultsList->InsertItem(0, wxT("Граф является ациклическим"));
        } else {
            resultsList->InsertItem(0, wxT("Граф содержит циклы"));
        }
        
        LogMessage(wxString::Format(wxT("Проверка ацикличности: %s"), 
                    isAcyclic ? wxT("ациклический") : wxT("содержит циклы")));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при проверке ацикличности: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnFindSourcesSinks(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Поиск источников/стоков определен только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto [sources, sinks] = SimpleGraphAlgorithms::FindSourcesAndSinks(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Источники и стоки"), wxLIST_FORMAT_LEFT, 300);
        
        wxString sourcesStr = wxT("Источники: ");
        if (sources.empty()) {
            sourcesStr += wxT("нет");
        } else {
            for (size_t i = 0; i < sources.size(); i++) {
                sourcesStr += wxString::Format(wxT("%d"), sources[i]);
                if (i < sources.size() - 1) sourcesStr += wxT(", ");
            }
        }
        resultsList->InsertItem(0, sourcesStr);
        
        wxString sinksStr = wxT("Стоки: ");
        if (sinks.empty()) {
            sinksStr += wxT("нет");
        } else {
            for (size_t i = 0; i < sinks.size(); i++) {
                sinksStr += wxString::Format(wxT("%d"), sinks[i]);
                if (i < sinks.size() - 1) sinksStr += wxT(", ");
            }
        }
        resultsList->InsertItem(1, sinksStr);
        
        LogMessage(wxT("Найдены источники и стоки графа"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при поиске источников/стоков: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnCheckPartialOrder(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Проверка частичного порядка определена только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        bool isPartialOrder = SimplePartialOrder::IsPartialOrder(vertices, edges, isDirected);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Проверка частичного порядка"), wxLIST_FORMAT_LEFT, 300);
        
        if (isPartialOrder) {
            resultsList->InsertItem(0, wxT("Граф является отношением частичного порядка"));
        } else {
            resultsList->InsertItem(0, wxT("Граф НЕ является отношением частичного порядка"));
        }
        
        LogMessage(wxString::Format(wxT("Проверка частичного порядка: %s"), 
                    isPartialOrder ? wxT("является") : wxT("не является")));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при проверке частичного порядка: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnBuildHasse(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Диаграмма Хассе определена только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        // Проверяем свойства частичного порядка более тщательно
        bool hasReflexivity = true;
        bool hasAntisymmetry = true;
        bool hasTransitivity = true;
        
        // 1. Антисимметричность
        for (const auto& edge1 : edges) {
            for (const auto& edge2 : edges) {
                if (edge1.from == edge2.to && edge1.to == edge2.from) {
                    if (edge1.from != edge1.to) { // Исключаем петли
                        hasAntisymmetry = false;
                        break;
                    }
                }
            }
            if (!hasAntisymmetry) break;
        }
        
        // 2. Проверка на ацикличность
        if (!SimpleGraphAlgorithms::IsAcyclic(vertices, edges, isDirected)) {
            wxMessageBox(wxT("Граф содержит циклы! Не является частичным порядком."), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        if (!hasAntisymmetry) {
            wxMessageBox(wxT("Граф не антисимметричен! Не является частичным порядком."), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto hasseEdges = SimplePartialOrder::BuildHasseDiagram(vertices, edges);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Диаграмма Хассе"), wxLIST_FORMAT_LEFT, 300);
        
        if (hasseEdges.empty()) {
            resultsList->InsertItem(0, wxT("Диаграмма Хассе пуста или граф состоит из несравнимых элементов"));
        } else {
            for (size_t i = 0; i < hasseEdges.size(); i++) {
                wxString edgeStr = wxString::Format(wxT("%d → %d"), 
                    hasseEdges[i].from, hasseEdges[i].to);
                resultsList->InsertItem(i, edgeStr);
            }
        }
        
        // Показываем диаграмму Хассе
        if (partialOrderVisualization) {
            partialOrderVisualization->SetHasseEdges(hasseEdges);
        }
        
        // Также показываем на основной визуализации
        if (graphVisualization) {
            graphVisualization->SetHasseEdges(hasseEdges);
        }
        
        LogMessage(wxT("Построена диаграмма Хассе"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при построении диаграммы Хассе: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnFindMinMaxElements(wxCommandEvent& event) {
    if (vertices.empty()) {
        wxMessageBox(wxT("Граф пуст!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        if (!isDirected) {
            wxMessageBox(wxT("Поиск минимальных/максимальных элементов определен только для ориентированных графов!"), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        auto minimal = SimplePartialOrder::FindMinimalElements(vertices, edges);
        auto maximal = SimplePartialOrder::FindMaximalElements(vertices, edges);
        
        ClearResults();
        resultsList->AppendColumn(wxT("Минимальные и максимальные элементы"), wxLIST_FORMAT_LEFT, 300);
        
        wxString minimalStr = wxT("Минимальные элементы: ");
        if (minimal.empty()) {
            minimalStr += wxT("нет");
        } else {
            for (size_t i = 0; i < minimal.size(); i++) {
                minimalStr += wxString::Format(wxT("%d"), minimal[i]);
                if (i < minimal.size() - 1) minimalStr += wxT(", ");
            }
        }
        resultsList->InsertItem(0, minimalStr);
        
        wxString maximalStr = wxT("Максимальные элементы: ");
        if (maximal.empty()) {
            maximalStr += wxT("нет");
        } else {
            for (size_t i = 0; i < maximal.size(); i++) {
                maximalStr += wxString::Format(wxT("%d"), maximal[i]);
                if (i < maximal.size() - 1) maximalStr += wxT(", ");
            }
        }
        resultsList->InsertItem(1, maximalStr);
        
        LogMessage(wxT("Найдены минимальные и максимальные элементы"));
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при поиске мин/макс элементов: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

// ТОЧКА ВХОДА
class GraphLabApp : public wxApp {
public:
    virtual bool OnInit() {
        // Инициализация локали для поддержки русских символов
        wxLocale::AddCatalogLookupPathPrefix(wxT("."));
        wxLocale* locale = new wxLocale(wxLANGUAGE_RUSSIAN);
        locale->AddCatalog(wxT("graphlab_app"));
        wxInitAllImageHandlers();
        
        MainWindow* frame = new MainWindow(wxT("Лабораторная работа №3 - Алгоритмы на графах"));
        frame->Show(true);
        
        return true;
    }
};

wxIMPLEMENT_APP(GraphLabApp);