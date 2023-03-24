all: s21_grep

CC = gcc -Wall -Werror -Wextra

s21_grep: s21_grep.c
	$(CC) -o s21_grep s21_grep.c


clean:
	rm -rf *.o s21_grep 

rebuild: clean all
