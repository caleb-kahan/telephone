all: control.c main.c
	gcc -o control control.c
	gcc -o write main.c
