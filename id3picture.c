#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
	FILE *file;
	char c;
	file = fopen(argv[1], "rb");

	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %d\n", ftell(file) - strlen("ID3"));
	} else printf("Tag not found. Position %d\n", ftell(file));
	
	if (fileContains(file, "PIC")) {
		printf("Found PIC Tag. Position %d\n", ftell(file) - strlen("PIC"));
	} else printf("Tag not found. Position %d\n", ftell(file));
		
	fclose(file);
	return 0;
}

int fileContains(FILE *file, const char* tag) {
	int tagLen, i, data = 0;

	tagLen = strlen(tag);

	while (!feof(file)) {
		for (i = 0; i < tagLen; i++) {
			if (fgetc(file) != tag[i]) {
				if (i) fseek(file, -i, SEEK_CUR);
				break;
			} else if (i + 1 == tagLen) return 1;
		}
	}
	return 0;
}

char * constructPicFrame(char *picFilename) {
	int jpg, png;
	long picSize, frameSize;
	char * picBuffer, * frame;
	char frameHeader[10] = "APIC\0\0\0\0\0\0";
	FILE * picFile;
	
	picFile = fopen(picFilename, "rb");
	fseek(picFile, 0, SEEK_END);
	picSize = ftell(picFile);
	rewind(picFile);
	
	picBuffer = malloc(picSize + 1);
	fread(picBuffer, picSize, 1, picFile);
	fclose(picFile);

	picBuffer[picSize] = 0;

	return frame;
}
