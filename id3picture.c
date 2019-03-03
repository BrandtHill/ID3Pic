#include "id3picture.h"

long tagStart;
long picFrameStart;
long prevSize;
long fileTailLen;
long picFrameSize;
long prevFramesLen;
int version; //ID3v2.x where x should be 3 or 4

int main(int argc, char * argv[]) {
	FILE * file = fopen("test/DELETEME", "w"); //Cygwin is terrible, errors out more often without this (???)
	char * picFrame, * fileTail, * prevFrames, * preTagBuf, * prevHeader, * header;
	
	header = readInFile(argv[1], &fileTail, &prevFrames, &preTagBuf);
	picFrame = constructPicFrame(argv[2]);
	writeOutFile(argv[1], preTagBuf, header, prevFrames, picFrame, fileTail);
	return 0;
}

/**
 * Pass audio file name and pass unassigned pointers for file tail length, ID3 tag start,
 * file tail, and file head that will each be set. Function will return the new header.
 */
char * readInFile(const char * filename, char ** fileTail, char ** prevFrames, char ** preTagBuf) {
	FILE * file = fopen(filename, "rb");
	char * prevHeader = malloc(10);
	char * newHeader;
	long prevSize;
	int i;

	if (file == NULL) {
		printf("Audio file not opened successfully. Exiting.\n");
		exit(1);
	}
	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %lu\n", tagStart = ftell(file));
		rewind(file);
		*preTagBuf = malloc(tagStart);
		fread(*preTagBuf, 1, tagStart, file);
		fread(prevHeader, 1, 10, file);
		newHeader = updateID3TagHeader(prevHeader);
		picFrameStart = skipFrames(file);
		fseek(file, 0, SEEK_END);
		*fileTail = malloc(fileTailLen = (ftell(file) - picFrameStart));
		fseek(file, picFrameStart, SEEK_SET);
		fread(*fileTail, 1, fileTailLen, file);
		*prevFrames = malloc(prevFramesLen = (picFrameStart - (tagStart + 10)));
		fseek(file, tagStart + 10, SEEK_SET);
		fread(*prevFrames, 1, prevFramesLen, file);
		fclose(file);
	} else {
		fclose(file);
		printf("No ID3 Tag found. Ensure your file has an existing ID3v2.3 or v2.4 tag.\n");
		exit(0);
	}

	return newHeader;
}

/**
 * Pass audio file name, file tail length, tag start, frame size, 
 * file tail, file head, frame, and updated header. The new file 
 * will be written whose name is appended with '_out'
 */
void writeOutFile(const char * filename, char * preTagBuf, char * header, char * prevFrames, char * picFrame, char * fileTail) {
	FILE * file;
	char outFileName[strlen(filename) + 5];

	strcpy(outFileName, filename);
	strcpy(strstr(outFileName, ".mp3"), "_out.mp3");
	printf("Output file name : %s\n", outFileName);
	file = fopen(outFileName, "wb+");
	if (file == NULL) {
		printf("New audio file not created successfully. Exiting.\n");
		exit(1);
	}
	printf("File pos, start: %lu\n", ftell(file));
	if (tagStart) fwrite(preTagBuf, 1, tagStart, file);
	printf("File pos, preTagBuf: %lu\n", ftell(file));
	fwrite(header, 1, 10, file);
	printf("File pos, Header: %lu\n", ftell(file));
	fwrite(prevFrames, 1, prevFramesLen, file);
	printf("File pos, PrevFrames: %lu\n", ftell(file));
	fwrite(picFrame, 1, picFrameSize, file);
	printf("File pos, PicFrame: %lu\n", ftell(file));
	fwrite(fileTail, 1, fileTailLen, file);
	printf("File pos, FileTail: %lu\n", ftell(file));
	fclose(file);
	printf("Successfully wrote output file with picture.\n");
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
char * constructPicFrame(const char * picFilename) {
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

	if (version == 4) {
		frameHeader[4] = (frameSize >> 21) & 0x7F;
		frameHeader[5] = (frameSize >> 14) & 0x7F;
		frameHeader[6] = (frameSize >> 7) & 0x7F;
		frameHeader[7] = (frameSize) & 0x7F;
	}
	else {
		frameHeader[4] = (frameSize >> 24);
		frameHeader[5] = (frameSize >> 16);
		frameHeader[6] = (frameSize >> 8);
		frameHeader[7] = (frameSize);
	}
	frame = malloc(frameSize);

	memcpy(frame, frameHeader, sizeof(frameHeader));
	memcpy(frame + sizeof(frameHeader), jpg ? "\0image/jpeg\0\3\0" : "\0image/png\0\3\0", 13 + jpg);
	memcpy(frame + sizeof(frameHeader) + 13 + jpg, picBuffer, picSize);

	picFrameSize = frameSize + 10;
	printf("Pic frame size %lu\n", picFrameSize);
	return frame;
}

/**
 * Pass the previous ID3 Tag header and size of new pic frame, and 
 * function returns the updated tag header. It will fail if the version, 
 * is less than ID3v2.3, and the global version will be set to 3 or 4 
 * (v4 uses sync-safe ints in frame size, v3 doesn't).
 */
char * updateID3TagHeader(const char * prevHeader) {
	long size;
	char * header = malloc(10);
	int i;
	memcpy(header, prevHeader, 10);
	version = header[3];
	if (version < 3) exit(1);
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

/**
 * Pass the opened file position at the start of the first frame of the tag, the 
 * start position of the tag, and the size of the previous (existing) ID3 tag.
 * Once the header of the ID3 tag has been read, scan through each of the existing
 * by reader each frame header then skipping it, until we either get to the zero-padding
 * of the tag or we get to the MP3 audio frames. Returns the position of the end of the last
 * frame of the tag, which the file pointer will still be positioned at.
 */
long skipFrames(FILE * file) {
	char header[10];
	long size;

	while (1) {
		printf("Position in ID3 Tag Frames: %lu\n", ftell(file));
		fread(header, 1, 10, file);
		if ((header[0] | header[1] | header[2] | header[3]) && ((ftell(file) - 10 - tagStart) < prevSize)) {
			if (version == 4) {
				size =	(header[4] << 21) +
						(header[5] << 14) +
						(header[6] << 7) +
						(header[7]);
			} else {
				size =	(header[4] << 24) +
						(header[5] << 16) +
						(header[6] << 8) +
						(header[7]);
			}
			fseek(file, size, SEEK_CUR);
		} else {
			fseek(file, -10, SEEK_CUR);
			printf("Final Position of existing ID3 Tag Frames: %lu\n", ftell(file));
			return ftell(file);
		}
	}
	return 0;
}
