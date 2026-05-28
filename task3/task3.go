package main

import (
	"fmt"
	"math"
	"strings"
)

type Matrix [][]float64
type Vector []float64

// Вывод матрицы
func printMatrix(A Matrix, b Vector) {
	n := len(A)
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			// аналог setw(10) << fixed << setprecision(4)
			fmt.Printf("%10.4f ", A[i][j])
		}
		fmt.Printf("| %10.4f\n", b[i])
	}
	// аналог string(60, '-')
	fmt.Println(strings.Repeat("-", 60))
}

// ПРЯМОЙ МЕТОД: Метод Гаусса (Прямой ход с выводом шагов)
func forwardElimination(A Matrix, b Vector) {
	n := len(A)
	fmt.Println("ИСХОДНАЯ МАТРИЦА:")
	printMatrix(A, b)

	for col := 0; col < n-1; col++ {
		maxRow := col
		for row := col + 1; row < n; row++ {
			if math.Abs(A[row][col]) > math.Abs(A[maxRow][col]) {
				maxRow = row
			}
		}

		if maxRow != col {
			// В Go обмен значений (swap) делается в одну строку
			A[col], A[maxRow] = A[maxRow], A[col]
			b[col], b[maxRow] = b[maxRow], b[col]
		}

		for row := col + 1; row < n; row++ {
			factor := A[row][col] / A[col][col]
			for j := col; j < n; j++ {
				A[row][j] -= factor * A[col][j]
			}
			b[row] -= factor * b[col]
		}
		fmt.Printf("Матрица после шага %d прямого хода:\n", col+1)
		printMatrix(A, b)
	}
}

// ПРЯМОЙ МЕТОД: Метод Гаусса (Обратный ход)
func backwardSubstitution(A Matrix, b Vector) Vector {
	n := len(A)
	x := make(Vector, n)
	for i := n - 1; i >= 0; i-- {
		x[i] = b[i]
		for j := i + 1; j < n; j++ {
			x[i] -= A[i][j] * x[j]
		}
		x[i] /= A[i][i]
	}
	return x
}

// ИТЕРАЦИОННЫЙ МЕТОД: Метод Зейделя
func solveSeidel(A Matrix, b Vector, eps float64) {
	n := len(A)

	// Преобразование матрицы для диагонального преобладания (согласно методике)
	// 1. Строка 4 = Строка 4 - Строка 2
	for j := 0; j < n; j++ {
		A[3][j] -= A[1][j]
	}
	b[3] -= b[1]

	// 2. Строка 1 = Строка 1 - 1.5 * Строка 4 (новая)
	for j := 0; j < n; j++ {
		A[0][j] -= 1.5 * A[3][j]
	}
	b[0] -= 1.5 * b[3]

	fmt.Println("\nМАТРИЦА ПОСЛЕ ПРЕОБРАЗОВАНИЯ (Выполнено условие сходимости):")
	printMatrix(A, b)

	x := make(Vector, n)
	x_new := make(Vector, n)
	var max_eps float64
	iter := 0

	fmt.Println("\nРезультаты метода Зейделя (в виде таблицы):")
	fmt.Println(strings.Repeat("-", 75))
	fmt.Printf("| %3s | %10s | %10s | %10s | %10s | %10s |\n", "N", "X1", "X2", "X3", "X4", "eps_n")
	fmt.Println(strings.Repeat("-", 75))

	// В Go нет цикла do...while, используем бесконечный for с выходом по условию
	for {
		max_eps = 0.0
		for i := 0; i < n; i++ {
			var sum1, sum2 float64
			// Используем уже вычисленные новые значения текущей итерации
			for j := 0; j < i; j++ {
				sum1 += A[i][j] * x_new[j]
			}
			// Используем значения с прошлой итерации
			for j := i + 1; j < n; j++ {
				sum2 += A[i][j] * x[j]
			}

			x_new[i] = (b[i] - sum1 - sum2) / A[i][i]

			if math.Abs(x_new[i]-x[i]) > max_eps {
				max_eps = math.Abs(x_new[i] - x[i])
			}
		}
		iter++
		
		// Копируем новые значения в x (аналог x = x_new; в C++)
		copy(x, x_new)

		fmt.Printf("| %3d | %10.5f | %10.5f | %10.5f | %10.5f | %10.5f |\n", iter, x[0], x[1], x[2], x[3], max_eps)

		// Условие выхода из цикла
		if max_eps <= eps {
			break
		}
	}
	fmt.Println(strings.Repeat("-", 75))

	fmt.Println("\nНАЙДЕННЫЕ КОРНИ (Метод Зейделя):")
	for i := 0; i < n; i++ {
		fmt.Printf("x%d = %f\n", i+1, x[i])
	}
}

// Вспомогательные функции для глубокого копирования (из-за особенностей ссылочных типов в Go)
func copyMatrix(src Matrix) Matrix {
	dst := make(Matrix, len(src))
	for i := range src {
		dst[i] = make(Vector, len(src[i]))
		copy(dst[i], src[i])
	}
	return dst
}

func copyVector(src Vector) Vector {
	dst := make(Vector, len(src))
	copy(dst, src)
	return dst
}

func main() {
	// В Go локаль UTF-8 используется для вывода в консоль по умолчанию.

	A := Matrix{
		{0.93, -0.04, 0.21, -1.16},
		{0.25, -1.23, 0.07, -0.09},
		{-0.21, 0.07, 0.80, -0.13},
		{0.15, -1.31, 0.06, -0.84},
	}
	b := Vector{-1.24, -0.84, 2.56, 0.93}

	A_gauss := copyMatrix(A)
	b_gauss := copyVector(b)

	fmt.Println("=== ПРЯМОЙ МЕТОД (МЕТОД ГАУССА) ===")
	forwardElimination(A_gauss, b_gauss)
	gauss_solution := backwardSubstitution(A_gauss, b_gauss)

	fmt.Println("НАЙДЕННЫЕ КОРНИ (Метод Гаусса - Обратный ход):")
	for i := 0; i < len(gauss_solution); i++ {
		fmt.Printf("x%d = %.5f\n", i+1, gauss_solution[i])
	}

	fmt.Println("\n\n=== ИТЕРАЦИОННЫЙ МЕТОД (МЕТОД ЗЕЙДЕЛЯ) ===")
	epsilon := 1e-4 // Заданная точность
	
	A_seidel := copyMatrix(A)
	b_seidel := copyVector(b)
	
	solveSeidel(A_seidel, b_seidel, epsilon)
}