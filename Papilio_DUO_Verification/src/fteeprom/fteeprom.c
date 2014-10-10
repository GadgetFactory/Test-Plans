#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "ftd2xx.h"


int main ( int argc, char *argv[] ) {
  FT_HANDLE ftHandle; 
  FT_STATUS ftStatus;
  DWORD i;
  WORD data;
  WORD buff[128];
  WORD checksum = 0xaaaa;
  DWORD EEUA_Size = 0;

  if (argc > 1) {
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}		
	
	ftStatus = FT_Open (0, &ftHandle);
	if (ftStatus == FT_OK) { 
		ftStatus = FT_EE_UASize(ftHandle, &EEUA_Size);
		if (ftStatus == FT_OK) { 
			printf("Size of EEPROM = %lu\n",EEUA_Size);
			if (EEUA_Size < 128) {
				printf("EEPROM is too small for this hex file!");
				FT_Close(ftHandle);
				fclose(f);				
				return 1;
			}
		}
		else 
		{
			printf("Cannot get size of EEPROM");
			FT_Close(ftHandle);
			fclose(f);				
			return 1;
		}
		fread(buff, sizeof(buff[0]), sizeof(buff)/sizeof(buff[0]), f);
		for (i=0; i<127; i++) {
		  checksum = buff[i]^checksum;
		  checksum = (checksum << 1) | (checksum >> 15);
		}
		buff[127] = checksum;
		for (i=0; i<128; i++) {
		  ftStatus = FT_WriteEE(ftHandle, i, buff[i]);
		}
		printf("Finished writing EEPROM.\n Verifying...\n");
        for (i=0; i<128; i++) {
          ftStatus = FT_ReadEE(ftHandle, i, &data);
          if (ftStatus == FT_OK) {
            if ( buff[i] == data );
			else
			{
				printf("Verify failed. File Data: %X - EEPROM Data: %X\n" , buff[i], data);
				//printf("%X\n", buff[i]);
				FT_Close(ftHandle);
				fclose(f);				
				return 1;
			}
          }
        }	
		FT_Close(ftHandle);
		fclose(f);		
		printf("EEPROM file loaded, power-cycle the board for the change to take effect\n");
	} else
		printf("Error! Can't find board with ft2232\n");
  } else
		printf("Usage: fteeprom filename\n");
}
