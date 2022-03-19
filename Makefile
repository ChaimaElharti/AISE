CC = gcc
LDFLAGS = -lSegFault
TARGET_EXEC := 

all: run segf debugSgf 

run: 
		$(CC) src/dbg.c -o dbg $(LDFLAGS)

debug:
		./dbg -e $(TARGET_EXEC)

segf:
	$(CC) test/segfault1.c -g -rdynamic -o  Sgf1
	$(CC) test/segfault2.c -g -rdynamic -o  Sgf2
	$(CC) test/segfault3.c -g -rdynamic -o  Sgf3
	$(CC) test/infiniteLoop.c -g -rdynamic -o  loop 
	$(CC) test/test.c -g -rdynamic -o  stacksmashing

debugSgf:
	./dbg -e Sgf1
	./dbg -e Sgf2
	./dbg -e Sgf3
	./dbg -e loop
	./dbg -e stacksmashing

sig:
	catchsegv ./Sgf1 -o sg1 
	catchsegv ./Sgf2 -o sg2 
	catchsegv ./Sgf3 -o sg3
	catchsegv ./loop -o lo 
	catchsegv ./stacksmashing -o sm 

.PHONY: clean

clean: 
	rm -f *.o run debug segf debugSgf sig