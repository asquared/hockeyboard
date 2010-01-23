#ifndef _glcairosurface_h_
#define _glcairosurface_h_

#define MAX_ALT 4

// required libraries
#pragma comment(lib, "pango-1.0.lib")
#pragma comment(lib, "pangocairo-1.0.lib")
#pragma comment(lib, "libcairo.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "opengl32")

#include <pango/pangocairo.h>
#include <pango/pango-font.h>
#include <windows.h>
#include <gl/gl.h>
#include <string>
#include <iostream>

using std::string;

class GLCairoSurface {

private:
	cairo_surface_t* cs;
	cairo_t* ct;
	PangoFontDescription* pfd;
	PangoFontDescription* pfda[MAX_ALT];
	PangoFontMap* pm;
	PangoCairoFontMap* pfm;
	PangoContext* pc;
	int norm_size;
	int X_glyph;								// index of 'X' glyph in font
	int X_glyph_a[MAX_ALT];						// index of 'X' glyph for alternate fonts

public:
	GLCairoSurface(int width, int height);
	GLCairoSurface(const string& pngfilename);
	~GLCairoSurface();

	void common_constructor();

	int width();
	int height();

	void clear();
	void fill(int r, int g, int b, int a);
	void fill_match_a(int r, int g, int b, int a);
	void drawrect(double x_l, double x_h, double y_l, double y_h);
	void paintgl(float x, float y);
	void painton(GLCairoSurface* glcs2, int x, int y, double alpha);
	void mixfrom(GLCairoSurface* s1, GLCairoSurface* s2, double alpha);

	void color(int r, int g, int b);

	bool setfontface(const string& face, bool bold, bool italic, bool smallcaps, int X);
	bool setfontfacealt(unsigned int index, const string& face, bool bold, bool italic, bool smallcaps, int X);
	bool setfontsize(int size);
	bool setaa(bool mode);
	bool writetext(const string& text, int x, int y, int align);
	bool writetext(const string& text, int x, int y, int align, int maxwidth);
	bool writetextshrink(const string& text, int x, int y_c, int align, int maxwidth);

};

#endif