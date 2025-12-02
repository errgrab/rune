all: vm

vm: main.c
	cc main.c -o vm
