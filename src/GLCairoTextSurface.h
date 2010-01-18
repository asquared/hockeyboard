#ifndef _glcairotextsurface_h_
#define _glcairotextsurface_h_

#include "GLCairoSurface.h"

class GLCairoTextSurface : public GLCairoSurface {

private:
	string last;
	int last_r, last_g, last_b;
	bool format_change;

public:
	GLCairoTextSurface(int width, int height);

	//void common_constructor();

	//int width();
	//int height();

	void clear();
	//void fill(int r, int g, int b, int a);
	//void drawrect(double x_l, double x_h, double y_l, double y_h);
	//void paintgl(float x, float y);
	//void painton(GLCairoSurface* glcs2, int x, int y, double alpha);

	void color(int r, int g, int b);

	bool setfontface(const string& face, bool bold, bool italic, bool smallcaps);
	bool setfontsize(int size);
	bool writetext(const string& text, int x, int y, int align);
	bool writetext(const string& text, int x, int y, int align, int maxwidth);
	bool writetextshrink(const string& text, int x, int y_c, int align, int maxwidth);

};

#endif