#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// For info on ID3 tags
// http://id3lib.sourceforge.net/id3/id3v2.3.0.html
// http://id3lib.sourceforge.net/id3/id3v2.4.0-structure.txt

int fileContains(FILE * file, const char * tag);

char * constructPicFrame(const char * picFilename, long * sizePtr);

char * updateID3TagHeader(const char * prevHeader, long picFrameSize);

int picIsJpg(const char * picFilename);