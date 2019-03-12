#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// For info on ID3 tags
// http://id3lib.sourceforge.net/id3/id3v2.3.0.html
// http://id3lib.sourceforge.net/id3/id3v2.4.0-structure.txt

typedef struct {
    char * preTagBuf;   // Anything before ID3 tag
    char * header;      // ID3 tag header
    char * prevFrames;  // All frames of existing tag
    char * fileTail;    // Anything after ID3 tag (may include tag's zero-padding)
} FileChunks;

FileChunks * readInFile(const char * audioFilename);

void writeOutFile(const char * audioFilename, FileChunks * chunks, char * picFrame);

int fileContains(FILE * file, const char * tag);

char * constructPicFrame(const char * imageFilename);

char * updateID3TagHeader(const char * prevHeader);

int picIsJpg(const char * imageFilename);

long skipFrames(FILE * file);

int getVersion(const char * audioFilename);