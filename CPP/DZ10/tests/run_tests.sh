#!/usr/bin/env bash
# Прогон всех тестов для задач 1-3 (ДЗ10).
# Использование: bash tests/run_tests.sh

set -u
cd "$(dirname "$0")/.." || exit 1

# Сборка программ (если ещё не собраны).
make >/dev/null || { echo "Ошибка сборки."; exit 1; }

# run <программа> <файл данных> <заголовок теста>
run() {
    echo "=============================================================="
    echo "$3"
    echo "вход: tests/$2"
    echo "--------------------------------------------------------------"
    "./$1" < "tests/$2"
    echo
}

echo "### Задача 1: сортировка по убыванию и удаление дубликатов ###"
run task1 task1_basic.txt        "Тест 1.1 - обычный набор с дубликатами"
run task1 task1_duplicates.txt   "Тест 1.2 - все элементы одинаковые"
run task1 task1_negatives.txt    "Тест 1.3 - отрицательные числа и нули"
run task1 task1_sorted.txt       "Тест 1.4 - уже упорядоченный набор без дубликатов"
run task1 task1_empty.txt        "Тест 1.5 - пустой набор (N = 0)"

echo "### Задача 2: частота слов ###"
run task2 task2_english.txt      "Тест 2.1 - английский текст со знаками препинания"
run task2 task2_russian.txt      "Тест 2.2 - русский текст (UTF-8), разный регистр"
run task2 task2_short.txt        "Тест 2.3 - меньше пяти различных слов"
run task2 task2_digits.txt       "Тест 2.4 - числа как слова"
run task2 task2_punctuation.txt  "Тест 2.5 - только знаки препинания (слов нет)"

echo "### Задача 3: алгоритмы STL над vector<int> ###"
run task3 task3_basic.txt        "Тест 3.1 - смешанный набор"
run task3 task3_positive.txt    "Тест 3.2 - после замены модулями все положительны"
run task3 task3_negative.txt     "Тест 3.3 - все элементы отрицательные"
run task3 task3_zeros.txt        "Тест 3.4 - все нули"
run task3 task3_single.txt       "Тест 3.5 - один элемент"
