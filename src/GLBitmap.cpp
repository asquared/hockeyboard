#include "GLBitmap.h"
using std::string;

// pragmas for 1-byte packing.  very useful.
#pragma pack(push)
#pragma pack(1)

struct bmpHeader {
	unsigned short bfType;  //specifies the file type
	unsigned int bfSize;  //specifies the size in bytes of the bitmap file
	unsigned short bfReserved1;  //reserved; must be 0
	unsigned short bfReserved2;  //reserved; must be 0
	unsigned int bOffset;  //species the offset in bytes to the bitmap bits
};

struct bmpInfo {
	unsigned int biSize;  //specifies the number of bytes required by the struct
	int biWidth;  //specifies width in pixels
	int biHeight;  //species height in pixels
	unsigned short biPlanes; //specifies the number of color planes, must be 1
	unsigned short biBitCount; //specifies the number of bit per pixel
	unsigned int biCompression;//spcifies the type of compression
	unsigned int biSizeImage;  //size of image in bytes
	int biXPelsPerMeter;  //number of pixels per meter in x axis
	int biYPelsPerMeter;  //number of pixels per meter in y axis
	unsigned int biClrUsed;  //number of colors used by th ebitmap
	unsigned int biClrImportant;  //number of colors that are important
};

#pragma pack(pop)

inline int roundup(int in, int mult) {
	int mod = in % mult;
	if ( mod == 0 ) return in;
	else return in + (mult - mod);
}

//bool drawRect(int xl, int xh, int yl, int yh, int z) {s
//	glBegin(GL_QUADS);
//		glVertex3i(xl, yl, z);
//		glVertex3i(xh, yl, z);
//		glVertex3i(xh, yh, z);
//		glVertex3i(xl, yh, z);
//	glEnd();
//	return true;
//}


GLBitmap::GLBitmap() {
	xt = yt = 0;
	tile = 0;
}

GLBitmap::GLBitmap(string& filename) {
	xt = yt = 0;
	tile = 0;
	load(filename);
}

GLBitmap::~GLBitmap() {
	unload();
}

bool GLBitmap::load(string& filename) {
	if (tile != 0) unload();
	FILE* file;
	file = fopen(filename.c_str(), "rb");
	if (file == 0) return false;

	bmpHeader header;
	bmpInfo info;

	// check header
	fread(&header, sizeof(bmpHeader), 1, file);
	if ( header.bfType != 0x4d42 ) {
		fclose(file);
		return false;
	}

	fread(&info, sizeof(bmpInfo), 1, file);

	if ( info.biBitCount != 24 || info.biSize != 40 || info.biCompression != 0 ) {
		fclose(file);
		return false;
	}

	xt = roundup(info.biWidth, 64) / 64;
	yt = roundup(info.biHeight, 64) / 64;
	tile = new rgba*[xt*yt];
	for (int i = 0; i < xt*yt; ++i) {
		tile[i] = new rgba[64*64];
	}
	int rowlen = roundup(info.biWidth * 3, 4);
	unsigned char* row = new unsigned char[rowlen];

	fseek(file, header.bOffset, 0);

	for (int y = 0; y < info.biHeight; ++y ) {
		fread(row, rowlen, 1, file);
		readRow(y, info.biWidth, row);
	}

	// fill in with A=0
	for (int y = info.biHeight; y < yt*64; ++y) {
		for (int x = 0; x < xt*64; ++x) {
			setTilePixel(x, y, rgba(240,0,0,0));
		}
	}

	delete[] row;
	fclose(file);
	return true;
}


bool GLBitmap::unload() {
	if (tile != 0) {
		for (int i = 0; i < xt * yt; ++i) {
			delete[] tile[i];
		}
		delete[] tile;
	}
	xt = yt = 0;
	tile = 0;
	return true;
}

void GLBitmap::setTransColor(unsigned char r, unsigned char g, unsigned char b) {
	transcol.r = r;
	transcol.g = g;
	transcol.b = b;
}

void GLBitmap::drawPixels(float x, float y) {
	if (tile == 0) return;

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < xt*yt; ++i) {
		glRasterPos3f( x + 64*(i%xt), y + 64*(i/xt), 0.0f );
		glDrawPixels(64, 64, GL_RGBA, GL_UNSIGNED_BYTE, tile[i] );
	}
}
void GLBitmap::drawTexture(float x, float y, float z) {}

void GLBitmap::readRow(int y, int xlen, unsigned char* row) {
	for ( int x = 0; x < xlen; ++x ) {
		unsigned char* p = &(row[3*x]);
		bool blank = transcol.r == p[2] && transcol.g == p[1] && transcol.b == p[0];
		if (blank) setTilePixel(x, y, rgba(0,0,0,0) );
		else setTilePixel(x, y, rgba(p[2],p[1],p[0],255) );
	}
	for ( int x = xlen; x < xt*64; ++x ) {
		setTilePixel(x, y, rgba(0,0,0,0));
	}
}

inline void GLBitmap::setTilePixel(int x, int y, rgba col) {
	int p = (y/64)*xt + x/64;
	int q = (y%64)*64 + (x%64);
	tile[p][q] = col;
}