# ID3Pic
ID3Pic is a Command LIne Tool for adding (and eventually replacing) album cover images to files with ID3 Metadata tags, like MP3.

## Purpose
I wanted a tool to put album covers on my tunes. A tool that I used to use would sometimes clobber a bit of audio data at the start, which would take away the punch when there was a kick drum at the beginning.
There exist plenty of other actual tools that work properly, but I just wanted to do something in with file I/O in C. There exist plenty of ID3 libraries, but that would defeat the purpose of me making something from scratch.

## How to Build
For Windows: ```make```

For Linux: ```make linux```

I developed this on a Windows 7 machine with Cygwin gcc, meaning old C89 style C. Cygwin is terrible.

## How to Use
```./id3pic <MP3_AUDIO_FILE> <JPG_OR_PNG_IMAGE_FILE>```

If you input something like ```BigBass.mp3``` and ```AlbumCover.png```, a new file named ```BigBass_out.mp3``` will be produced with your cover photo attached to it. Hurray!

## What is ID3?
ID3 is a format for metadata tags used in audio files. An ID3 tag contains one or more frame where each frame has a chunk of metadata. 
This is where items like 'Contributing artists' and 'Title' are stored, which you've probably seen on your MP3s, each of which are stored in a particular frame. 
It's also where album covers are stored, specifically in an 'APIC' frame. The current versions that are more or less in use today are 3 and 4, [ID3v2.3](http://id3lib.sourceforge.net/id3/id3v2.3.0.html) and [ID3v2.4](http://id3lib.sourceforge.net/id3/id3v2.4.0-structure.txt).
Currently this tool supports both these versions and complies with whatever the input file has. FL Studio 12 (probably 20 too, haven't updated because lazy...?) currently outputs with version 3.
