package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// Grid - тип для хранения доски (двумерный срез)
type Grid [][]int

// State - структура состояния доски
type State struct {
	board Grid
	moves int
}

// Вывод доски на экран
func printBoard(board Grid, title string) {
	if title != "" {
		fmt.Println("\n" + title)
	}

	for _, row := range board {
		for _, cell := range row {
			fmt.Printf("%d ", cell)
		}
		fmt.Println()
	}
	fmt.Println()
}

// Проверка, является ли доска рамкой
func isFrame(board Grid) bool {
	n := len(board)
	if n < 3 {
		return false
	}
	m := len(board[0])
	if m < 3 {
		return false
	}

	// Определяем значение края
	edgeValue := board[0][0]

	// Проверяем все края (верх и низ)
	for j := 0; j < m; j++ {
		if board[0][j] != edgeValue || board[n-1][j] != edgeValue {
			return false
		}
	}

	// Проверяем все края (лево и право)
	for i := 0; i < n; i++ {
		if board[i][0] != edgeValue || board[i][m-1] != edgeValue {
			return false
		}
	}

	// Определяем значение внутренней части
	innerValue := board[1][1]

	// Внутреннее значение должно отличаться от краевого
	if innerValue == edgeValue {
		return false
	}

	// Проверяем внутренность
	for i := 1; i < n-1; i++ {
		for j := 1; j < m-1; j++ {
			if board[i][j] != innerValue {
				return false
			}
		}
	}

	return true
}

// Получение хеша доски (строковое представление для map)
func getHash(board Grid) string {
	var sb strings.Builder
	for _, row := range board {
		for _, cell := range row {
			sb.WriteString(strconv.Itoa(cell))
		}
	}
	return sb.String()
}

// Глубокое копирование доски (важно в Go, так как срезы передаются по ссылке)
func copyGrid(src Grid) Grid {
	dst := make(Grid, len(src))
	for i := range src {
		dst[i] = make([]int, len(src[i]))
		copy(dst[i], src[i])
	}
	return dst
}

// Ввод матрицы с проверкой на 0 и 1
func inputBoard() Grid {
	fmt.Println("\nВВОД МАТРИЦЫ МОНЕТ")

	var board Grid
	rowNumber := 1
	scanner := bufio.NewScanner(os.Stdin)

	for {
		fmt.Printf("Строка %d: ", rowNumber)
		if !scanner.Scan() {
			break
		}
		line := scanner.Text()

		// Пустая строка означает конец ввода
		if strings.TrimSpace(line) == "" {
			if len(board) == 0 {
				fmt.Println("Ошибка: введите хотя бы одну строку!")
				continue
			}
			break
		}

		var row []int
		hasError := false

		// Парсим строку символ за символом
		for i := 0; i < len(line); i++ {
			ch := line[i]

			// Пропускаем разделители
			if ch == ' ' || ch == ',' || ch == ';' || ch == '\t' {
				continue
			}

			// Проверяем, что это 0 или 1
			if ch == '0' {
				row = append(row, 0)
			} else if ch == '1' {
				row = append(row, 1)
			} else {
				// Недопустимый символ
				fmt.Printf("Ошибка: обнаружен недопустимый символ '%c' на позиции %d\n", ch, i+1)
				fmt.Println("Вводите только 0, 1, пробелы и запятые!")
				hasError = true
				break
			}
		}

		// Если была ошибка при парсинге - повторяем ввод
		if hasError {
			continue
		}

		// Проверка на пустоту после парсинга
		if len(row) == 0 {
			fmt.Println("Ошибка: не обнаружены значения 0 или 1!")
			continue
		}

		// Проверка количества столбцов (должны совпадать)
		if len(board) > 0 && len(row) != len(board[0]) {
			fmt.Printf("Ошибка: количество элементов (%d) не совпадает с предыдущими строками (%d)\n", len(row), len(board[0]))
			continue
		}

		// Выводим принятую строку
		fmt.Print("Принято: ")
		for _, val := range row {
			fmt.Printf("%d ", val)
		}
		fmt.Println()

		board = append(board, row)
		rowNumber++
	}

	fmt.Println("\nМатрица успешно введена!")
	if len(board) > 0 {
		fmt.Printf("Размер матрицы: %d x %d\n", len(board), len(board[0]))
	}

	return board
}

// ФУНКЦИЯ: Решение задачи методом BFS
func solveUsingBFS(initialBoard Grid) int {
	if len(initialBoard) == 0 {
		return -1
	}

	n := len(initialBoard)
	m := len(initialBoard[0])

	fmt.Println("\nПОИСК МИНИМАЛЬНОГО ЧИСЛА ХОДОВ (МЕТОД BFS)")

	// Проверка минимального размера для рамки
	if n < 3 || m < 3 {
		fmt.Println("Ошибка: матрица слишком маленькая")
		fmt.Println("Для создания рамки нужна матрица минимум 3x3")
		return -1
	}

	if isFrame(initialBoard) {
		fmt.Println("Исходная доска уже является рамкой!")
		fmt.Println("Минимальное число ходов: 0")
		printBoard(initialBoard, "Исходная доска:")
		return 0
	}

	queue := []State{{board: initialBoard, moves: 0}}

	// В Go set реализуется через map[Тип]bool
	visited := make(map[string]bool)
	visited[getHash(initialBoard)] = true

	statesExplored := 0

	for len(queue) > 0 {
		// Извлекаем первый элемент (pop front)
		current := queue[0]
		queue = queue[1:]

		statesExplored++

		// \r переписывает текущую строку в консоли
		fmt.Printf("Исследовано состояний: %d\r", statesExplored)

		for i := 0; i < n; i++ {
			for j := 0; j < m; j++ {

				// ХОД ВПРАВО
				if j+1 < m {
					nextBoard := copyGrid(current.board)

					// Инвертируем ячейки (1-x меняет 0 на 1 и 1 на 0)
					nextBoard[i][j] = 1 - nextBoard[i][j]
					nextBoard[i][j+1] = 1 - nextBoard[i][j+1]

					if isFrame(nextBoard) {
						fmt.Println("\n\nРешение найдено!")
						fmt.Printf("Минимальное число ходов: %d\n", current.moves+1)
						fmt.Printf("Всего исследовано состояний: %d\n", statesExplored)
						printBoard(nextBoard, "Итоговая рамка:")
						return current.moves + 1
					}

					hash := getHash(nextBoard)
					if !visited[hash] {
						visited[hash] = true
						queue = append(queue, State{board: nextBoard, moves: current.moves + 1})
					}
				}

				// ХОД ВНИЗ
				if i+1 < n {
					nextBoard := copyGrid(current.board)

					nextBoard[i][j] = 1 - nextBoard[i][j]
					nextBoard[i+1][j] = 1 - nextBoard[i+1][j]

					if isFrame(nextBoard) {
						fmt.Println("\n\nРешение найдено!")
						fmt.Printf("Минимальное число ходов: %d\n", current.moves+1)
						fmt.Printf("Всего исследовано состояний: %d\n", statesExplored)
						printBoard(nextBoard, "Итоговая рамка:")
						return current.moves + 1
					}

					hash := getHash(nextBoard)
					if !visited[hash] {
						visited[hash] = true
						queue = append(queue, State{board: nextBoard, moves: current.moves + 1})
					}
				}
			}
		}
	}

	fmt.Println("\n\nРешение не найдено")
	fmt.Println("Не удалось создать рамку из этой конфигурации")

	return -1
}

func main() {
	// В Go локаль UTF-8 используется для вывода в консоль по умолчанию
	fmt.Println("ЗАДАЧА О МОНЕТАХ НА ЩИТЕ (СОЗДАНИЕ РАМКИ)")

	board := inputBoard()

	printBoard(board, "Ваша матрица:")

	result := solveUsingBFS(board)

	fmt.Println("\nИТОГОВЫЙ РЕЗУЛЬТАТ")
	fmt.Println("==================\n")

	if result == -1 {
		fmt.Println("Решение не найдено")
	} else if result == 0 {
		fmt.Println("Доска уже является рамкой!")
		fmt.Println("Требуется ходов: 0")
	} else {
		fmt.Printf("Минимальное число ходов: %d\n", result)
	}

	fmt.Println("\nПРОГРАММА ЗАВЕРШЕНА")
}
