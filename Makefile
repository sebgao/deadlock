run:
	gcc -pthread -o dl deadlock.c
	./dl
hidden:
	gcc -pthread -o dl hiddendeadlock.c
	./dl
norm:
	gcc -pthread -o dl normdeadlock.c
	./dl