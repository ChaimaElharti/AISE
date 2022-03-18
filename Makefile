CC = gcc
LDFLAGS = -lSegFault
TARGET_EXEC := 


run: 
		$(CC) fromscratch_ptrace.c -o dbg $(LDFLAGS)

debug:
		./dbg -e $(TARGET_EXEC)

segf:
	$(CC) segfault1.c -g -rdynamic -o  Sgf1
	$(CC) segfault2.c -g -rdynamic -o  Sgf2
	$(CC) segfault3.c -g -rdynamic -o  Sgf3

debugSgf:
	./dbg -e Sgf1
	./dbg -e Sgf2
	./dbg -e Sgf3

sig:
	catchsegv ./Sgf1 -o sg1 
	catchsegv ./Sgf2 -o sg2 
	catchsegv ./Sgf3 -o sg3

.PHONY: clean

clean: 
	rm -f *.o run debug segf debugSgf sig