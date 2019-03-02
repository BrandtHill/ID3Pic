#include "id3picture.h"

int main(int argc, char * argv[]) {
	FILE * file = fopen("test/NEWFILE.dat", "w");
	char * frame, * fileTail, * fileHead, * prevHeader, * header;
	long size, tagStart, fileTailLen;

	prevHeader = readInFile(argv[1], &fileTailLen, &tagStart, fileTail, fileHead);
	frame = constructPicFrame(argv[2], &size);
	header = updateID3TagHeader(prevHeader, size);
	writeOutFile(argv[1], fileTailLen, tagStart, size, fileTail, fileHead, frame, header);
	return 0;
}

/**
 * Pass audio file name and pass unassigned pointers for file tail length, ID3 tag start,
 * file tail, and file head that will each be set. Function will return the previous header.
 */
char * readInFile(const char * filename, long * fileTailLen, long * tagStart, char * fileTail, char * fileHead) {
	FILE * file = fopen(filename, "rb");
	char * prevHeader = malloc(10);

	if (file == NULL) {
		printf("Audio file not opened successfully. Exiting.\n");
		exit(1);
	}
	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %lu\n", *tagStart = ftell(file));
		fread(prevHeader, 1, 10, file);
		fseek(file, 0, SEEK_END);
		fileTail = malloc(*fileTailLen = ftell(file) - (*tagStart + 10));
		fseek(file, *tagStart + 10, SEEK_SET);
		fread(fileTail, 1, *fileTailLen, file);
		rewind(file);
		fileHead = malloc(*tagStart);
		fread(fileHead, 1, *tagStart, file);
		fclose(file);
	} else {
		fclose(file);
		printf("No ID3 Tag found. Ensure your file has an existing ID3v2.3 or v2.4 tag.\n");
		exit(0);
	}

	return prevHeader;
}

/**
 * Pass audio file name, file tail length, tag start, frame size, 
 * file tail, file head, frame, and updated header. The new file 
 * will be written whose name is appended with '_out'
 */
void writeOutFile(const char * filename, long fileTailLen, long tagStart, long size, char * fileTail, char * fileHead, char * frame, char * header) {
	FILE * file;// = fopen("test/NEWFILE.dat", "w");
	char outFileName[strlen(filename) + 5];

	strcpy(outFileName, filename);
	strcpy(strstr(outFileName, ".mp3"), "_out.mp3");
	printf("Output file name : %s\n", outFileName);
	file = fopen(outFileName, "wb+");
	if (file == NULL) {
		printf("New audio file not created successfully. Exiting.\n");
		exit(1);
	}
	if (tagStart) fwrite(fileHead, 1, tagStart, file);
	fwrite(header, 1, 10, file);
	fwrite(frame, 1, size, file);
	fwrite(fileTail, 1, fileTailLen, file);
	fclose(file);
}

/**
 * Pass an open file with a tag to search for. Returns true if file 
 * contains tag, and the file will point at the position where the
 * tag begins, else return false.
 */ 
int fileContains(FILE * file, const char * tag) {
	int i, data = 0, tagLen = strlen(tag);

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
	unsigned char * picBuffer, * frame;
	unsigned char frameHeader[10] = "APIC\0\0\0\0\0\0";
	FILE * picFile = fopen(picFilename, "rb");;

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
	printf("Pic frame size %lu\n", *sizePtr);
	return frame;
}

/**
 * Pass the previous ID3 Tag header and size of new pic frame,
 * and function returns the updated tag header. It will fail if
 * the version is less than ID3v2.3, and the version will be 
 * bumped to ID3v2.4 just because.
 */
char * updateID3TagHeader(const char * prevHeader, long picFrameSize) {
	long prevSize, size;
	char * header = malloc(10);
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
	return header;
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
