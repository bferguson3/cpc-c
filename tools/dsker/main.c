#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //toupper
#include <math.h> //floor

#include "dsker.h"

#define DEBUG 

//int activeDskSize;

int main(int numargs, const char* args[])
{
    #ifdef DEBUG
    for(int c = 0; c < numargs; c++)
        printf("Command line arg %d: %s\n", c, args[c]);
    #endif 
    if(numargs == 1)
    {
        usage();
        return 1;
    }
    
    if(!strcmp(args[1], "new"))
    {
        // CREATE NEW DISK 
        if(numargs < 3)
        {
            usage();
            return 1;
        }
        DSKFile* newdisk = make_new_dsk(40, 9, 512);
        int success = write_dsk_file(args[2], newdisk, 0);
        if(success) 
            printf("Successfully created blank disk: %s.\n", args[2]);
        else 
            printf("Failed to create disk: %s.\n", args[2]);
    }
    else if(!strcmp(args[1], "add"))
    {
        // ADD FILE TO DISK 
        if(numargs < 4)
        {
            usage();
            return 1;
        }
        if(strlen(args[3]) > 12)
            printf("Warning: Embedded file length greater than 8+3 characters. Truncating.\n");
        char* diskbytes = read_disk(args[2]);
        if(diskbytes == NULL){
            printf("Failed to add file.\n");
            return 1;
        }


        // DSKFile* needs to point to the laoded disk, and needs to be populated appropriately.
        DSKFile* cur_dsk = (DSKFile*)malloc(sizeof(DSKFile));
        cur_dsk->bytes = diskbytes;
        long disksize = get_filesize(args[2]);
        cur_dsk->totalSize = disksize;
        printf("Disk size is %d bytes.\n", cur_dsk->totalSize);
        DSKHeader* cur_hdr = (DSKHeader*)&cur_dsk->bytes[0];
        cur_dsk->header = cur_hdr;
        #ifdef DEBUG 
        printf("DSK Header string:\n%s\n", cur_dsk->header->headerStr);
        #endif 
        
        int fnlen = strlen(args[3]);
        int newfsize = get_filesize(args[3]);

        // Set up track list 
        int numtrax;
        int trsize;
        numtrax = cur_dsk->header->numTracks;
        trsize = cur_dsk->header->trackSize[0] + (cur_dsk->header->trackSize[1]*256);
        // get basic disk structure 
        // Allocate and direct the cur dsk track pointers
        cur_dsk->tracks = (trackHeader**)malloc(sizeof(trackHeader*) * numtrax);
        trackHeader* t = (trackHeader*)&cur_dsk->bytes + 0x100;
        cur_dsk->tracks[0] = t;
        for(int i = 0; i < numtrax; i++){
            t = (trackHeader*)&cur_dsk->bytes[0x100 + (i * trsize)];
            cur_dsk->tracks[i] = t;
        }
        char* filebytes = read_file_bytes(args[3]);
        

        //TODO: fix for multiple files/tracks and if too many files
        
        int ofs = get_next_empty_fs(cur_dsk->bytes);
        DOSFile* next_f = (DOSFile*)(cur_dsk->bytes + ofs);
        next_f->null_a = 0;
        next_f->null_b[0] = 0;
        next_f->null_b[1] = 0;
        next_f->bam = (unsigned char)(newfsize/36);
        // NOW FIND ACTUAL BYTE LOCATION FOR THE FILE
        // first populate track meta bam from dos file list
        pop_tracklist(cur_dsk);
        // then find, starting at track 0 sector 4, the first unpopulated sector
        // start at a00, then increase by (sector size * 2)
        int tgt_sec = 2; // times 2 = 4 = a00
        printf("target sector: %d", tgt_sec);
        //int trloc = 2; (IN SECTOR IDS, SECTOR COUNTS START FROM 0)
        int tgt_track = (int)floor((tgt_sec*2) / 9);
        tgt_track++;
        printf("sector: %d\ntrack: %d\n", tgt_sec, tgt_track);
        next_f->tracklist[0] = tgt_sec;
        for(int i = 1; i < 16; i++)
            next_f->tracklist[i] = 0;
        // SECTOR ID2 = SECTOR 4 = TRACK 0, SECTOR 4 (5th sector) == (0x100 + (0x200*4) + (0x100*(curtrack+1))

        // Parse filename, remove period, cat to 8+3
        int max = 8;
        int i;        
        if(max > fnlen) max = fnlen;
        // set filename
        for(i = 0; i < max; i++) {
            if(args[3][i] == '.')
                break;
            next_f->fileName[i] = toupper(args[3][i]);
        }
        while(args[3][i] == '.') i++; //skip .
        // set extension
        max = 3;
        fnlen -= i;
        if(fnlen > 0){
            if(max > fnlen) max = fnlen;
            for(int j = 0; j < max; j++) next_f->fileExt[j] = toupper(args[3][i+j]);
        }
        // replace non ascii with null
        for(i = 0; i < 11; i++){
            if(next_f->fileName[i] < 0x30) next_f->fileName[i] = 0x20;
            if(next_f->fileName[i] > 0x7f) next_f->fileName[i] = 0x20;
        }
        #ifdef DEBUG 
        printf("Writing file: "); // to make sure the filename is right
        for(i = 0; i < 13; i++)
            printf("%c", next_f->fileName[i]);
        printf("...\n");
        #endif 

        
        // PART if > sectorsize in size 
        if(newfsize > trsize){
            printf("File is larger than 1 track, try again later\n");
            return 1;
        }
        next_f->filePart = 0;
        
        // TODO: files bigger than 1 sector 
        unsigned char headers_to_skip = floor(tgt_sec / 9);
        int fofs = (0x100 + ((0x200*2)*tgt_sec) + (0x100*(tgt_track)));
        printf("tgt loc: %x\n sec size %x\n", fofs, 128<<(cur_dsk->tracks[0]->sectorSize));
        char* cc = cur_dsk->bytes + fofs;
        for(int p = 0; p < newfsize; p++)
            *cc++ = filebytes[p];

#define OVERWRITE 1
        int tf = write_dsk_file(args[2], cur_dsk, OVERWRITE);
        if(tf) 
            printf("Successfully added file\n");
        else 
            printf("File add failed\n");
        
    }
    return 0;
}

void usage(void)
{
    printf("dsker v0.2\nUsage:\n$ ./dsker new <diskname>\n  Where diskname = name of new empty CPC DSK to make.\n");
    printf("        OR:   \n$ ./dsker add <diskname> <filename>\n  Where filename = file to add to disk.\n\n");
    printf(" CURRENT LIMITATIONS: Can only add 1 file. File must be < 4.5kB.\n");
}

