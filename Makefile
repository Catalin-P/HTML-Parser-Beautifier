build:
	gcc -Wall -g main.c Parser.c stackCustom.c tree.c write.c deleteRec.c override.c append.c -o tema3
run: 
	./tema3
clean:
	rm tema3
