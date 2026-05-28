#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <locale>

using namespace std;

void fillTriangle(vector<string>& grid, int topRow, int topColumn, int size) {
    for (int i = 0; i < size; i++) {
        int r = topRow + i;
        int left = topColumn - i;
        int right = topColumn + i;
        for (int c = left; c <= right; c++) {
            if (r >= 0 && r < (int)grid.size() && c >= 0 && c < (int)grid[0].size())
                grid[r][c] = 'o';
        }
    }
}

void sierpinski(vector<string>& grid, int topRow, int topColumn, int size, int level) {
    if (level == 0) {
        fillTriangle(grid, topRow, topColumn, size);
        return;
    }

    int half = size / 2;

    sierpinski(grid, topRow,         topColumn,     half, level - 1);
    sierpinski(grid, topRow + half,  topColumn - half, half, level - 1);
    sierpinski(grid, topRow + half,  topColumn + half, half, level - 1);

}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int level;
    cout << "Введите уровень рекурсии: ";
    cin >> level;

    if (level < 0) level = 0;
    int size = 1 << level;

    int rows = size;
    int cols = 2 * size - 1;

    vector<string> grid(rows, string(cols, ' '));

    int topRow = 0;
    int topColumn = cols / 2;

    sierpinski(grid, topRow, topColumn, size, level);

    for (int r = 0; r < rows; r++) {
        cout << grid[r] << "\n";
    }

    return 0;
}