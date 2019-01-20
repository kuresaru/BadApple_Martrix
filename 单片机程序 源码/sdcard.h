bit SD_Init(void);
unsigned char SD_Command(unsigned char cmd, unsigned char dat0, unsigned char dat1, unsigned char dat2, unsigned char dat3, unsigned char crc);
unsigned char SD_Data_Read(void);
void SD_Data_Send(unsigned char dat);
void SD_Start(void);
void SD_Stop(void);
bit SD_StartReadBlock(unsigned long int address);

struct SD_INFO {
	enum SD_Type {
		UNINITIALIZED=0, SD_1, SD_2, SD_MMC, SD_HC
	} SD_Type;
};
