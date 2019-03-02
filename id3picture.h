#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// For info on ID3 tags
// http://id3lib.sourceforge.net/id3/id3v2.3.0.html
// http://id3lib.sourceforge.net/id3/id3v2.4.0-structure.txt

char * readInFile(const char * filename, long * fileTailLen, long * tagStart, char * fileTail, char * fileHead);

void writeOutFile(const char * filename, long fileTailLen, long tagStart, long size, char * fileTail, char * fileHead, char * frame, char * header);

int fileContains(FILE * file, const char * tag);

char * constructPicFrame(const char * picFilename, long * sizePtr);

char * updateID3TagHeader(const char * prevHeader, long picFrameSize);

int picIsJpg(const char * picFilename);