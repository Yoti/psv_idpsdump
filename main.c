#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>

#include <stdio.h> // sprintf()
#include <psp2/ctrl.h> // sceCtrl*()

#include "graphics.h"

#define VER_MAJOR 0
#define VER_MINOR 7
#define VER_BUILD 'a'

#define printf psvDebugScreenPrintf

int _vshSblAimgrGetConsoleId(char CID[32]);

/*
	Model: FatWF, SKU: PCH-1000, MoBo: IRS-002;
	Model: Fat3G, SKU: PCH-1100, MoBo: IRS-002;
	Model: Slim, SKU: PCH-2000, MoBo: USS-1001;
	Model: TV, SKU: VTE-1000, MoBo: DOL-1001.

	No diff between FatWF and Fat3G.
	No diff between Vita TV (Asian) and PSTV (Western).
*/

SceCtrlData pad;

void ExitCross(char*text)
{
	printf("%s, press X to exit...\n", text);
	do
	{
		sceCtrlReadBufferPositive(0, &pad, 1);
		sceKernelDelayThread(0.05*1000*1000);
	}
	while(!(pad.buttons & SCE_CTRL_CROSS));
	sceKernelExitProcess(0);
}

void ExitError(char*text, int delay, int error)
{
	printf(text, error);
	sceKernelDelayThread(delay*1000*1000);
	sceKernelExitProcess(0);
}

int WriteFile(char*file, void*buf, int size)
{
	sceIoRemove(file);
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

int main(int argc, char *argv[])
{
	int i = 0;
	int paranoid = 0;
	char idps_buffer[16];
	unsigned char idps_text_char_tmp[1];
	unsigned char idps_text_char_1st[1];
	unsigned char idps_text_char_2nd[1];
	char idps_text_buffer[32] = "";

	sceKernelDelayThread(1000000);
	sceCtrlReadBufferPositive(0, &pad, 1);
	sceKernelDelayThread(1000000);
	if (pad.buttons & SCE_CTRL_LTRIGGER)
		paranoid = 1;

	psvDebugScreenInit();
	psvDebugScreenClear(0);
	printf("PSV IDPS Dumper v%i.%i%c by Yoti\nbased on VitaCID by Major_Tom\n\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	_vshSblAimgrGetConsoleId(idps_buffer);

	printf(" Your IDPS is: ");
	if (paranoid == 1)
	{
		for (i=0; i<0x08; i++) // 0x0A???
		{
			if (i == 0x04)
				psvDebugScreenSetFgColor(0xFF0000FF); // red
			else if (i == 0x06)
				psvDebugScreenSetFgColor(0xFF0000FF); // red
			else if (i == 0x07)
				psvDebugScreenSetFgColor(0xFF00FF00); // green
			else if (i == 0x05)
				psvDebugScreenSetFgColor(0xFFFF0000); // blue
			else
				psvDebugScreenSetFgColor(0xFFFFFFFF); // white
			printf("%02X", (u8)idps_buffer[i]);
		}
		for (i=0; i<0x08; i++) // 0x06???
		{
			psvDebugScreenSetFgColor(0xFF777777); // gray
			printf("XX");
			psvDebugScreenSetFgColor(0xFFFFFFFF); // white
		}
	}
	else
	{
		for (i=0; i<0x10; i++)
		{
			if (i == 0x04)
				psvDebugScreenSetFgColor(0xFF0000FF); // red
			else if (i == 0x06)
				psvDebugScreenSetFgColor(0xFF0000FF); // red
			else if (i == 0x07)
				psvDebugScreenSetFgColor(0xFF00FF00); // green
			else if (i == 0x05)
				psvDebugScreenSetFgColor(0xFFFF0000); // blue
			else
				psvDebugScreenSetFgColor(0xFFFFFFFF); // white
			printf("%02X", (u8)idps_buffer[i]);
		}
	}
	printf("\n\n");

	printf(" It seems that you are using ");
	psvDebugScreenSetFgColor(0xFF0000FF); // red
	if (idps_buffer[0x04] == 0x00)
		printf("PlayStation Portable");
	else if (idps_buffer[0x04] == 0x01) // psv, vtv/pstv
	{
		if (idps_buffer[0x06] == 0x00)
			printf("PlayStation Vita"); // fatWF/fat3G, slim
		else if (idps_buffer[0x06] == 0x02)
			printf("PlayStation/Vita TV"); // vtv, pstv
		else
			printf("Unknown Vita 0x%02X", idps_buffer[0x06]);
	}
	else
		printf("Unknown PS 0x%02X", idps_buffer[0x04]);
	psvDebugScreenSetFgColor(0xFFFFFFFF); // white
	printf("\n");

	printf(" Your motherboard is ");
	psvDebugScreenSetFgColor(0xFF00FF00); // green
	if (idps_buffer[0x06] == 0x00) // portable
	{
		switch(idps_buffer[0x07])
		{
			case 0x01:
				printf("TA-079/081 (PSP-1000)");
				break;
			case 0x02:
				printf("TA-082/086 (PSP-1000)");
				break;
			case 0x03:
				printf("TA-085/088 (PSP-2000)");
				break;
			case 0x04:
				printf("TA-090/092 (PSP-3000)");
				break;
			case 0x05:
				printf("TA-091 (PSP-N1000)");
				break;
			case 0x06:
				printf("TA-093 (PSP-3000)");
				break;
			//case 0x07:
			//	printf("???");
			//	break;
			case 0x08:
				printf("TA-095 (PSP-3000)");
				break;
			case 0x09:
				printf("TA-096/097 (PSP-E1000)");
				break;
			case 0x10:
				printf("IRS-002 (PCH-1000/1100)");
				break;
			case 0x14:
				printf("USS-1001 (PCH-2000)");
				break;
			default:
				printf("Unknown MoBo 0x%02X", idps_buffer[0x07]);
				break;
		}
	}
	else if (idps_buffer[0x06] == 0x02) // home system
	{
		switch(idps_buffer[0x07])
		{
			case 0x01:
				printf("DOL-1001 (VTE-1000)");
				break;
			default:
				printf("Unknown MoBo 0x%02X", idps_buffer[0x07]);
				break;
		}
	}
	else
		printf("Unknown type 0x%02X", idps_buffer[0x06]);
	psvDebugScreenSetFgColor(0xFFFFFFFF); // white
	printf("\n");

	printf(" And your region is ");
	psvDebugScreenSetFgColor(0xFFFF0000); // blue
	switch(idps_buffer[0x05])
	{
		case 0x03:
			printf("Japan");
			break;
		case 0x04:
			printf("North America");
			break;
		case 0x05:
			printf("Europe/East/Africa");
			break;
		case 0x06:
			printf("Korea");
			break;
		case 0x07: // PCH-xx03 VTE-1016
			printf("Great Britain/United Kingdom");
			break;
		case 0x08:
			printf("Mexica/Latin America");
			break;
		case 0x09:
			printf("Australia/New Zeland");
			break;
		case 0x0A:
			printf("Hong Kong/Singapore");
			break;
		case 0x0B:
			printf("Taiwan");
			break;
		case 0x0C:
			printf("Russia");
			break;
		case 0x0D:
			printf("China");
			break;
		default:
			printf("Unknown region 0x%02X", idps_buffer[0x05]);
			break;
	}
	psvDebugScreenSetFgColor(0xFFFFFFFF); // white
	printf("\n\n");

	// binary
	printf(" Saving as ux0:data/idps.bin... ");
	if (WriteFile("ux0:data/idps.bin", idps_buffer, 16) > 0)
		printf("OK");
	else
		printf("NG");
	printf("\n");

	// text
	for (i=0; i<0x10; i++)
	{
		idps_text_char_tmp[1]=idps_buffer[i];
		idps_text_char_1st[1]=(idps_text_char_tmp[1] & 0xf0) >> 4;
		idps_text_char_2nd[1]=(idps_text_char_tmp[1] & 0x0f);

		// 1st half of byte
		if (idps_text_char_1st[1] < 0xA) // digit
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_1st[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_1st[1]+0x37);

		// 2nd half of byte
		if (idps_text_char_2nd[1] < 0xA) // digit
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_2nd[1]+0x30);
		else // char
			sprintf(idps_text_buffer, "%s%c", idps_text_buffer, idps_text_char_2nd[1]+0x37);
	}
	printf(" Saving as ux0:data/idps.txt... ");
	if (WriteFile("ux0:data/idps.txt", idps_text_buffer, 32) > 0)
		printf("OK");
	else
		printf("NG");
	printf("\n\n");

	printf(" https://github.com/yoti/psv_idpsdump/\n");

	ExitCross("\nDone");
	return 0;
}
