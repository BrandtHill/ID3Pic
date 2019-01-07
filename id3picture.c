#include "id3picture.h"

int main(int argc, char * argv[]) {
	FILE *file;
	char i;
	unsigned char * frame;
	long size;
	file = fopen(argv[1], "rb");

	if (fileContains(file, "ID3")) {
		printf("Found ID3 Tag. Position %lu\n", ftell(file) - strlen("ID3"));
	} else printf("ID3 Tag not found.\n");
	
	if (fileContains(file, "APIC")) {
		printf("Found APIC Tag. Position %lu\n", ftell(file) - strlen("APIC"));
	} else printf("APIC Tag not found.\n");
		
	fclose(file);

	file = fopen("test/PicFrameTest", "wb");
	frame = constructPicFrame("test/Catnip.png", &size);
	fwrite(frame, 1, size, file);
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

char * constructPicFrame(const char * picFilename, long * sizePtr) {
	int jpg, i;
	long picSize, frameSize;
	unsigned char * picBuffer;
	unsigned char * frame;
	unsigned char frameHeader[10] = "APIC\0\0\0\0\0\0";
	FILE * picFile;
	jpg = 0;

	picFile = fopen(picFilename, "rb");
	fseek(picFile, 0, SEEK_END);
	picSize = ftell(picFile);
	rewind(picFile);
	
	picBuffer = malloc(picSize);
	fread(picBuffer, 1, picSize, picFile);
	fclose(picFile);

	frameSize = picSize + 13 + jpg;

	frameHeader[4] = (frameSize >> 21) & 0x7F;
	frameHeader[5] = (frameSize >> 14) & 0x7F;
	frameHeader[6] = (frameSize >> 7) & 0x7F;
	frameHeader[7] = frameSize & 0x7F;

	frame = malloc(frameSize);

	memcpy(frame, frameHeader, sizeof(frameHeader));
	memcpy(frame + sizeof(frameHeader), "\0image/png\0\3\0", 13);
	memcpy(frame + sizeof(frameHeader) + 13, picBuffer, picSize);

	*sizePtr = frameSize + 10;
	return frame;
}
