#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;

typedef vector<vector<double>> Matrix;
typedef vector<double> Vector;

// Вывод матрицы
void printMatrix(const Matrix& A, const Vector& b) {
    int n = A.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << setw(10) << fixed << setprecision(4) << A[i][j] << " ";
        }
        cout << "| " << setw(10) << fixed << setprecision(4) << b[i] << "\n";
    }
    cout << string(60, '-') << "\n";
}

// ПРЯМОЙ МЕТОД: Метод Гаусса (Прямой ход с выводом шагов)
void forwardElimination(Matrix& A, Vector& b) {
    int n = A.size();
    cout << "ИСХОДНАЯ МАТРИЦА:\n";
    printMatrix(A, b);

    for (int col = 0; col < n - 1; col++) {
        int maxRow = col;
        for (int row = col + 1; row < n; row++) {
            if (abs(A[row][col]) > abs(A[maxRow][col])) maxRow = row;
        }
        
        if (maxRow != col) {
            swap(A[col], A[maxRow]);
            swap(b[col], b[maxRow]);
        }
        
        for (int row = col + 1; row < n; row++) {
            double factor = A[row][col] / A[col][col];
            for (int j = col; j < n; j++) A[row][j] -= factor * A[col][j];
            b[row] -= factor * b[col];
        }
        cout << "Матрица после шага " << col + 1 << " прямого хода:\n";
        printMatrix(A, b);
    }
}

// ПРЯМОЙ МЕТОД: Метод Гаусса (Обратный ход)
Vector backwardSubstitution(const Matrix& A, const Vector& b) {
    int n = A.size();
    Vector x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = b[i];
        for (int j = i + 1; j < n; j++) x[i] -= A[i][j] * x[j];
        x[i] /= A[i][i];
    }
    return x;
}

// ИТЕРАЦИОННЫЙ МЕТОД: Метод Зейделя
void solveSeidel(Matrix A, Vector b, double eps) {
    int n = A.size();
    
    // Преобразование матрицы для диагонального преобладания (согласно методике)
    // 1. Строка 4 = Строка 4 - Строка 2
    for(int j=0; j<n; j++) A[3][j] -= A[1][j];
    b[3] -= b[1];
    
    // 2. Строка 1 = Строка 1 - 1.5 * Строка 4 (новая)
    for(int j=0; j<n; j++) A[0][j] -= 1.5 * A[3][j];
    b[0] -= 1.5 * b[3];

    cout << "\nМАТРИЦА ПОСЛЕ ПРЕОБРАЗОВАНИЯ (Выполнено условие сходимости):\n";
    printMatrix(A, b);

    Vector x(n, 0.0); 
    Vector x_new(n, 0.0);
    double max_eps;
    int iter = 0;

    cout << "\nРезультаты метода Зейделя (в виде таблицы):\n";
    cout << string(75, '-') << "\n";
    cout << "| " << setw(3) << "N"
         << " | " << setw(10) << "X1"
         << " | " << setw(10) << "X2"
         << " | " << setw(10) << "X3"
         << " | " << setw(10) << "X4"
         << " | " << setw(10) << "eps_n" << " |\n";
    cout << string(75, '-') << "\n";

    do {
        max_eps = 0.0;
        for (int i = 0; i < n; i++) {
            double sum1 = 0, sum2 = 0;
            // Используем уже вычисленные новые значения текущей итерации
            for (int j = 0; j < i; j++) sum1 += A[i][j] * x_new[j];
            // Используем значения с прошлой итерации
            for (int j = i + 1; j < n; j++) sum2 += A[i][j] * x[j];
            
            x_new[i] = (b[i] - sum1 - sum2) / A[i][i];
            
            if (abs(x_new[i] - x[i]) > max_eps) {
                max_eps = abs(x_new[i] - x[i]);
            }
        }
        iter++;
        x = x_new;
        
        cout << "| " << setw(3) << iter 
             << " | " << setw(10) << fixed << setprecision(5) << x[0]
             << " | " << setw(10) << x[1]
             << " | " << setw(10) << x[2]
             << " | " << setw(10) << x[3]
             << " | " << setw(10) << max_eps << " |\n";
             
    } while (max_eps > eps);
    cout << string(75, '-') << "\n";
    
    cout << "\nНАЙДЕННЫЕ КОРНИ (Метод Зейделя):\n";
    for(int i = 0; i < n; i++) {
        cout << "x" << i+1 << " = " << x[i] << "\n";
    }
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    Matrix A = {
        {0.93, -0.04, 0.21, -1.16},
        {0.25, -1.23, 0.07, -0.09},
        {-0.21, 0.07, 0.80, -0.13},
        {0.15, -1.31, 0.06, -0.84}
    };
    Vector b = {-1.24, -0.84, 2.56, 0.93};
    
    Matrix A_gauss = A;
    Vector b_gauss = b;

    cout << "=== ПРЯМОЙ МЕТОД (МЕТОД ГАУССА) ===\n";
    forwardElimination(A_gauss, b_gauss);
    Vector gauss_solution = backwardSubstitution(A_gauss, b_gauss);
    
    cout << "НАЙДЕННЫЕ КОРНИ (Метод Гаусса - Обратный ход):\n";
    for(int i = 0; i < gauss_solution.size(); i++) {
        cout << "x" << i+1 << " = " << fixed << setprecision(5) << gauss_solution[i] << "\n";
    }

    cout << "\n\n=== ИТЕРАЦИОННЫЙ МЕТОД (МЕТОД ЗЕЙДЕЛЯ) ===\n";
    double epsilon = 1e-4; // Заданная точность
    solveSeidel(A, b, epsilon);

    return 0;
}