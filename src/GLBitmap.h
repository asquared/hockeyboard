#ifndef _glbitmap_h_
#define _glbitmap_h_
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <string>

typedef unsigned char byte;

using std::string;

struct rgba {
	byte r;
	byte g;
	byte b;
	byte a;

	rgba() {}
	rgba(byte ri, byte gi, byte bi, byte ai) { 
		r = ri;
		g = gi;
		b = bi;
		a = ai;
	}
};

//bool drawRect(int xl, int xh, int yl, int yh, int z);


class GLBitmap {

private:
	int xt, yt;
	rgba transcol;
	rgba** tile;


public:
	GLBitmap();
	GLBitmap(string& filename);
	~GLBitmap();

	bool load(string& filename);
	bool unload();

	void setTransColor(unsigned char r, unsigned char g, unsigned char b);

	void drawPixels(float x, float y);
	void drawTexture(float x, float y, float z);

private:
	void readRow(int y, int xlen, unsigned char* row);
	inline void setTilePixel(int x, int y, rgba col);

};

#endif