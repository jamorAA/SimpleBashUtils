CC = gcc -Wall -Werror -Wextra

all : s21_grep s21_grep.o clean

s21_grep.o : s21_grep.c s21_grep.h
	$(CC) -c s21_grep.c s21_grep.h

s21_grep : s21_grep.o
	$(CC) -o s21_grep s21_grep.o 

clean :
	rm -f s21_grep.o