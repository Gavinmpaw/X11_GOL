main: main.c
	gcc main.c -lX11 -o GOL

clean:
	rm GOL
