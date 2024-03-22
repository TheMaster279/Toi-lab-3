#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

const int NUM_CITIES = 5; // ���������� �������
const int NUM_ANTS = 10; // ���������� ��������
const double ALPHA = 1.0; // �������� ������� ��������
const double BETA = 2.0; // �������� ������� ����� ����
const double RHO = 0.5; // ����������� ��������� ��������
const double Q = 100.0; // ���������� ��������, ����������� �� �����

double distanceMatrix[NUM_CITIES][NUM_CITIES]; // ������� ���������� ����� ��������
double pheromoneMatrix[NUM_CITIES][NUM_CITIES]; // ������� ��������� �� �����
double bestTourLength = numeric_limits<double>::max(); // ����� ������� ���������� ����

// ��������� ��� ������������� �������
struct Ant {
    vector<int> tour; // ������� ������� �������
    vector<bool> visited; // ���������� ������
};

// ������� ��� ������������� ������� ���������� ����� ��������
void initializeDistanceMatrix() {
    // � ���� ������� ������ �������� ���������� ������� ����������
    for (int i = 0; i < NUM_CITIES; ++i) {
        for (int j = 0; j < NUM_CITIES; ++j) {
            if (i != j) {
                distanceMatrix[i][j] = rand() % 100 + 1; // ���������� ��������� ���������� �� 1 �� 100
            }
            else {
                distanceMatrix[i][j] = 0; // ���������� �� ������ �� ������ ���� ����� 0
            }
        }
    }
}

// ������� ��� ������������� ������� ���������
void initializePheromoneMatrix() {
    // �������������� ��� �������� � ������� ��������� ��������� ���������
    for (int i = 0; i < NUM_CITIES; ++i) {
        for (int j = 0; j < NUM_CITIES; ++j) {
            pheromoneMatrix[i][j] = 1.0; // ��������� �������� ��������
        }
    }
}

// ������� ��� ������� ������������ �������� �� ������ ������ � �������
vector<double> calculateProbabilities(int currentCity, const Ant& ant) {
    vector<double> probabilities(NUM_CITIES, 0.0);
    double total = 0.0;

    // ������� ����� (tau^alpha * eta^beta) ��� ���� ��������� �������
    for (int i = 0; i < NUM_CITIES; ++i) {
        if (!ant.visited[i]) {
            probabilities[i] = pow(pheromoneMatrix[currentCity][i], ALPHA) * pow(1.0 / distanceMatrix[currentCity][i], BETA);
            total += probabilities[i];
        }
    }

    // ����������� �����������
    for (int i = 0; i < NUM_CITIES; ++i) {
        probabilities[i] /= total;
    }

    return probabilities;
}

// ������� ��� ������ ���������� ������ ��� �������
int selectNextCity(int currentCity, const Ant& ant) {
    vector<double> probabilities = calculateProbabilities(currentCity, ant);

    // ��������� ����� ������ �� ������ ������������� ������������
    double randomValue = static_cast<double>(rand()) / RAND_MAX;
    double sum = 0.0;
    for (int i = 0; i < NUM_CITIES; ++i) {
        sum += probabilities[i];
        if (randomValue <= sum) {
            return i;
        }
    }

    // ������� ��������� �����, ���� ��������� ����� ����� �� ������� ���������
    return NUM_CITIES - 1;
}

// ������� ��� ���������� ����������� ���������
void antAlgorithm() {
    // ��������� �������� �������� ���������� �������� 
    for (int iter = 0; iter < 100; ++iter) {
        // ������ �������� � �������������� ��������� �������� �� ���������
        vector<Ant> ants(NUM_ANTS);
        for (int i = 0; i < NUM_ANTS; ++i) {
            ants[i].tour.resize(NUM_CITIES);
            ants[i].visited.assign(NUM_CITIES, false);
            ants[i].tour[0] = rand() % NUM_CITIES; // �������� � ���������� ������
            ants[i].visited[ants[i].tour[0]] = true;
        }

        // �������� �� ���� �������, ����� �������
        for (int i = 1; i < NUM_CITIES; ++i) {
            // ��� ������� ������� �������� ��������� �����
            for (int j = 0; j < NUM_ANTS; ++j) {
                int nextCity = selectNextCity(ants[j].tour[i - 1], ants[j]);
                ants[j].tour[i] = nextCity;
                ants[j].visited[nextCity] = true;
            }
        }

        // ��������� �������� �� �����
        for (int i = 0; i < NUM_ANTS; ++i) {
            double tourLength = 0.0;
            for (int j = 0; j < NUM_CITIES - 1; ++j) {
                tourLength += distanceMatrix[ants[i].tour[j]][ants[i].tour[j + 1]];
            }
            // ��������� ����� �������� �������
            tourLength += distanceMatrix[ants[i].tour[NUM_CITIES - 1]][ants[i].tour[0]];

            // ���������, �������� �� ����� ������� ����� �������� ���������� �������
            if (tourLength < bestTourLength) {
                bestTourLength = tourLength;
            }

            // ��������� ������� �� ���� ����� �������� �������
            for (int j = 0; j < NUM_CITIES - 1; ++j) {
                int city1 = ants[i].tour[j];
                int city2 = ants[i].tour[j + 1];
                pheromoneMatrix[city1][city2] += Q / tourLength;
                pheromoneMatrix[city2][city1] += Q / tourLength;
            }
            // ��������� ������� �� ��������� ����� �������� (�� ���������� ������ � �������)
            pheromoneMatrix[ants[i].tour[NUM_CITIES - 1]][ants[i].tour[0]] += Q / tourLength;
            pheromoneMatrix[ants[i].tour[0]][ants[i].tour[NUM_CITIES - 1]] += Q / tourLength;
        }

        // ��������� ��������
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
    srand(time(nullptr)); // �������������� ��������� ��������� �����

    initializeDistanceMatrix(); // �������������� ������� ����������
    initializePheromoneMatrix(); // �������������� ������� ���������

    antAlgorithm(); // ��������� ���������� ��������

    cout << "������ �������: " << bestTourLength << endl; // ������� ����� ������� ���������� ��������

    return 0;
}