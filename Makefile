all: main.c
	@gcc main.c -lX11 -o GOL

run:
	@make all
	@./GOL

clean:
	rm GOL
