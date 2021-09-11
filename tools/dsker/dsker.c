#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
//

#include "dsker.h"


#define PRINT_LAST_ERROR printf("Error #%d: %s\n", errno, strerror(errno));


char* read_disk(const char* fn)
{
    char* diskBytes = read_file_bytes(fn);
    if (diskBytes == NULL)
        return NULL;
    #ifdef DEBUG 
    //printf("Loaded DSK filesize: %d\n", activeDskSize);
    #endif 
    
    DSKHeader* dhead = (DSKHeader*)&diskBytes[0];
    //printf("%s\n",dhead->headerStr);
    if(strcmp(dhead->headerStr, "MV - CPCEMU Disk-File\r\nDisk-Info\r\n")) 
    {
        printf("Warning: Disk header string not correct. Is this a DSK file?\n");
        
    }
    printf("DSK Created by: %s\n", dhead->dskCreator);
    printf("Num. tracks: %d\n", dhead->numTracks);
    printf("Sides: %d", dhead->numSides);
    if(dhead->numSides == 1)printf(" (SD)\n");
    else if(dhead->numSides == 2)printf(" (2D)\n");
    printf("Track size: %d\n", dhead->trackSize[0] + dhead->trackSize[1]*256);
    /*
    trackHeader* t1 = (trackHeader*)&diskBytes[0x100];
    //printf("%s\n", t1->trackStr);
    if(strcmp(t1->trackStr, "Track-Info\r\n")) 
    {
        printf("Warning: Track header string not correct. DSK file corrupted?\n");
        
    }
    printf("Track no: %d\n", t1->trackNo);
    printf("Side: %d\n", t1->sideNo);
    printf("Sector size (b): %d\n", 128 << t1->sectorSize);
    printf("# of sectors: %d\n", t1->numSectors);
    printf("gap3: %d\n", t1->gap3);
    sectorHeader* sh = (sectorHeader*)&diskBytes[0x118];
    for(int t = 0; t < t1->numSectors; t++)
    {
        printf("-Sector ID: %d \n", sh->R & 0b00011111);
        if((t + 1) != (sh->R & 0x1f)) printf("Warning: sector ID out of order. Bad image?\n");
        sh++;
    }
    DOSFile* df = (DOSFile*)&diskBytes[0x200];
    while(df->null_a == 0)
    {
        printf("File entry: %s (Part %d)(Sector used: %d)\n", df->fileName, df->filePart, df->bam<<2);
        if(df->bam & 0b00001110 && (df->bam < 16)) printf("Maybe BASIC file. Careful!\n");
        df++;
    }
    // loop next track
    t1 = (trackHeader*)&diskBytes[0x100 + 0x100 + ((128 << t1->sectorSize) * t1->numSectors)];
    if(strcmp(t1->trackStr, "Track-Info\r\n")) 
    {
        printf("Warning: Track header string not correct. DSK file corrupted?\n");
        //return 1;
    }
    */
    //DOSFile* next_f = get_next_empty_fs(diskBytes);
    //next_f->null_a = 0;
    //next_f->fileName = 
    return diskBytes;
}

int get_next_empty_fs(char* bytes)
{
    int ofs = 0x200;
    DOSFile* f = (DOSFile*)bytes + ofs;
    while(f->null_a == 0) {
        ofs += (sizeof(DOSFile));
        f++;
    }
    printf("Next empty file location: 0x%x\n", ofs);
    return ofs;
}

int write_dsk_file(const char* fn, DSKFile* disk, int overwrite)
{
    if(!overwrite){
        if((access(fn, F_OK) != -1)){
            printf("Error: File already exists!\n");
            return 0;
        }
    }
    FILE* f;
    f = fopen(fn, "wb");
    if(f == NULL){
        PRINT_LAST_ERROR
        return 0;
    }
    int w = fwrite(disk->bytes, 1, disk->totalSize, f);
    fclose(f);
    if(w != disk->totalSize){
        printf("Disk write failed (%d): %s\n", errno, strerror(errno));
        return 0;
    }
    return 1;
}

long get_filesize(const char* fn)
{
    FILE* f;
    if((access(fn, F_OK) == -1)){
        printf("Error: File %s not found.\n", fn);
        return -1;
    }
    f = fopen(fn, "rb");
    if(f == NULL)
    {
        PRINT_LAST_ERROR
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long flen = ftell(f);
    fclose(f);
    return flen;
}

char* read_file_bytes(const char* fn)
{
    char* buf;
    int flen;
    FILE* f;
    if((access(fn, F_OK) == -1)){
        printf("Error: File %s not found.\n", fn);
        return NULL;
    }
    f = fopen(fn, "rb");
    if(f == NULL)
    {
        PRINT_LAST_ERROR
        return 0;
    }
    fseek(f, 0, SEEK_END);
    flen = ftell(f);
    //activeDskSize = flen;
    rewind(f);
    buf = (char*)malloc(flen * sizeof(char));
    fread(buf, flen, 1, f);
    fclose(f);
    return buf;
}

trackHeader* make_blank_track(int track_no, int side_no, int sec_size, int num_secs, int gap3)
{
    trackHeader* t = (trackHeader*)malloc(sizeof(trackHeader));
    strcpy(t->trackStr, "Track-Info\r\n");
    t->trackNo = track_no;
    t->sideNo = side_no;
    t->sectorSize = sec_size; // in N format, i.e. 2 = 512b
    t->numSectors = num_secs;
    t->gap3 = gap3;
    // dont forget to fill it in with sector info!
    sectorHeader* sec = (sectorHeader*)t->sectorInfo;
    for(int i = 0; i < t->numSectors; i++)
    {
        sec->C = track_no; 
        sec->H = side_no;
        sec->R = (i+1) | 0xc0;
        sec->N = sec_size;
        sec++;
    }
    return t;
}
// 194816 : 2f900
// 40 tracks by 9 sectors, 512b each
// 360 sectors + 40 tr headers + 1 disk header
// (360*0x200) + (40*0x100) + (0x100) [ok!]
void pop_tracklist(DSKFile* dsk)
{
    // iterate through dos index BAM lists (ie dos ofs+16-31)
    DOSFile* nf = (DOSFile*)&dsk->bytes + 0x200;
    while(nf < (DOSFile*)&dsk->bytes + 0x200 + (32*32)){ // 32 files max
        for(int i = 0; i < 16; i++){
            if((nf->tracklist[i] > 1) && (nf->tracklist[i] < 80)) 
                dsk->usedTracks[i] = 1;
            else {
                dsk->usedTracks[i] = 0; }
        }
        nf++;
    }
    // if you find E5, stop
    // tick off used sectors in the dsk array 
    // return first '0' from track array
}

DSKFile* make_new_dsk(int tracks, int sec_per_track, int bytes_per_sec)
{
    // Make meta disk 
    DSKFile* newdisk = (DSKFile*)malloc(sizeof(DSKFile));
    // allocate bytes array 
    long dsize = (tracks * sec_per_track * bytes_per_sec) + (tracks * 256) + 256;
    char* d = (char*)malloc(dsize * sizeof(char));
    for(long i = 0; i < dsize; i++)
        d[i] = 0xe5;
    newdisk->bytes = d;
    // make disk header
    DSKHeader* newheader = (DSKHeader*)malloc(sizeof(DSKHeader));
    strcpy(newheader->headerStr, "MV - CPCEMU Disk-File\r\nDisk-Info\r\n");
    newheader->numTracks = tracks;
    newheader->numSides = 1; // SD
    int tsize = (bytes_per_sec * sec_per_track) + (0x100);
    newheader->trackSize[0] = (tsize & 0xff);
    newheader->trackSize[1] = tsize >> 8;
    for(int i = 0; i < 204; i++)
        newheader->unused[i] = 0;
    newdisk->header = newheader;
    // make blank tracks 
    newdisk->tracks = (trackHeader**)malloc(tracks * sizeof(trackHeader*));
    for(int c = 0; c < tracks; c++) // TODO: work with two sides?
    {
        newdisk->tracks[c] = make_blank_track(c, 0, bytes_per_sec >> 8, sec_per_track, 78);
        memcpy((char*)(newdisk->bytes + 0x100 + (bytes_per_sec*sec_per_track*c) + (c*0x100)), newdisk->tracks[c], 0x100);
        //c++;
        //newdisk->tracks[c] = make_blank_track(c, 1, bytes_per_sec >> 8, sec_per_track, 78);
        //memcpy((char*)(newdisk->bytes + 0x100 + (bytes_per_sec*sec_per_track*c) + (c*0x100)), newdisk->tracks[c], 0x100);
    }
    newdisk->totalSize = dsize;
    memcpy(newdisk->bytes, newdisk->header, 0x100);
    return newdisk;
}
