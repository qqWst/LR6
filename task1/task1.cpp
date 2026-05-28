#include <iostream>
#include <vector>
#include <random>
#include <locale>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int HEIGHT = 20;
const int WIDTH = 40;
const int DELAY = 250;

void display(const vector<vector<int>>& m) {
    for (const vector<int>& str : m) {
        cout << "[ ";
        for (const int& element : str) {
            cout << setw(4) << element;
        }
        cout << " ]" << endl;
    }
}

void reverseMatrix(int N, int M) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    random_device rd;
    mt19937 r(rd());
    uniform_int_distribution<int> range(-50, 50);
    vector<vector<int>> matrix(N, vector<int>(M));
    for (vector<int>& str : matrix) {
        for (int& element : str) {
            element = range(r);
        }
    }
    cout << "Начальная матрица:" << endl;
    display(matrix);

    for (size_t i = 0; i < matrix.size() / 2; ++i) {
        vector<int> temp = matrix[i];
        matrix[i] = matrix[matrix.size() - 1 - i];
        matrix[matrix.size() - 1 - i] = temp;
    }
    cout << "Матрица в обратном порядке:" << endl;
    display(matrix);
    int maxElement = matrix[0][0], minElement = maxElement;
    for (const vector<int>& str : matrix) {
        for (const int element : str) {
            if (element > maxElement) maxElement = element;
            if (element < minElement) minElement = element;
        }
    }
    int secondMax, secondMin;
    int diff1 = abs(matrix[0][0] - maxElement);
    int diff2 = abs(matrix[0][0] - minElement);
    for (const vector<int>& str : matrix) {
        for (const int element : str) {
            if (element == maxElement || element == minElement) continue;
            if (abs(element - maxElement) < diff1) {
                secondMax = element;
                diff1 = abs(element - maxElement);
            }
            if (abs(element - minElement) < diff2) {
                secondMin = element;
                diff2 = abs(element - minElement);
            }
        }
    }
    cout << "Второй максимальный элемент: " << secondMax << endl;
    cout << "Второй минимальный элемент: " << secondMin << endl;
}

void symetric(int N) {
    int M;
    if (N % 2) M = N / 2 + 1;
    else M = N / 2;
    vector<vector<int>> matrix(N, vector<int>(M, 0));
    matrix[0][0] = matrix[N - 1][0] = 100;
    for(size_t i = 1; i < M; ++i) {
        matrix[i][0] = matrix[i - 1][0] + 5 * i;
        matrix[N - 1 - i][0] = matrix[i][0];
        for (size_t j = 1; j < M; ++j) {
            if (j > i) continue;
            matrix[i][j] = matrix[i][j - 1] + 5;
            matrix[N - 1 - i][j] = matrix[i][j];
        }
    }
    display(matrix);
}

// Очистка консоли
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Вывод поля
void printField(const vector<vector<int>>& field) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (field[i][j] == 1)
                cout << "0 ";
            else
                cout << "  ";
        }
        cout << endl;
    }
}

// Подсчёт живых соседей
int countNeighbors(const vector<vector<int>>& field, int y, int x) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {

            if (dy == 0 && dx == 0)
                continue;

            int ny = y + dy;
            int nx = x + dx;

            if (ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH) {
                count += field[ny][nx];
            }
        }
    }

    return count;
}

// Классические правила Конвея
void nextGenerationClassic(vector<vector<int>>& field) {
    vector<vector<int>> next = field;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {

            int neighbors = countNeighbors(field, i, j);

            // Живая клетка
            if (field[i][j] == 1) {

                if (neighbors < 2 || neighbors > 3)
                    next[i][j] = 0;
                else
                    next[i][j] = 1;
            }
            // Мёртвая клетка
            else {

                if (neighbors == 3)
                    next[i][j] = 1;
                else
                    next[i][j] = 0;
            }
        }
    }

    field = next;
}

// Изменённые (оригинальные) правила
// Живая клетка выживает при 2,3,4 соседях
// Мёртвая оживает при 3 или 6 соседях
void nextGenerationCustom(vector<vector<int>>& field) {
    vector<vector<int>> next = field;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {

            int neighbors = countNeighbors(field, i, j);

            if (field[i][j] == 1) {

                if (neighbors == 2 || neighbors == 3 || neighbors == 4)
                    next[i][j] = 1;
                else
                    next[i][j] = 0;
            }
            else {

                if (neighbors == 3 || neighbors == 6)
                    next[i][j] = 1;
                else
                    next[i][j] = 0;
            }
        }
    }

    field = next;
}

// Создание случайного поля
void randomField(vector<vector<int>>& field) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            field[i][j] = rand() % 2;
        }
    }
}

// Фигура Block
void createBlock(vector<vector<int>>& field) {
    field.assign(HEIGHT, vector<int>(WIDTH, 0));

    int y = HEIGHT / 2;
    int x = WIDTH / 2;

    field[y][x] = 1;
    field[y][x + 1] = 1;
    field[y + 1][x] = 1;
    field[y + 1][x + 1] = 1;
}

// Демонстрация
void runSimulation(vector<vector<int>> field,
                   void (*nextGen)(vector<vector<int>>&),
                   int generations,
                   const string& title) {

    for (int gen = 0; gen < generations; gen++) {

        clearScreen();

        cout << title << endl;
        cout << "Generation: " << gen << endl << endl;

        printField(field);

        nextGen(field);

        this_thread::sleep_for(chrono::milliseconds(DELAY));
    }
}

void life() {
    srand(time(0));

    vector<vector<int>> field(HEIGHT, vector<int>(WIDTH, 0));

    // 1. Устойчивая фигура Block
    createBlock(field);

    runSimulation(field,
                  nextGenerationClassic,
                  10,
                  "CLASSIC RULES - BLOCK");

    // 2. Та же фигура, но новые правила
    createBlock(field);

    runSimulation(field,
                  nextGenerationCustom,
                  10,
                  "CUSTOM RULES - BLOCK");

    // 3. Случайная колония (классика)
    randomField(field);

    runSimulation(field,
                  nextGenerationClassic,
                  30,
                  "CLASSIC RULES - RANDOM");

    // 4. Случайная колония (новые правила)

    randomField(field);

    runSimulation(field,
                  nextGenerationCustom,
                  30,
                  "CUSTOM RULES - RANDOM");
}


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    char task;
    cout << "Номер задания (1-3): ";
    while (cin >> task) {
        if (task == '1') {
            int N, M;
            cout << "Введите M: ";
            cin >> M;
            cout << "Введите N: ";
            cin >> N;
            if (N < 1 || M < 1) {
                cout << "ОШИБКА: M и N должны быть больше 0!" << endl;
                return 0;
            }
            reverseMatrix(N, M);
        } 
        else if (task == '2') {
            int N;
            cin >> N;
            if (N < 1) {
                cout << "ОШИБКА: N должен быть больше 0!" << endl;
                return 0;
            }
            symetric(N);
        }
        else if (task == '3') {
            life();
        }
    }
    return 0;
}