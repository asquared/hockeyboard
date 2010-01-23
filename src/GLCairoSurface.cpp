#include "GLCairoSurface.h"

void GLCairoSurface::common_constructor() {
	ct = cairo_create(cs);
	pfd = pango_font_description_new();
	for (int i = 0; i < MAX_ALT; ++i) pfda[i] = 0;
	pm = pango_cairo_font_map_get_default();
	pfm = (PangoCairoFontMap*) pm;
	pc = pango_font_map_create_context(pm);
	cairo_font_options_t* cfo = cairo_font_options_create();
	cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_GRAY);
	pango_cairo_context_set_font_options(pc, cfo);
	pango_cairo_update_context(ct, pc);
	cairo_font_options_destroy(cfo);
	norm_size = 0;
	X_glyph = -1;
}

GLCairoSurface::GLCairoSurface(int width, int height) {
	cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	common_constructor();
}

GLCairoSurface::GLCairoSurface(const string& pngfilename) {
	cs = cairo_image_surface_create_from_png(pngfilename.c_str());
	if ( cairo_surface_status(cs) != CAIRO_STATUS_SUCCESS ) throw 1;
	common_constructor();
}

GLCairoSurface::~GLCairoSurface() {
	cairo_destroy(ct);
	cairo_surface_destroy(cs);
	for (int i = 0; i < MAX_ALT; ++i) {
		if (pfda[i]) pango_font_description_free(pfda[i]);
	}
	pango_font_description_free(pfd);
	g_object_unref(pc);
}	

int GLCairoSurface::width() {
	return cairo_image_surface_get_width(cs);
}

int GLCairoSurface::height() {
	return cairo_image_surface_get_height(cs);
}

void GLCairoSurface::clear() {
	cairo_save(ct);
	cairo_set_source_rgba(ct, 0,0,0,0);
	cairo_set_operator(ct, CAIRO_OPERATOR_SOURCE);
	cairo_paint(ct);
	cairo_restore(ct);
}

void GLCairoSurface::fill(int r, int g, int b, int a) {
	double fr, fg, fb, fa;
	fr = (double) r / 255.0;
	fg = (double) g / 255.0;
	fb = (double) b / 255.0;
	fa = (double) a / 255.0;
	cairo_save(ct);
	cairo_set_source_rgba(ct, fr,fg,fb,fa);
	cairo_set_operator(ct, CAIRO_OPERATOR_SOURCE);
	cairo_paint(ct);
	cairo_restore(ct);
}

void GLCairoSurface::fill_match_a(int r, int g, int b, int a) {
	unsigned char* p = cairo_image_surface_get_data(cs);
	int h = cairo_image_surface_get_height(cs);
	int w = cairo_image_surface_get_width(cs);
	for (unsigned char* i = p; i < p + 4*h*w; i += 4) {
		if ( *(i+3) == a ) {
			*i = b;
			*(i+1) = g;
			*(i+2) = r;
		}
	}
}

void GLCairoSurface::drawrect(double x_l, double x_h, double y_l, double y_h) {
	cairo_rectangle(ct, x_l, y_l, x_h-x_l, y_h-y_l);
	cairo_fill(ct);
}

void GLCairoSurface::paintgl(float x, float y) {
	unsigned char* p = cairo_image_surface_get_data(cs);
	glRasterPos3f(x, y, 0.0f);
	glPixelZoom(1.0f, -1.0f);
	glDrawPixels(width(), height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, p);
}

void GLCairoSurface::painton(GLCairoSurface* glcs2, int x, int y, double alpha) {
	cairo_set_source_surface(glcs2->ct, cs, (double) x, (double) y);
	cairo_paint_with_alpha(glcs2->ct, alpha);
}

void GLCairoSurface::mixfrom(GLCairoSurface* s1, GLCairoSurface* s2, double alpha) {
	unsigned char* p1 = cairo_image_surface_get_data(s1->cs);
	unsigned char* p2 = cairo_image_surface_get_data(s2->cs);
	unsigned char* pd = cairo_image_surface_get_data(cs);
	unsigned int h = cairo_image_surface_get_height(cs);
	unsigned int w = cairo_image_surface_get_width(cs);
	unsigned short a = (unsigned short) (255.0 * alpha);
	for (unsigned char* i = pd; i < pd + 4*h*w; ++i) {
		*i = (unsigned char) ( ((unsigned short) *p1 * a + (unsigned short) *p2 * (255-a)) / 255 );
		++p1;
		++p2;
	}
}

void GLCairoSurface::color(int r, int g, int b) {
	double fr, fg, fb;
	fr = (double) r;
	fg = (double) g;
	fb = (double) b;
	cairo_set_source_rgb(ct, fr/255.0, fg/255.0, fb/255.0);
}

bool GLCairoSurface::setfontface(const string& face, bool bold, bool italic, bool smallcaps, int X) {
	pango_font_description_set_family(pfd, face.c_str());
	if (bold) pango_font_description_set_weight(pfd, PANGO_WEIGHT_BOLD);
	else pango_font_description_set_weight(pfd, PANGO_WEIGHT_NORMAL);
	if (italic) pango_font_description_set_style(pfd, PANGO_STYLE_ITALIC);
	else pango_font_description_set_style(pfd, PANGO_STYLE_NORMAL);
	if (smallcaps) pango_font_description_set_variant(pfd, PANGO_VARIANT_SMALL_CAPS);
	else pango_font_description_set_variant(pfd, PANGO_VARIANT_NORMAL);
	X_glyph = X;
	return true;
}

bool GLCairoSurface::setfontfacealt(unsigned int index, const string& face, bool bold, bool italic, bool smallcaps, int X) {
	if (index < 0 || index > MAX_ALT) return false;
	if (!pfda[index]) pfda[index] = pango_font_description_new();
	pango_font_description_set_family(pfda[index], face.c_str());
	if (bold) pango_font_description_set_weight(pfda[index], PANGO_WEIGHT_BOLD);
	else pango_font_description_set_weight(pfda[index], PANGO_WEIGHT_NORMAL);
	if (italic) pango_font_description_set_style(pfda[index], PANGO_STYLE_ITALIC);
	else pango_font_description_set_style(pfda[index], PANGO_STYLE_NORMAL);
	if (smallcaps) pango_font_description_set_variant(pfda[index], PANGO_VARIANT_SMALL_CAPS);
	else pango_font_description_set_variant(pfda[index], PANGO_VARIANT_NORMAL);
	X_glyph_a[index] = X;
	return true;
}


bool GLCairoSurface::setfontsize(int size) {
	if (!pfd) return false;
	pango_font_description_set_size(pfd, size*PANGO_SCALE);
	norm_size = size;
	return true;
}

bool GLCairoSurface::setaa(bool mode) {
	cairo_font_options_t* cfo = cairo_font_options_create();
	if (mode) cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_GRAY);
	else cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_NONE);
	pango_cairo_context_set_font_options(pc, cfo);
	pango_cairo_update_context(ct, pc);
	cairo_font_options_destroy(cfo);
	return true;
}


bool GLCairoSurface::writetext(const string& text, int x, int y, int align) {
	if (!pfd) return false;
	if (align < 0 || align > 2) return false;
	PangoLayout* pl = pango_layout_new(pc);
	pango_layout_set_text(pl, text.c_str(), -1);
	pango_layout_set_font_description(pl, pfd);
	int width, height;
	pango_layout_get_pixel_size(pl, &width, &height);
	pango_layout_set_alignment(pl, (PangoAlignment) align);
	cairo_move_to(ct, x-align*width/2, y);
	pango_cairo_show_layout(ct, pl);
	g_object_unref(pl);
	return true;
}

bool GLCairoSurface::writetext(const string& text, int x, int y, int align, int maxwidth) {
	if (!pfd) return false;
	if (align < 0 || align > 2) return false;
	PangoLayout* pl = pango_layout_new(pc);
	pango_layout_set_width(pl, maxwidth * PANGO_SCALE);
	pango_layout_set_text(pl, text.c_str(), -1);
	pango_layout_set_font_description(pl, pfd);
	int width, height;
	pango_layout_get_pixel_size(pl, &width, &height);
	pango_layout_set_alignment(pl, (PangoAlignment) align);
	cairo_move_to(ct, x-align*width/2, y);
	pango_cairo_show_layout(ct, pl);
	g_object_unref(pl);
	return true;
}

bool GLCairoSurface::writetextshrink(const string& text, int x, int y_c, int align, int maxwidth) {
	if (!pfd) return false;
	if (align < 0 || align > 2) return false;
	PangoLayout* pl = pango_layout_new(pc);
	pango_layout_set_width(pl, maxwidth * PANGO_SCALE);
	pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_text(pl, text.c_str(), -1);
	pango_layout_set_font_description(pl, pfd);
	int shrink_size = norm_size;
	PangoFontDescription* pfdp = pfd;
	int X_glyph_p = X_glyph;
	unsigned int pfdi = 0;
	while ( ( pango_layout_get_line_count(pl) > 1 ) && shrink_size > 4 ) {
		if (pfdi < MAX_ALT && pfda[pfdi]) {
			pfdp = pfda[pfdi];
			X_glyph_p = X_glyph_a[pfdi];
			++pfdi;
			pango_font_description_set_size(pfdp, shrink_size*PANGO_SCALE);
			pango_layout_set_font_description(pl, pfdp);
		}
		else {
			shrink_size -= 2;
			pango_font_description_set_size(pfdp, shrink_size*PANGO_SCALE);
			pango_layout_set_font_description(pl, pfdp);
		}
	}
	PangoFont* pf = pango_context_load_font(pc, pfdp);
	PangoRectangle ink_rect, log_rect;
	pango_font_get_glyph_extents(pf, X_glyph_p, &ink_rect, &log_rect);
	int char_asc = PANGO_ASCENT(ink_rect) / PANGO_SCALE;
	int line_asc = PANGO_ASCENT(log_rect) / PANGO_SCALE;
	g_object_unref(pf);
	int width = pango_layout_get_width(pl) / PANGO_SCALE;
	pango_layout_set_alignment(pl, (PangoAlignment) align);
	cairo_move_to(ct, x-align*width/2, y_c-(line_asc-char_asc/2));
	pango_cairo_show_layout(ct, pl);
	pango_font_description_set_size(pfdp, norm_size*PANGO_SCALE);
	g_object_unref(pl);
	return true;
}