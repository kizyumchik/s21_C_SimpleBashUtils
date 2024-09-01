SHELL := /bin/bash
FILE = test.txt

all: s21_cat

s21_cat: 
	gcc -Wall -Werror -Wextra -std=c11 cat.c -o s21_cat

clean: 
	rm s21_cat

rebuild: clean all

tests:
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat --number-nonblank $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -e $(FILE))
	-diff <(./s21_cat -E $(FILE)) <(cat -E $(FILE))
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat --number $(FILE)) <(cat --number $(FILE))
	-diff <(./s21_cat --squeeze-blank $(FILE)) <(cat --squeeze-blank $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -t $(FILE))
	-diff <(./s21_cat -T $(FILE)) <(cat -T $(FILE))