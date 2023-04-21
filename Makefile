main: main.c
	@gcc main.c -lX11 -o GOL

run:
	@make main
	@./GOL

clean:
	rm GOL
