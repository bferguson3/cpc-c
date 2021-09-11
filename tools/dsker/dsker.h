
/*
*
* Struct definitions
*
*/ 
typedef struct _dskheader { 
    char headerStr[34];
    char dskCreator[14];
    unsigned char numTracks;
    unsigned char numSides; // 2D/SD
    unsigned char trackSize[2];
    char unused[204];
} DSKHeader;

typedef struct _trackheader {
    char trackStr[13];
    char unused[3];
    unsigned char trackNo;
    unsigned char sideNo;
    char unused_a[2];
    unsigned char sectorSize;
    unsigned char numSectors;
    unsigned char gap3;
    char fillbyte;
    char sectorInfo[232]; // 29
} trackHeader;

typedef struct _secheader {
    unsigned char C;  //tr
    unsigned char H;  //side
    unsigned char R;  //sec
    unsigned char N;  //size
    unsigned char fdc_1;
    unsigned char fdc_2;
    char unused[2];
} sectorHeader;

typedef struct _dosfile {
    char null_a;
    char fileName[8];
    char fileExt[3];
    char filePart;
    char null_b[2];
    unsigned char bam;
    unsigned char tracklist[16];
} DOSFile;

typedef struct _dskfile {
    DSKHeader* header;
    trackHeader** tracks;
    char* bytes;    // (header->numTracks * tracks[n]->numSectors) * (128<<tracks[n]->sectorSize)
    long totalSize;
    char usedTracks[256]; // TODO: for multiple files 
} DSKFile;


/*
*
* Function prototypes
*
*/ 
void usage(void);
char* read_file_bytes(const char* fn);
char* read_disk(const char* fn);
int write_dsk_file(const char* fn, DSKFile* disk, int o);
trackHeader* make_blank_track(int track_no, int side_no, int sec_size, int num_secs, int gap3);
DSKFile* make_new_dsk(int tracks, int sec_per_track, int bytes_per_sec);
int get_next_empty_fs(char* bytes);
long get_filesize(const char* fn);
void pop_tracklist(DSKFile* dsk);
