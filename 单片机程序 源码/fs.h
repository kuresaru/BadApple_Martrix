#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long int

void FS_Init(void);
void FS_FindBadAppleBin(void);
ulong FS_GetNextClust(ulong clust);

struct FS_FILE {
	uchar FileName[8];
	uchar FileType[3];
	uchar FileAttribute;
	ulong FileClust;
	ulong FileLength;
};
