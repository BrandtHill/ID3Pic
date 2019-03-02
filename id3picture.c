#include "id3picture.h"

int main(int argc, char * argv[]) {
	FILE *file;
	char i;
	char * frame;
	char * fileTail;
	char * fileHead;
	char outFileName[strlen(argv[1]) + 5];
	char prevHeader[10], header[10];
	long size;
	long tagStart;
	long fileTailLen;
	file = fopen(argv[1], "rb");

	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %lu\n", ftell(file));
		tagStart = ftell(file);
		fread(prevHeader, 1, 10, file);
		fseek(file, 0, SEEK_END);
		fileTailLen = ftell(file) - (tagStart + 10);
		printf("File position: %lu, and File Tail Length: %lu\n", ftell(file), fileTailLen);
		fseek(file, tagStart + 10, SEEK_SET);
		fileTail = malloc(fileTailLen);
		fread(fileTail, 1, fileTailLen, file);
		rewind(file);
		fileHead = malloc(tagStart);
		fread(fileHead, 1, tagStart, file);
		fclose(file);
		frame = constructPicFrame(argv[2], &size);
		printf("Pic frame size %lu\n", size);
		updateID3TagHeader(prevHeader, header, size);
		strcpy(outFileName, argv[1]);
		strcpy(strstr(outFileName, ".mp3"), "_out.mp3");
		printf("Output file name : %s\n", outFileName);
		file = fopen(outFileName, "wb+");
		printf("File pos: %lu\n", ftell(file));
		if (tagStart) fwrite(fileHead, 1, tagStart, file);
		printf("File pos: %lu\n", ftell(file));
		fwrite(header, 1, 10, file);
		printf("File pos: %lu\n", ftell(file));
		fwrite(frame, 1, size, file);
		printf("File pos: %lu\n", ftell(file));
		fwrite(fileTail, 1, fileTailLen, file);
		printf("File pos: %lu\n", ftell(file));
	} else printf("ID3 Tag not found.\n");
	
	fclose(file);
	return 0;
}

/**
 * Pass an open file with a tag to search for. Returns true if file 
 * contains tag, and the file will point at the position where the
 * tag begins, else return false.
 */ 
int fileContains(FILE * file, const char * tag) {
	int tagLen, i, data = 0;

	tagLen = strlen(tag);

	while (!feof(file)) {
		for (i = 0; i < tagLen; i++) {
			if (fgetc(file) != tag[i]) { // Not part of tag
				fseek(file, -i, SEEK_CUR);
				break;
			} else if (i + 1 == tagLen) { // Found complete tag
				fseek(file, -strlen(tag), SEEK_CUR);
				return 1;
			}
		}
	}
	return 0;
}

/**
 * Pass a filename of a jpeg or png, and a buffer will be allocated
 * that contains the APIC ID3 frame, and sizePtr will be set to the size
 * of the frame plus ten bytes for the frame header.
 */
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

/**
 * Pass the previous ID3 Tag header and size of new pic frame,
 * and function returns the updated tag header. It will fail if
 * the version is less than ID3v2.3, and the version will be 
 * bumped to ID3v2.4 just because.
 */
void updateID3TagHeader(const char * prevHeader, char * header, long picFrameSize) {
	long prevSize, size;
	strncpy(header, prevHeader, 10);
	if (header[3] < 3) exit(1);
	header[3] = 4;
	prevSize = 	(header[6] << 21) +
				(header[7] << 14) +
				(header[8] << 7) +
				(header[9]);
	size = prevSize + picFrameSize;
	header[6] = (size >> 21) & 0x7F;
	header[7] = (size >> 14) & 0x7F;
	header[8] = (size >> 7) & 0x7F;
	header[9] = (size) & 0x7F;
}

/**
 * Returns true if filename that of a jpeg file, else returns false.
 */
int picIsJpg(const char * picFilename) {
	int i;
	char lower[strlen(picFilename)];
	strcpy(lower, picFilename);
	for (i = 0; lower[i]; i++) lower[i] = tolower(lower[i]);
	return (strstr(lower, "jpg") != NULL || strstr(lower, "jpeg") != NULL) ? 1 : 0;
}
