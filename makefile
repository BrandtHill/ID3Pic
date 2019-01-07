CC = gcc

id3pic : id3picture.c id3picture.h
	$(CC) -o id3pic id3picture.c
	mv id3pic.exe id3pic

.PHONY : clean

clean : 
	rm -f id3pic *.exe *.core
