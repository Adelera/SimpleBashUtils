#!/bin/sh

# Цвета для вывода 
GREEN='\033[0;32m'
RED='\033[0;31m'
RESET='\033[0m'

# Пути к скомпилированным бинарникам 
CAT_BIN="../cat/s21_cat"
GREP_BIN="../grep/s21_grep"

# Подготовка стандартных текстовых файлов
echo -e "hello\tworld\n\n123 int debug\nline\n" > text1.txt
echo -e "debug mode\nfloat x\nint y;" > text2.txt
echo -e "int\ndebug" > patterns.txt
touch empty.txt

# ГЕНЕРАЦИЯ ФАЙЛА С НЕПЕЧАТАЕМЫМИ СИМВОЛАМИ
# \x01 -> ^A, \x09 -> \t (или ^I), \x7f -> ^?, \x82 -> M-^B
printf "Start\x01\x09Middle\x7fEnd\x82\n" > non_printable.txt

SUCCESS_COUNT=0
TOTAL_COUNT=0

run_test() {
    TOTAL_COUNT=$((TOTAL_COUNT + 1))
    UTIL=$1
    ARGS=$2

    if [ "$UTIL" = "cat" ]; then
        ORIGINAL_CMD="cat $ARGS"
        MY_CMD="$CAT_BIN $ARGS"
    else
        ORIGINAL_CMD="grep $ARGS"
        MY_CMD="$GREP_BIN $ARGS"
    fi

    # Выполнение оригинальной утилиты
    eval "$ORIGINAL_CMD" > orig_out.txt 2> orig_err.txt
    ORIG_CODE=$?

    # Выполнение нашей утилиты
    eval "$MY_CMD" > my_out.txt 2> my_err.txt
    MY_CODE=$?

    # Проверка результатов
    if ! diff orig_out.txt my_out.txt > /dev/null; then
        echo -e "${RED}[FAIL]${RESET} $ORIGINAL_CMD (Разный вывод stdout)"
    elif [ "$ORIG_CODE" -ne "$MY_CODE" ]; then
        echo -e "${RED}[FAIL]${RESET} $ORIGINAL_CMD (Разные коды возврата: Оригинал=$ORIG_CODE, Ваш=$MY_CODE)"
    elif [ -s orig_err.txt ] && [ ! -s my_err.txt ]; then
        echo -e "${RED}[FAIL]${RESET} $ORIGINAL_CMD (Ваша программа не вывела ошибку в stderr)"
    else
        echo -e "${GREEN}[OK]${RESET}   $ORIGINAL_CMD"
        SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
    fi
}

echo "=== ЗАПУСК ТЕСТОВ НА ALPINE (С НЕПЕЧАТАЕМЫМИ СИМВОЛАМИ) ==="

# --- ТЕСТЫ ДЛЯ CAT ---
if [ -f "$CAT_BIN" ]; then
    echo -e "\n--- Тестирование s21_cat ---"
    run_test "cat" "text1.txt"
    run_test "cat" "text1.txt text2.txt"
    run_test "cat" "-n text1.txt"
    run_test "cat" "-b text1.txt"
    run_test "cat" "-e text2.txt"
    
    # Тесты на непечатаемые символы
    run_test "cat" "-v non_printable.txt"  
    run_test "cat" "-t non_printable.txt"  
    run_test "cat" "-e non_printable.txt"  
    run_test "cat" "nonexistent_file.txt" 

else
    echo -e "${RED}Файл s21_cat не найден по пути $CAT_BIN${RESET}"
fi

# --- ТЕСТЫ ДЛЯ GREP ---
if [ -f "$GREP_BIN" ]; then
    echo -e "\n--- Тестирование s21_grep ---"
    run_test "grep" "int text1.txt"
    run_test "grep" "-i INT text1.txt"
    run_test "grep" "-v int text1.txt"
    run_test "grep" "-c debug text1.txt text2.txt"
    run_test "grep" "-n mode text2.txt"
    run_test "grep" "-e \"\(debug\|int\)\" text1.txt"
    
    run_test "grep" "-h int text1.txt text2.txt"
    run_test "grep" "-s int nonexistent_file.txt"
    run_test "grep" "-f patterns.txt text1.txt"
    run_test "grep" "-o int text1.txt"
    run_test "grep" "-l int text1.txt text2.txt"
    run_test "grep" "pattern nonexistent_file.txt" 

    # === СКЛЕЕННЫЕ ФЛАГИ ДЛЯ GREP ===
    # Простые склейки флагов без аргументов
    run_test "grep" "-in INT text1.txt"                    # Регистр + номер строки
    run_test "grep" "-cv debug text1.txt text2.txt"        # Подсчет строк, которые НЕ совпадают
    run_test "grep" "-nv mode text2.txt"                   # Номер строки + инверсия
    run_test "grep" "-oh int text1.txt text2.txt"          # Только совпадение + скрытие имен файлов
    run_test "grep" "-il INT text1.txt text2.txt"          # Регистр + только имена файлов с совпадениями
    run_test "grep" "-cl debug text1.txt text2.txt"        # Подсчет + имена файлов
    run_test "grep" "-sh int nonexistent_file.txt"         # Подавление ошибок + скрытие имен
    
    # Тройные и четверные склейки флагов без аргументов
    run_test "grep" "-inv INT text1.txt"                   # Регистр + номер + инверсия
    run_test "grep" "-och debug text1.txt text2.txt"       # Только совпадение + подсчет + скрытие файлов
    run_test "grep" "-invl INT text1.txt text2.txt"        # Регистр + номер + инверсия + вывод файлов

    # Склейки с флагами, принимающими аргументы (-e, -f)
    run_test "grep" "-ie INT text1.txt"                    # -i склеен с -e, паттерн идет следом
    run_test "grep" "-ne debug text1.txt"                  # -n склеен с -e
    run_test "grep" "-vhe int text1.txt text2.txt"         # -v и -h склеены с -e
    run_test "grep" "-if patterns.txt text1.txt"           # -i склеен с -f, файл паттернов идет следом
    run_test "grep" "-nf patterns.txt text1.txt"           # -n склеен с -f
    run_test "grep" "-vhf patterns.txt text1.txt text2.txt"# -v и -h склеены с -f
else
    echo -e "${RED}Файл s21_grep не найден по пути $GREP_BIN${RESET}"
fi

# Вывод итогов
echo -e "\n=== ИТОГ: Сдано $SUCCESS_COUNT из $TOTAL_COUNT тестов ==="

# Очистка временных файлов
rm -f text1.txt text2.txt patterns.txt empty.txt non_printable.txt orig_out.txt orig_err.txt my_out.txt my_err.txt

[ "$SUCCESS_COUNT" -eq "$TOTAL_COUNT" ] && exit 0 || exit 1