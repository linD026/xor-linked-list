all:
	gcc -o xorlist test_xorlist.c -g -Wall
clean:
	rm -f xorlist
