#include "id3picture.h"

int main(int argc, char * argv[]) {
	FILE *file;
	char i;
	char * frame;
	char * fileTail;
	char * header;
	char * fileHead;
	char prevHeader[10];
	long size;
	long tagStart;
	long fileTailLen;
	file = fopen(argv[1], "rb");

	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %lu\n", ftell(file) - strlen("ID3"));
		fseek(file, -strlen("ID3"), SEEK_CUR);
		tagStart = ftell(file);
		fread(prevHeader, 1, 10, file);
		fseek(file, 0, SEEK_END);
		fileTailLen = ftell(file) - tagStart;
		fseek(file, tagStart, SEEK_SET);
		fileTail = malloc(fileTailLen);
		fread(fileTail, 1, fileTailLen, file);
		rewind(file);
		fileHead = malloc(tagStart);
		fread(fileHead, 1, tagStart, file);
		fclose(file);
		frame = constructPicFrame(argv[2], &size);
		header = updateID3TagHeader(prevHeader, size);
		file = fopen("test/out.mp3", "wb+");
		if (tagStart) fwrite(fileHead, 1, tagStart, file);
		fwrite(header, 1, 10, file);
		fwrite(frame, 1, size, file);
		fwrite(fileTail, 1, fileTailLen, file);
	} else printf("ID3 Tag not found.\n");
	
	fclose(file);
	return 0;
}

int fileContains(FILE * file, const char * tag) {
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

char * constructPicFrame(const char * picFilename, long * sizePtr) {
	int i, jpg;
	long picSize, frameSize;
	unsigned char * picBuffer;
	unsigned char * frame;
	unsigned char frameHeader[10] = "APIC\0\0\0\0\0\0";
	FILE * picFile;

	picFile = fopen(picFilename, "rb");
	fseek(picFile, 0, SEEK_END);
	picSize = ftell(picFile);
	rewind(picFile);
	
	picBuffer = malloc(picSize);
	fread(picBuffer, 1, picSize, picFile);
	fclose(picFile);

	jpg = picIsJpg(picFilename);
	frameSize = picSize + 13 + jpg;

	frameHeader[4] = (frameSize >> 21) & 0x7F;
	frameHeader[5] = (frameSize >> 14) & 0x7F;
	frameHeader[6] = (frameSize >> 7) & 0x7F;
	frameHeader[7] = (frameSize) & 0x7F;

	frame = malloc(frameSize);

	memcpy(frame, frameHeader, sizeof(frameHeader));
	memcpy(frame + sizeof(frameHeader), jpg ? "\0image/jpeg\0\3\0" : "\0image/png\0\3\0", 13 + jpg);
	memcpy(frame + sizeof(frameHeader) + 13 + jpg, picBuffer, picSize);

	*sizePtr = frameSize + 10;
	return frame;
}

char * updateID3TagHeader(const char * prevHeader, long picFrameSize) {
	char * header;
	long prevSize, size;
	header = malloc(10);
	strcpy(header, prevHeader);
	if (header[3] < 3) exit(1);
	prevSize = 	(header[6] << 21) +
				(header[7] << 14) +
				(header[8] << 7) +
				(header[9]);
	size = prevSize + picFrameSize;
	header[6] = (size >> 21) & 0x7F;
	header[7] = (size >> 14) & 0x7F;
	header[8] = (size >> 7) & 0x7F;
	header[9] = (size) & 0x7F;
	return header;
}

int picIsJpg(const char * picFilename) {
	int i;
	char lower[strlen(picFilename)];
	strcpy(lower, picFilename);
	for (i = 0; lower[i]; i++) lower[i] = tolower(lower[i]);
	return (strstr(lower, "jpg") != NULL || strstr(lower, "jpeg") != NULL) ? 1 : 0;
}
