#include "12864.h"
#include "delay.h"
#include "sdcard.h"
#include "num.h"
#include "fs.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long int

ulong FS_PatternSector;						// ������ַ
uchar FS_SectorPerClust;					// ÿ��������
uint 	FS_PersistSector;						// ����������
ulong FS_FatUseSector;						// FAT��ռ��������
ulong FS_FatSector;								// FAT1����ʼ������
ulong FILE_BadApple_StartClust;		// badapple.bin�ļ���ʼ�غ�

// ��ȡFAT32�ļ�ϵͳ��Ϣ
void FS_Init() {
	uint i;
	uchar *hex;
	SD_Start();
	// ��MBR ȡ������ַ
	if (SD_StartReadBlock(0x00000000)) {
		for (i = 0; i < 0x01C6; i++)
			SD_Data_Read();
		FS_PatternSector = SD_Data_Read();
		FS_PatternSector |= SD_Data_Read() << 8;
		FS_PatternSector |= SD_Data_Read() << 16;
		FS_PatternSector |= SD_Data_Read() << 24;
		i += 4;
		while (i++ < 0x202)
			SD_Data_Read();
		LCD12864_Positon(1, 0);
		LCD12864_WriteText("����  0x");
		hex = ul2hstr(FS_PatternSector);
		for (i = 0; i < 8; i++)
			LCD12864_WriteData(*(hex + i));
	} else {
		LCD12864_Positon(1, 0);
		LCD12864_WriteText("������ַ��ȡʧ��");
		return;
	}
	// ��DBR ȡÿ�������� ���������� ��ռ�������� ��Ŀ¼�غż���Ϊ2
	if (SD_StartReadBlock(FS_PatternSector)) {
		for (i = 0; i < 0x000D; i++)
			SD_Data_Read();
		FS_SectorPerClust = SD_Data_Read(); // 0x000D ÿ��������
		FS_PersistSector = SD_Data_Read(); // 0x0E-0x0F ����������
		FS_PersistSector |= SD_Data_Read() << 8;
		i += 3;
		while (i++ < 0x0024)
			SD_Data_Read();
		FS_FatUseSector = SD_Data_Read(); // 0x24-0x27 ÿ��FAT��ռ��������
		FS_FatUseSector |= SD_Data_Read() << 8;
		FS_FatUseSector |= SD_Data_Read() << 16;
		FS_FatUseSector |= SD_Data_Read() << 24;
		i += 4;
		while (i++ < 0x0202)
			SD_Data_Read();
		SD_Stop();
		LCD12864_Positon(2, 0);
		LCD12864_WriteText("ÿ������     ");
		hex = uc2dstr(FS_SectorPerClust);
		for (i = 0; i < 3; i++)
			LCD12864_WriteData(*(hex + i));
		LCD12864_Positon(3, 0);
		LCD12864_WriteText("��������   ");
		hex = ui2dstr(FS_PersistSector);
		for (i = 0; i < 5; i++)
			LCD12864_WriteData(*(hex + i));
		delay(3000);
		LCD12864_Clear();
		LCD12864_WriteText("FAT ռ0x");
		hex = ul2hstr(FS_FatUseSector);
		for (i = 0; i < 8; i++)
			LCD12864_WriteData(*(hex + i));
	} else {
		SD_Stop();
		LCD12864_Positon(2, 0);
		LCD12864_WriteText("������Ϣ��ȡʧ��");
		return;
	}
}

// ȡ��һ����(FAT�б��ص�����)  0x0FFFFF�ǽ���
ulong FS_GetNextClust(ulong clust) {
	ulong sector = FS_FatSector + (clust / 128); // һ����4�ֽ� һ��������128����
	uchar pos = clust % 128; // ������ĵڼ����ֽ�
	uchar i;
	ulong result = 0;
	if (SD_StartReadBlock(sector)) {
		for(i = 0; i < 128; i++) {
			if (i == pos) {
				result = SD_Data_Read();
				result |= SD_Data_Read() << 8;
				result |= SD_Data_Read() << 16;
				result |= SD_Data_Read() << 24;
			} else {
				SD_Data_Read();
				SD_Data_Read();
				SD_Data_Read();
				SD_Data_Read();
			}
		}
		SD_Data_Read();
		SD_Data_Read();
	}
	return result;
}

void FS_FindBadAppleBin() {
	struct FS_FILE file;
	ulong nextClust = 2; // һ�����2  ����2�Ĳ����� ������
	ulong rootSector;
	uchar clustSector, fatItem, j;
	uchar *hex;
	
	FS_FatSector = FS_PatternSector + FS_PersistSector; // FAT1������� = ����DBR���� + ����������
	rootSector = FS_FatSector + FS_FatUseSector + FS_FatUseSector; // ��(2��)���� = FAT1���� + FATռ������x2
	FILE_BadApple_StartClust = 0;
	LCD12864_Positon(1, 0);
	LCD12864_WriteText("����BADAPPLE.BIN");
	SD_Start();
	while (nextClust != 0x0FFFFFFF) { // ������Ŀ¼���д�
		for (clustSector = 0; clustSector < FS_SectorPerClust; clustSector++) { // ������Ŀ¼һ���ص���������
			if (SD_StartReadBlock(rootSector + (((nextClust - 2) * FS_SectorPerClust) + clustSector))) {
				for (fatItem = 0; fatItem < 16; fatItem++) { // ������Ŀ¼һ���ص�һ��������һ��  һ��32�ֽ� ��16*32=512�ֽ�=1����
					for (j = 0; j < 8; j++) // 8�ֽڵ��ļ���
						file.FileName[j] = SD_Data_Read();
					for (j = 0; j < 3; j++) // 3�ֽڵ���չ��
						file.FileType[j] = SD_Data_Read();
					file.FileAttribute = SD_Data_Read(); // 1�ֽڵ�����
					if (file.FileAttribute != 0x20 || file.FileName[0] == 0xE5) { // ���ǹ鵵�ļ����ѱ�ɾ�� ֱ������
						for (j = 0; j < 20; j++) // ��ߵ�20�ֽڶ���
							SD_Data_Read();
						continue;
					}
					for (j = 0; j < 8; j++) // 1�ֽڵı��� 1�ֽڵĴ���ʱ����� 2�ֽڴ���ʱ�� 2�ֽڴ������� 2�ֽ�����ȡ���� = 8�ֽڶ���
						SD_Data_Read();
					file.FileClust = SD_Data_Read() << 16; // ��ʼ�ظ�2�ֽ�
					file.FileClust |= SD_Data_Read() << 24;
					for (j = 0; j < 4; j++) // 2�ֽ��޸�ʱ�� 2�ֽ��޸����� = 4�ֽڶ���
						SD_Data_Read();
					file.FileClust |= SD_Data_Read(); // ��ʼ�ص�2�ֽ�
					file.FileClust |= SD_Data_Read() << 8;
					file.FileLength = SD_Data_Read(); // 4�ֽڵ��ļ�����
					file.FileLength |= SD_Data_Read() << 8;
					file.FileLength |= SD_Data_Read() << 16;
					file.FileLength |= SD_Data_Read() << 24;
					if (file.FileName[0] == 'B' && file.FileName[1] == 'A' && file.FileName[2] == 'D' && file.FileName[3] == 'A' && 
									file.FileName[4] == 'P' && file.FileName[5] == 'P' && file.FileName[6] == 'L' && file.FileName[7] == 'E' && 
									file.FileType[0] == 'B' && file.FileType[1] == 'I' && file.FileType[2] == 'N') {
						FILE_BadApple_StartClust = file.FileClust;
					}
				}
				SD_Data_Read();
				SD_Data_Read();
				if (FILE_BadApple_StartClust)
					break;
			}
		}
		if (FILE_BadApple_StartClust)
			break;
		nextClust = FS_GetNextClust(nextClust);
	}
	if (FILE_BadApple_StartClust) {
		LCD12864_Positon(2, 0);
		LCD12864_WriteText("�ҵ�  0x");
		hex = ul2hstr(FILE_BadApple_StartClust);
		for (j = 0; j < 8; j++)
			LCD12864_WriteData(*(hex + j));
	} else {
		LCD12864_Positon(2, 0);
		LCD12864_WriteText("�ļ�δ�ҵ�");
	}
	SD_Stop();
}