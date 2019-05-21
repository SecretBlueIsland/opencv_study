#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <winsock2.h>
typedef struct
{
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;            //! Nal Unit Buffer size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx    
	char* buf;                    //! contains the first byte followed by the EBSP
	unsigned short lost_packets;  //! true, if packet loss is detected
} NALU_t;
int GetNalu();
void FreeNALU(NALU_t* n);
NALU_t* AllocNALU(int buffersize);
void OpenBitstreamFile(const char* fn);
int GetAnnexbNALU(NALU_t* nalu);
void dump(NALU_t* nal);
int DumpChar(char* filename, char* buf, int len);
FILE* getFile();


