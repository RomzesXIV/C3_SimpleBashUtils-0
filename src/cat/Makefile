all: s21_cat

CC = gcc -Wall -Werror -Wextra

s21_cat: s21_cat.c
	$(CC) -o s21_cat s21_cat.c


clean:
	rm -rf *.o s21_cat 

rebuild: clean all
