#ifdef _MSC_VER
#include <windows.h>

#pragma pack(push, 1)
typedef struct
{
	BYTE        bWidth;          // Width, in pixels, of the image
	BYTE        bHeight;         // Height, in pixels, of the image
	BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
	BYTE        bReserved;       // Reserved ( must be 0)
	WORD        wPlanes;         // Color Planes
	WORD        wBitCount;       // Bits per pixel
	DWORD       dwBytesInRes;    // How many bytes in this resource?
	DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct
{
	WORD           idReserved;   // Reserved (must be 0)
	WORD           idType;       // Resource Type (1 for icons)
	WORD           idCount;      // How many images?
	ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
} ICONDIR, *LPICONDIR;
#endif
#pragma pack(pop)

#include "icon.h"
#include "../resources/resources.h"

wxIconBundle getIconBundle()
{
	wxIconBundle icons;
#ifdef _MSC_VER
	int sizes[] = { 16, 32, 48, 0 };
	for (auto size : sizes) {
		// Get offset of icon in file
		LPICONDIR iconDir = (LPICONDIR)resource_Emuall_ico;
		if (iconDir->idType != 1 || iconDir->idCount == 0) {
			break;
		}
		int offset = 0;
		int dataSize = 0;
		for (int i = 0; i < iconDir->idCount; i++) {
			LPICONDIRENTRY entry = &iconDir->idEntries[i];
			if (entry->bWidth == size && entry->bHeight == size && entry->wBitCount == 32) {
				offset = iconDir->idEntries[i].dwImageOffset;
				dataSize = iconDir->idEntries[i].dwBytesInRes;
				break;
			}
		}

		HICON hicon = CreateIconFromResourceEx((PBYTE)resource_Emuall_ico + offset, dataSize,
			TRUE, 0x00030000, size, size, LR_DEFAULTCOLOR);
		if (hicon == NULL) {
			continue;
		}
		wxIcon icon;
		if (icon.CreateFromHICON(hicon))
		{
			icons.AddIcon(icon);
		}
	}
#else 
#error TODO: need to implement the get icon function
#endif
	return icons;
}
