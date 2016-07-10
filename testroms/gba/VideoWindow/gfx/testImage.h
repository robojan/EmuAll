
//{{BLOCK(testImage)

//======================================================================
//
//	testImage, 240x160@8, 
//	+ palette 256 entries, not compressed
//	+ 224 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 30x20 
//	Total size: 512 + 14336 + 1200 = 16048
//
//	Time-stamp: 2016-07-06, 16:39:35
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.13
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TESTIMAGE_H
#define GRIT_TESTIMAGE_H

#define testImageTilesLen 14336
extern const unsigned int testImageTiles[3584];

#define testImageMapLen 1200
extern const unsigned short testImageMap[600];

#define testImagePalLen 512
extern const unsigned short testImagePal[256];

#endif // GRIT_TESTIMAGE_H

//}}BLOCK(testImage)
