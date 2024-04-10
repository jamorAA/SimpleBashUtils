CC = gcc -Wall -Werror -Wextra

all : s21_cat s21_cat.o clean

s21_cat.o : s21_cat.c s21_cat.h
	$(CC) -c s21_cat.c s21_cat.h

s21_cat : s21_cat.o
	$(CC) -o s21_cat s21_cat.o 

clean :
	rm -f s21_cat.o