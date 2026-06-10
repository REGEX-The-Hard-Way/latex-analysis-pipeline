#CC=clang
#CFLAGS=-g -fsanitize=undefined -fno-sanitize-recover=all

CC=gcc
CFLAGS=-g 
scanner:
	ragel -m -G2 scanner.rl
	$(CC)  -O2  scanner.c main.c murmur3.c regex_util.c -o scanner.out -lm

sent_split:
	ragel -m -G2 sent_split.rl
	$(CC) -DTEST -O2  sent_split.c -o sent_split.out

clean:
	rm scanner.out sent_split.out	
