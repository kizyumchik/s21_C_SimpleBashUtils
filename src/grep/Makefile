SHELL := /bin/bash
FILE = test.txt
FILE1 = test1.txt

all: s21_grep

s21_grep: 
	gcc -Wall -Werror -Wextra -std=c11 grep.c -o s21_grep

clean: 
	rm s21_grep

rebuild: clean all

tests:
	-diff <(./s21_grep dear $(FILE)) <(grep dear $(FILE))
	-diff <(./s21_grep -e dear $(FILE)) <(grep -e dear $(FILE))
	-diff <(./s21_grep -i hello $(FILE)) <(grep -i hello $(FILE))
	-diff <(./s21_grep -v Hello $(FILE)) <(grep -v Hello $(FILE))
	-diff <(./s21_grep -c dear $(FILE)) <(grep -c dear $(FILE))
	-diff <(./s21_grep -l dear $(FILE) $(FILE1)) <(grep -l dear $(FILE) $(FILE1))
	-diff <(./s21_grep -n dear $(FILE)) <(grep -n dear $(FILE))
	-diff <(./s21_grep -h dear $(FILE) $(FILE1)) <(grep -h dear $(FILE) $(FILE1))
	-diff <(./s21_grep -s dear $(FILE) $(FILE1)) <(grep -s dear $(FILE) $(FILE1))
	-diff <(./s21_grep -f $(FILE1) $(FILE)) <(grep -f $(FILE1) $(FILE))
	-diff <(./s21_grep -o dear $(FILE)) <(grep -o dear $(FILE))
	-diff <(./s21_grep -in Hello $(FILE)) <(grep -in Hello $(FILE))
	-diff <(./s21_grep -cv dear $(FILE)) <(grep -cv dear $(FILE))
	-diff <(./s21_grep -iv dear $(FILE)) <(grep -iv dear $(FILE))
	-diff <(./s21_grep -lv dear $(FILE) $(FILE1)) <(grep -lv dear $(FILE) $(FILE1))
	-diff <(./s21_grep -ho dear $(FILE) $(FILE1)) <(grep -ho dear $(FILE) $(FILE1))
	-diff <(./s21_grep -nf $(FILE1) $(FILE)) <(grep -nf $(FILE1) $(FILE))
