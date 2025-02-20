#include <iostream>
#include <fstream>
#include <set>
#include <string>

using namespace std;

const int MAX_CITIES = 10; // Максимальное количество городов

// Класс для представления графа
class Graph {
private:
    string cities[MAX_CITIES]; // Список городов
    string adjacencyList[MAX_CITIES][MAX_CITIES]; // Массив для хранения смежных городов
    int cityCount; // Текущее количество городов

public:
    Graph() : cityCount(0) {}

    // Добавление города
    int addCity(const string& city) {
        if (cityCount >= MAX_CITIES) return -1; // Превышено максимальное количество городов
        cities[cityCount] = city;
        return cityCount++; // Возвращаем индекс добавленного города
    }

    // Получение индекса города
    int getCityIndex(const string& city) const {
        for (int i = 0; i < cityCount; ++i) {
            if (cities[i] == city) { ; return i; }
        }
        return -1; // Город не найден
    }

    // Получение названия города по индексу
    string getCityName(int index) const {
        if (index >= 0 && index < cityCount) {
            return cities[index];
        }
        return ""; // Индекс вне диапазона
    }

    // Получение общего количества городов
    int getCityCount() const {
        return cityCount;
    }

    // Добавление ребра в граф
    void addEdge(const string& from, const string& to) {
        int fromIndex = getCityIndex(from);
        int toIndex = getCityIndex(to);

        if (fromIndex != -1 && toIndex != -1) {
            // Добавляем соседа
            bool found = false;
            for (int i = 0; i < cityCount; ++i) {
                if (adjacencyList[fromIndex][i].empty()) {
                    adjacencyList[fromIndex][i] = to;
                    found = true;
                    break;
                }
            }
        }
    }

    // Получение соседей города
    string getNeighbors(int cityIndex) const {
        string neighbors = "";
        for (int i = 0; i < cityCount; ++i) {
            if (!adjacencyList[cityIndex][i].empty()) {
                if (!neighbors.empty()) neighbors += " ";
                neighbors += adjacencyList[cityIndex][i];
            }
        }
        return neighbors;
    }

    // Вывод графа
    void printGraph() const {
        cout << "Граф:" << endl;
        for (int i = 0; i < cityCount; ++i) {
            cout << cities[i] << " -> " << getNeighbors(i) << endl;
        }
    }
};

// Класс для работы с маршрутами
class RouteFinder {
private:
    const Graph& graph;
    int startCityIndex;
    int endCityIndex;

public:
    // Конструктор
    RouteFinder(const Graph& g, const string& start, const string& end)
        : graph(g), startCityIndex(g.getCityIndex(start)), endCityIndex(g.getCityIndex(end)) {}

    // Рекурсивный поиск всех маршрутов
    void findAllPaths(string allPaths[][MAX_CITIES], int& pathCount, string currentPath[], int pathSize, set<int>& visited, int currentCityIndex) const {
        currentPath[pathSize++] = graph.getCityName(currentCityIndex);
        visited.insert(currentCityIndex);

        if (currentCityIndex == endCityIndex) {
            allPaths[pathCount][0] = graph.getCityName(currentCityIndex); // Сохраняем маршрут
            for (int i = 1; i < pathSize; ++i) {
                allPaths[pathCount][i] = currentPath[i - 1];
            }
            pathCount++;
        }
        else {
            string neighbors = graph.getNeighbors(currentCityIndex);
            if (!neighbors.empty()) {
                string city;
                int pos = 0;
                while ((pos = neighbors.find(" ", pos)) != string::npos) {
                    city = neighbors.substr(0, pos);
                    int neighborIndex = graph.getCityIndex(city);
                    if (visited.find(neighborIndex) == visited.end()) {
                        findAllPaths(allPaths, pathCount, currentPath, pathSize, visited, neighborIndex);
                    }
                    neighbors = neighbors.substr(pos + 1);
                }
                if (!neighbors.empty()) {
                    city = neighbors;
                    int neighborIndex = graph.getCityIndex(city);
                    if (visited.find(neighborIndex) == visited.end()) {
                        findAllPaths(allPaths, pathCount, currentPath, pathSize, visited, neighborIndex);
                    }
                }
            }
        }

        pathSize--;
        visited.erase(currentCityIndex);
    }

    // Поиск всех возможных маршрутов
    string getAllRoutes() const {
        string allPaths[MAX_CITIES][MAX_CITIES] = { "" };
        string currentPath[MAX_CITIES];
        set<int> visited;
        int pathCount = 0;

        findAllPaths(allPaths, pathCount, currentPath, 0, visited, startCityIndex);

        string result = "";
        for (int i = 0; i < pathCount; ++i) {
            result += to_string(i + 1) + ". ";
            for (int j = 0; j < MAX_CITIES && !allPaths[i][j].empty(); ++j) {
                result += allPaths[i][j] + " -> ";
            }
            result += "\n";
        }

        return result + "\nКоличество маршрутов: " + to_string(pathCount);
    }
};

// Функция для чтения данных из файла
pair<string, string> readCitiesFromFile(const string& filename, Graph& graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла!" << endl;
        exit(1);
    }

    int n;
    file >> n; // Количество городов

    string start, end;
    file >> start >> end; // Начальный и конечный города

    // Множество для уникальных городов
    set<string> uniqueCities = { start, end };

    // Считываем все пары городов и добавляем их в множество
    string x, y;
    while (file >> x >> y) {
        uniqueCities.insert(x);
        uniqueCities.insert(y);
    }

    file.clear(); // Сбрасываем состояние потока
    file.seekg(0); // Возвращаемся в начало файла

    // Добавляем уникальные города в граф
    for (const string& city : uniqueCities) {
        graph.addCity(city);
    }

    // Добавляем ребра в граф
    file >> n; // Пропускаем количество городов
    file >> start >> end; // Пропускаем начальный и конечный города
    while (file >> x >> y) {
        graph.addEdge(x, y);
    }

    file.close();
    return { start, end };
}

int main() {
    setlocale(LC_ALL, "Russian");
    string filename = "INPUT.TXT";
    Graph graph;
    pair<string, string> cities = readCitiesFromFile(filename, graph);

    string startCity = cities.first; // Начальный город
    string endCity = cities.second;  // Конечный город

    // Вывод графа
    graph.printGraph();

    // Поиск всех маршрутов
    RouteFinder routeFinder(graph, startCity, endCity);
    cout << "\n" << routeFinder.getAllRoutes() << endl;

    return 0;
}