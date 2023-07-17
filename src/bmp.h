#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Color{
    union{
        struct {
            uint8_t r,g,b,a;
        };
        uint32_t abgr;
    };
}Color;

// changes (rr gg bb aa) to uint32_t(bb gg rr aa) due to little endian
static uint32_t RGBtoBMPColor(uint32_t color){
    Color c;
    c.abgr = color;
    uint32_t p = c.a;
    p = (p<<8) + c.r;
    p = (p<<8) + c.g;
    p = (p<<8) + c.b;
    // p = aa rr gg bb actual value
    // p = bb gg rr aa in memory
    return(p);
}

// changes (bb gg rr aa) to uint32_t(rr gg bb aa)
static Color BMPtoRBGColor(uint32_t c){
    Color p;
    p.a = (c>>24);
    p.b = c&0xff;
    p.g = (c>>8)&0xff;
    p.r = (c>>16)&0xff;
    // p = aa rr gg bb actual value
    // p = bb gg rr aa in memory
    return(p);
}

typedef struct{
    // BM
    char type[2];
    // size of bmp (of whole file)
    uint32_t size;
    // 2*2 bytes reserved: set to 0
    uint16_t reserved1, reserved2;
    // offset to the actual bitmap data
    uint32_t offset;
}BMP_FileHeader;

typedef struct{
    // size of header (40)
    uint32_t headerSize;
    // width/ height of bitmap: sign determines the position of origin
    int32_t width, height;
    // ?
    uint16_t planes;
    // bits per pixel
    uint16_t bitsPerPixel;
    // type of compression used, if any
    uint32_t compression;
    // set to 0 for uncompressed RGB maps
    uint32_t imageSize;
    // horizontal and vertical resolution (pixels per meter)
    int32_t Xres, Yres;
    // number of color indices in color table that are actually used
    // set to 0 for 2^bitsPerPixel colors
    uint32_t colorsUsed;
    // number of color indices considered important in displaying the bmp: 
    // set to 0 if all colors are important
    uint32_t impColorsUsed;
}BMP_InfoHeader;


enum{
    BMP_WRITE_NONE = 0x0,
    BMP_WRITE_REFLECT_X = 0x1, 
    BMP_WRITE_REFLECT_Y = 0x2,
    BMP_WRITE_REFLECT_XY = BMP_WRITE_REFLECT_X|BMP_WRITE_REFLECT_Y
};


#ifdef BMP_USE_INDEX

#define COLOR_DEPTH 4

typedef struct{
    BMP_FileHeader fileHeader;
    BMP_InfoHeader infoHeader;
    Color *RGBQuads;
    uint8_t *indexArr; 
}BMP_File;

// sets the width, height, colordepth and other basic stuff
// can be changed according to need
static BMP_File *newBMP(int32_t w, int32_t h, Color *colors, uint32_t n, uint8_t *indexArr, uint16_t colorDepth){
    BMP_File *file = (BMP_File *)malloc(sizeof(BMP_File));
    
    // file header
    file->fileHeader.type[0] = 'B';
    file->fileHeader.type[1] = 'M';
    file->fileHeader.reserved1 = 0;
    file->fileHeader.reserved2 = 0;
    

    // info header
    file->infoHeader.bitsPerPixel = colorDepth;
    file->infoHeader.height = h;
    file->infoHeader.width = w;
    file->infoHeader.planes = 1;
    file->infoHeader.imageSize = 0;
    file->infoHeader.compression = 0;
    file->infoHeader.headerSize = sizeof(BMP_InfoHeader);
    file->infoHeader.impColorsUsed = 0;
    file->infoHeader.colorsUsed = n;
    file->infoHeader.Xres = 0;
    file->infoHeader.Yres = 0;

    // filesize = fileheader (14) + infoheader (40) + rgbquads (n * 4) + pixels (h * w * colordepth/8)
    uint32_t colorArrSize = file->infoHeader.colorsUsed * 4;
    uint32_t indexArrSize = file->infoHeader.height * file->infoHeader.width * file->infoHeader.bitsPerPixel/8;
    file->fileHeader.size = 14 + 40 + colorArrSize + indexArrSize;  
    file->fileHeader.offset = 14 + 40 + colorArrSize;

    file->RGBQuads = colors;
    file->indexArr = indexArr;
    return(file);
}


static int writeBMP(BMP_File *file, const char *path){
    FILE *f = NULL;
    fopen_s(&f, path, "wb");
    if (!f) return(-1);
    // write file header (2+4+4+4 = 14 bytes)
    fwrite(file->fileHeader.type, 1, 2, f);
    fwrite(&file->fileHeader.size, 4, 3,f);

    // write info header (40 bytes)
    fwrite(&file->infoHeader, 1, sizeof(file->infoHeader), f);
    
    // write RGB quads
    for (int i=0; i< file->infoHeader.colorsUsed; i++){
        // Color(rrggbb00) stored as (00 bb gg rr) 
        // (00rrggbb) stored as (bb gg rr 00)
        uint32_t p = RGBtoBMPColor(file->RGBQuads[i]);
        fwrite(&p, sizeof(uint32_t), 1, f);
    }

    // write index array
    uint32_t indexArrSize = file->infoHeader.height * file->infoHeader.width * file->infoHeader.bitsPerPixel/8;
    fwrite(file->indexArr, 1,indexArrSize, f);
    fclose(f);
    return 0;
}

#endif // BMP_USE_INDEX


#ifdef BMP_USE_VALUES


#define COLOR_DEPTH 32

typedef struct{
    BMP_FileHeader fileHeader;
    BMP_InfoHeader infoHeader;
    uint32_t *pixelValues; 
}BMP_File;

// sets the width, height, colordepth and other basic stuff
// can be changed according to need
static BMP_File *newBMP(int32_t w, int32_t h, uint32_t *pixels, uint16_t colorDepth){
    BMP_File *file = (BMP_File *)malloc(sizeof(BMP_File));
    
    // file header
    file->fileHeader.type[0] = 'B';
    file->fileHeader.type[1] = 'M';
    file->fileHeader.reserved1 = 0;
    file->fileHeader.reserved2 = 0;
    

    // info header
    file->infoHeader.bitsPerPixel = colorDepth;
    file->infoHeader.height = h;
    file->infoHeader.width = w;
    file->infoHeader.planes = 1;
    file->infoHeader.imageSize = 0;
    file->infoHeader.compression = 0;
    file->infoHeader.headerSize = sizeof(BMP_InfoHeader);
    file->infoHeader.impColorsUsed = 0;
    file->infoHeader.colorsUsed = 0;
    file->infoHeader.Xres = 0;
    file->infoHeader.Yres = 0;

    // filesize = fileheader (14) + infoheader (40) + + pixels (h * w * colordepth/8)
    uint32_t pixelArraySize = file->infoHeader.height * file->infoHeader.width * file->infoHeader.bitsPerPixel/8;
    file->fileHeader.size = 14 + 40 + pixelArraySize;  
    file->fileHeader.offset = 14 + 40;

    file->pixelValues = pixels;
    return(file);
}


static int writeBMP(BMP_File *file, const char *path, int writeFlags = 0){
    FILE *f = NULL;
    fopen_s(&f, path, "wb");
    if (!f) return(-1);
    // write file header (2+4+4+4 = 14 bytes)
    fwrite(file->fileHeader.type, 1, 2, f);
    fwrite(&file->fileHeader.size, 4, 3,f);

    // write info header (40 bytes)
    fwrite(&file->infoHeader, 1, sizeof(file->infoHeader), f);
    

    // write pixel array
    uint32_t pixelArraySize = file->infoHeader.height * file->infoHeader.width;

    for (int i= 0; i != file->infoHeader.height; i++){
        for (int j= 0; j != file->infoHeader.width; j++){
            int row = i, column = j;
            if (writeFlags & BMP_WRITE_REFLECT_Y)
                row = file->infoHeader.height - row -1;
            if (writeFlags & BMP_WRITE_REFLECT_X)
                column = file->infoHeader.width -column -1;
            uint32_t a = RGBtoBMPColor(file->pixelValues[column + row *file->infoHeader.width]);
            fwrite(&a, 1,file->infoHeader.bitsPerPixel/8, f);
        }
    }
    fclose(f);
    return 0;
}

#endif // BMP_USE_VALUES


// returns the pixels values of the bmp
// pass rgba as 0 to leave the pixels in bgra format 
uint32_t *readPixels(const char *filePath, int *h, int *w, int rgba = 1){
    auto len = [&](const char *str){
        int l = 0;
        while (str[l])
            l++;
        return l;
    };

    int l = len(filePath);
    if (!(filePath[l-3]=='b'&& filePath[l-2]=='m'&& filePath[l-1]=='p')){
        fprintf(stderr, "[ERROR] %s is not a bmp file", filePath);
        return NULL;
    }
    FILE *f = NULL;
    fopen_s(&f, filePath, "rb");
    BMP_FileHeader fheader;
    // assuming fileheader is 14 bytes
    fread(fheader.type, 1,2, f);
    fread(&fheader.size, 4,3, f);

    BMP_InfoHeader iheader;
    //assuming infoheader is 40 bytes
    fread(&iheader,1,40,f);

    uint32_t *pixels = (uint32_t*)malloc(sizeof(*pixels)*iheader.height*iheader.width);
    if (iheader.colorsUsed == 0){
        fseek(f,fheader.offset,SEEK_SET);
        // if each pixel uses a 32 bit value
        if (iheader.bitsPerPixel == 32)
            fread(pixels,4, iheader.width *iheader.height,f);
        else{
            for (int i=0; i<iheader.width*iheader.height;i++){
                fread(&pixels[i], 4, 1,f);
            }
        }
    }
    fclose(f);
    if (rgba){
        for (int i=0; i<iheader.width*iheader.height;i++){
            pixels[i] = BMPtoRBGColor(pixels[i]).abgr;
        }
    }
    *h = iheader.height;
    *w = iheader.width;
    return(pixels);
}
