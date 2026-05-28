#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <locale>

using namespace std;

typedef vector<vector<int>> Grid;

// СТРУКТУРА: Состояние доски
struct State {
    Grid board;
    int moves;
    
    bool operator<(const State& other) const {
        return board < other.board;
    }
};

// Вывод доски на экран
void printBoard(const Grid& board, const string& title = "") {
    if (!title.empty()) {
        cout << "\n" << title << "\n";
    }
    
    int n = board.size();
    int m = board[0].size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

// Проверка, является ли доска рамкой
bool isFrame(const Grid& board) {
    int n = board.size();
    int m = board[0].size();
    
    if (n < 3 || m < 3) {
        return false;
    }
    
    // Определяем значение края
    int edgeValue = board[0][0];
    
    // Проверяем все края
    for (int i = 0; i < n; i++) {
        if (board[i][0] != edgeValue || board[i][m-1] != edgeValue) {
            return false;
        }
    }
    
    for (int j = 0; j < m; j++) {
        if (board[0][j] != edgeValue || board[n-1][j] != edgeValue) {
            return false;
        }
    }
    
    // Определяем значение внутренней части
    int innerValue = board[1][1];
    
    // Внутреннее значение должно отличаться от краевого
    if (innerValue == edgeValue) {
        return false;
    }
    
    // Проверяем внутренность
    for (int i = 1; i < n - 1; i++) {
        for (int j = 1; j < m - 1; j++) {
            if (board[i][j] != innerValue) {
                return false;
            }
        }
    }
    
    return true;
}

// Получение хеша доски
string getHash(const Grid& board) {
    string hash;
    for (const auto& row : board) {
        for (int cell : row) {
            hash += to_string(cell);
        }
    }
    return hash;
}

// Ввод матрицы с проверкой на 0 и 1
void inputBoard(Grid& board) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    cout << "\nВВОД МАТРИЦЫ МОНЕТ\n";
    
    board.clear();
    int rowNumber = 1;
    
    while (true) {
        cout << "Строка " << rowNumber << ": ";
        string line;
        getline(cin, line);
        
        // Пустая строка означает конец ввода
        if (line.empty()) {
            if (board.empty()) {
                cout << "Ошибка: введите хотя бы одну строку!\n";
                continue;
            }
            break;
        }
        
        vector<int> row;
        bool hasError = false;
        
        // Парсим строку символ за символом
        for (size_t j = 0; j < line.length(); j++) {
            char ch = line[j];
            
            // Пропускаем разделители
            if (ch == ' ' || ch == ',' || ch == ';' || ch == '\t') {
                continue;
            }
            
            // Проверяем, что это 0 или 1
            if (ch == '0') {
                row.push_back(0);
            }
            else if (ch == '1') {
                row.push_back(1);
            }
            else {
                // Недопустимый символ
                cout << "Ошибка: обнаружен недопустимый символ '" << ch 
                     << "' на позиции " << (j + 1) << "\n";
                cout << "Вводите только 0, 1, пробелы и запятые!\n";
                hasError = true;
                break;
            }
        }
        
        // Если была ошибка при парсинге - повторяем ввод
        if (hasError) {
            continue;
        }
        
        // Проверка на пустоту после парсинга
        if (row.empty()) {
            cout << "Ошибка: не обнаружены значения 0 или 1!\n";
            continue;
        }
        
        // Проверка количества столбцов (должны совпадать)
        if (!board.empty() && row.size() != board[0].size()) {
            cout << "Ошибка: количество элементов (" << row.size() 
                 << ") не совпадает с предыдущими строками (" 
                 << board[0].size() << ")\n";
            continue;
        }
        
        // Выводим принятую строку
        cout << "Принято: ";
        for (int val : row) {
            cout << val << " ";
        }
        cout << "\n";
        
        board.push_back(row);
        rowNumber++;
    }
    
    cout << "\nМатрица успешно введена!\n";
    cout << "Размер матрицы: " << board.size() << " x " << board[0].size() << "\n";
}

// ФУНКЦИЯ: Решение задачи методом BFS
int solveUsingBFS(Grid initialBoard) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    int n = initialBoard.size();
    int m = initialBoard[0].size();
    
    cout << "\nПОИСК МИНИМАЛЬНОГО ЧИСЛА ХОДОВ (МЕТОД BFS)\n";
    
    // Проверка минимального размера для рамки
    if (n < 3 || m < 3) {
        cout << "Ошибка: матрица слишком маленькая\n";
        cout << "Для создания рамки нужна матрица минимум 3x3\n";
        return -1;
    }
    
    if (isFrame(initialBoard)) {
        cout << "Исходная доска уже является рамкой!\n";
        cout << "Минимальное число ходов: 0\n";
        printBoard(initialBoard, "Исходная доска:");
        return 0;
    }
    
    queue<State> q;
    set<string> visited;
    
    State initial;
    initial.board = initialBoard;
    initial.moves = 0;
    
    q.push(initial);
    visited.insert(getHash(initial.board));
    
    int statesExplored = 0;
    
    while (!q.empty()) {
        State current = q.front();
        q.pop();
        statesExplored++;
        
        cout << "Исследовано состояний: " << statesExplored << "\r";
        cout.flush();
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                
                // ХОД ВПРАВО
                if (j + 1 < m) {
                    State next;
                    next.board = current.board;
                    next.moves = current.moves + 1;
                    
                    next.board[i][j] = 1 - next.board[i][j];
                    next.board[i][j+1] = 1 - next.board[i][j+1];
                    
                    if (isFrame(next.board)) {
                        cout << "\nРешение найдено!\n";
                        cout << "Минимальное число ходов: " << next.moves << "\n";
                        cout << "Всего исследовано состояний: " << statesExplored << "\n";
                        printBoard(next.board, "Итоговая рамка:");
                        return next.moves;
                    }
                    
                    string hash = getHash(next.board);
                    if (visited.find(hash) == visited.end()) {
                        visited.insert(hash);
                        q.push(next);
                    }
                }
                
                // ХОД ВНИЗ
                if (i + 1 < n) {
                    State next;
                    next.board = current.board;
                    next.moves = current.moves + 1;
                    
                    next.board[i][j] = 1 - next.board[i][j];
                    next.board[i+1][j] = 1 - next.board[i+1][j];
                    
                    if (isFrame(next.board)) {
                        cout << "\nРешение найдено!\n";
                        cout << "Минимальное число ходов: " << next.moves << "\n";
                        cout << "Всего исследовано состояний: " << statesExplored << "\n";
                        printBoard(next.board, "Итоговая рамка:");
                        return next.moves;
                    }
                    
                    string hash = getHash(next.board);
                    if (visited.find(hash) == visited.end()) {
                        visited.insert(hash);
                        q.push(next);
                    }
                }
            }
        }
    }
    
    cout << "\nРешение не найдено\n";
    cout << "Не удалось создать рамку из этой конфигурации\n";
    
    return -1;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    cout << "ЗАДАЧА О МОНЕТАХ НА ЩИТЕ (СОЗДАНИЕ РАМКИ)\n";
    
    Grid board;
    
    inputBoard(board);
    
    printBoard(board, "Ваша матрица:");
    
    int result = solveUsingBFS(board);
    
    cout << "\nИТОГОВЫЙ РЕЗУЛЬТАТ\n";
    cout << "==================\n\n";
    
    if (result == -1) {
        cout << "Решение не найдено\n";
    } else if (result == 0) {
        cout << "Доска уже является рамкой!\n";
        cout << "Требуется ходов: 0\n";
    } else {
        cout << "Минимальное число ходов: " << result << "\n";
    }
    
    cout << "\nПРОГРАММА ЗАВЕРШЕНА\n";
    
    return 0;
}