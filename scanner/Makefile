#CC=clang
#CFLAGS=-g -fsanitize=undefined -fno-sanitize-recover=all

CC=gcc
CFLAGS=-g -I../include
scanner:
	ragel -m -G2 scanner.rl
	$(CC)  -O2  scanner.c main.c murmur3.c file_mmap.c ../include/regex_util.c -o scanner.out -lm

sent_split:
	ragel -m -G2 sent_split.rl
	$(CC) -DTEST -O2  sent_split.c -o sent_split.out

macro_expander:
	ragel -m -G2 expand_macros.rl
	$(CC) -O2 -I../include macro_expander.c ../include/regex_util.c -o macro_expander.out

build_macros: macro_expander

get_sample_data:
	wget https://www.cs.cornell.edu/projects/kddcup/download/hep-th-2003.tar.gz --no-check-certificate
	tar -xf hep-th-2003.tar.gz
	cd 2003 && find . -type f |grep -v tex$ |xargs -i mv "{}" "{}".tex 
	gcc strip_non_ascii.c -o strip_non_ascii.out
	bash pre-process-dataset.sh

clean:
	rm -f scanner.out sent_split.out scanner.c sent_split.c macro_expander.out
