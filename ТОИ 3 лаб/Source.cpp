#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

const int NUM_CITIES = 5; // Количество городов
const int NUM_ANTS = 10; // Количество муравьев
const double ALPHA = 1.0; // Параметр влияния феромона
const double BETA = 2.0; // Параметр влияния длины пути
const double RHO = 0.5; // Коэффициент испарения феромона
const double Q = 100.0; // Количество феромона, оставляемое на ребре

double distanceMatrix[NUM_CITIES][NUM_CITIES]; // Матрица расстояний между городами
double pheromoneMatrix[NUM_CITIES][NUM_CITIES]; // Матрица феромонов на рёбрах
double bestTourLength = numeric_limits<double>::max(); // Длина лучшего найденного пути

// Структура для представления муравья
struct Ant {
    vector<int> tour; // Текущий маршрут муравья
    vector<bool> visited; // Посещённые города
};

// Функция для инициализации матрицы расстояний между городами
void initializeDistanceMatrix() {
    // В этом примере просто случайно генерируем матрицу расстояний
    for (int i = 0; i < NUM_CITIES; ++i) {
        for (int j = 0; j < NUM_CITIES; ++j) {
            if (i != j) {
                distanceMatrix[i][j] = rand() % 100 + 1; // Генерируем случайное расстояние от 1 до 100
            }
            else {
                distanceMatrix[i][j] = 0; // Расстояние от города до самого себя равно 0
            }
        }
    }
}

// Функция для инициализации матрицы феромонов
void initializePheromoneMatrix() {
    // Инициализируем все значения в матрице феромонов начальным значением
    for (int i = 0; i < NUM_CITIES; ++i) {
        for (int j = 0; j < NUM_CITIES; ++j) {
            pheromoneMatrix[i][j] = 1.0; // Начальное значение феромона
        }
    }
}

// Функция для расчёта вероятностей перехода от одного города к другому
vector<double> calculateProbabilities(int currentCity, const Ant& ant) {
    vector<double> probabilities(NUM_CITIES, 0.0);
    double total = 0.0;

    // Считаем сумму (tau^alpha * eta^beta) для всех возможных городов
    for (int i = 0; i < NUM_CITIES; ++i) {
        if (!ant.visited[i]) {
            probabilities[i] = pow(pheromoneMatrix[currentCity][i], ALPHA) * pow(1.0 / distanceMatrix[currentCity][i], BETA);
            total += probabilities[i];
        }
    }

    // Нормализуем вероятности
    for (int i = 0; i < NUM_CITIES; ++i) {
        probabilities[i] /= total;
    }

    return probabilities;
}

// Функция для выбора следующего города для муравья
int selectNextCity(int currentCity, const Ant& ant) {
    vector<double> probabilities = calculateProbabilities(currentCity, ant);

    // Случайный выбор города на основе распределения вероятностей
    double randomValue = static_cast<double>(rand()) / RAND_MAX;
    double sum = 0.0;
    for (int i = 0; i < NUM_CITIES; ++i) {
        sum += probabilities[i];
        if (randomValue <= sum) {
            return i;
        }
    }

    // Вернуть последний город, если случайное число вышло за пределы диапазона
    return NUM_CITIES - 1;
}

// Функция для выполнения муравьиного алгоритма
void antAlgorithm() {
    // Повторяем алгоритм заданное количество итераций 
    for (int iter = 0; iter < 100; ++iter) {
        // Создаём муравьёв и инициализируем начальные значения их маршрутов
        vector<Ant> ants(NUM_ANTS);
        for (int i = 0; i < NUM_ANTS; ++i) {
            ants[i].tour.resize(NUM_CITIES);
            ants[i].visited.assign(NUM_CITIES, false);
            ants[i].tour[0] = rand() % NUM_CITIES; // Начинаем с случайного города
            ants[i].visited[ants[i].tour[0]] = true;
        }

        // Проходим по всем городам, кроме первого
        for (int i = 1; i < NUM_CITIES; ++i) {
            // Для каждого муравья выбираем следующий город
            for (int j = 0; j < NUM_ANTS; ++j) {
                int nextCity = selectNextCity(ants[j].tour[i - 1], ants[j]);
                ants[j].tour[i] = nextCity;
                ants[j].visited[nextCity] = true;
            }
        }

        // Обновляем феромоны на рёбрах
        for (int i = 0; i < NUM_ANTS; ++i) {
            double tourLength = 0.0;
            for (int j = 0; j < NUM_CITIES - 1; ++j) {
                tourLength += distanceMatrix[ants[i].tour[j]][ants[i].tour[j + 1]];
            }
            // Обновляем длину маршрута муравья
            tourLength += distanceMatrix[ants[i].tour[NUM_CITIES - 1]][ants[i].tour[0]];

            // Проверяем, является ли новый маршрут лучше текущего найденного лучшего
            if (tourLength < bestTourLength) {
                bestTourLength = tourLength;
            }

            // Обновляем феромон на всех рёбрах маршрута муравья
            for (int j = 0; j < NUM_CITIES - 1; ++j) {
                int city1 = ants[i].tour[j];
                int city2 = ants[i].tour[j + 1];
                pheromoneMatrix[city1][city2] += Q / tourLength;
                pheromoneMatrix[city2][city1] += Q / tourLength;
            }
            // Обновляем феромон на последнем ребре маршрута (от последнего города к первому)
            pheromoneMatrix[ants[i].tour[NUM_CITIES - 1]][ants[i].tour[0]] += Q / tourLength;
            pheromoneMatrix[ants[i].tour[0]][ants[i].tour[NUM_CITIES - 1]] += Q / tourLength;
        }

        // Испарение феромона
        for (int i = 0; i < NUM_CITIES; ++i) {
            for (int j = 0; j < NUM_CITIES; ++j) {
                pheromoneMatrix[i][j] *= (1.0 - RHO);
            }
        }
    }
}

int main()
{
    setlocale(LC_ALL, "ru");
    srand(time(nullptr)); // Инициализируем генератор случайных чисел

    initializeDistanceMatrix(); // Инициализируем матрицу расстояний
    initializePheromoneMatrix(); // Инициализируем матрицу феромонов

    antAlgorithm(); // Запускаем муравьиный алгоритм

    cout << "Лучший маршрут: " << bestTourLength << endl; // Выводим длину лучшего найденного маршрута

    return 0;
}