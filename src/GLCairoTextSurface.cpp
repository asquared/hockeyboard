#include "GLCairoTextSurface.h"

GLCairoTextSurface::GLCairoTextSurface(int width, int height) 
: GLCairoSurface(width, height) {
	last = "";
	last_r = last_g = last_b = -1;
	format_change = false;
}

void GLCairoTextSurface::clear() {
	last = "";
	format_change = false;
	GLCairoSurface::clear();
}

void GLCairoTextSurface::color(int r, int g, int b) {
	if (r != last_r || g != last_g || b != last_b) {
		format_change = true;
		GLCairoSurface::color(r, g, b);
	}
}

bool GLCairoTextSurface::setfontface(const string& face, bool bold, bool italic, bool smallcaps, int X) {
	format_change = true;
	return GLCairoSurface::setfontface(face, bold, italic, smallcaps, X);
}

bool GLCairoTextSurface::setfontsize(int size) {
	format_change = true;
	return GLCairoSurface::setfontsize(size);
}

bool GLCairoTextSurface::writetext(const string& text, int x, int y, int align) {
	bool out = false;
	//std::cout << text << " " << last << std::endl;
	if (text == last && !format_change) return false;
	else {
		clear();
		out = GLCairoSurface::writetext(text, x, y, align);
		format_change = false;
		last = text;
	}
	return out;
}

bool GLCairoTextSurface::writetext(const string& text, int x, int y, int align, int maxwidth) {
	bool out = false;
	//std::cout << text << " " << last << std::endl;
	if (text == last && !format_change) return false;
	else {
		clear();
		out = GLCairoSurface::writetext(text, x, y, align, maxwidth);
		format_change = false;
		last = text;
	}
	return out;
}

bool GLCairoTextSurface::writetextshrink(const string& text, int x, int y_c, int align, int maxwidth) {
	bool out = false;
	//std::cout << text << " " << last << std::endl;
	if (text == last && !format_change) return false;
	else {
		clear();
		out = GLCairoSurface::writetextshrink(text, x, y_c, align, maxwidth);
		format_change = false;
		last = text;
	}
	return out;
}
