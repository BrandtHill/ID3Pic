#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// For info on ID3 tags
// http://id3lib.sourceforge.net/id3/id3v2.3.0.html
// http://id3lib.sourceforge.net/id3/id3v2.4.0-structure.txt

char * readInFile(const char * filename, char ** fileTail, char ** prevFrames, char ** preTagBuf);

void writeOutFile(const char * filename, char * preTagBuf, char * header, char * prevFrames, char * picFrame, char * fileTail);

int fileContains(FILE * file, const char * tag);

char * constructPicFrame(const char * picFilename);

char * updateID3TagHeader(const char * prevHeader);

int picIsJpg(const char * picFilename);

long skipFrames(FILE * file);

int getVersion(const char * filename);