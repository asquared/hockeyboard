/*
	A quick and simple opengl font library that uses GNU freetype2, written
	and distributed as part of a tutorial for nehe.gamedev.net.
	Sven Olsen, 2003
*/

// Modified by Rob Humphreys 
// (made into a real class, fixed rendering bugs, added Unicode BMP support)

//Include our header file.
#include "freetype.h"

namespace freetype {

// This function gets the first power of 2 >= the
// int that we pass it.
inline int next_p2(int a) {
	int rval = 1;
	while (rval < a) rval <<= 1;
	return rval;
}

// get width and dynamically load texture if necessary
inline int font_data::load_and_get_width(ushort us) {
	int w = m.get_w(us);
	if (w != -1) return w;
	else {
		make_dlist(us);
		w = m.get_w(us);
		return w;
	}
	return 0;
}

// split into lines and convert UTF-8 to UCS-2
void font_data::split_and_convert(uchar* text, vector<ucs2string>& lines, vector<int>& widths) {
	ucs2string line;
	int width = 0;
	ushort partial = 0;
	uchar bytes_left = 0;
	//const uchar* start_line=text;
	uchar* c;
	for (c = text; *c; c++) {
		ushort us = (ushort) *c;
		if (us == '\n') {
			lines.push_back(line);
			line.clear();
			widths.push_back(width);
			width = 0;
			//start_line=c+1;
		}
		else if (us < 0x80) {
			line.push_back(us);
			width += load_and_get_width(us);
		}
		else if (us < 0xc0) {
			if (bytes_left) {
				--bytes_left;
				partial += (us & 0x3f) << (bytes_left * 6);
				if (!bytes_left) {
					line.push_back(partial);
					width += load_and_get_width(partial);
					partial = 0;
				}
			}
			// else ignore, invalid without higher lead byte
		}
		else if (us < 0xe0) {
			partial = (us & 0x1f) << 6;
			bytes_left = 1;
		}
		else if (us < 0xf0) {
			partial = (us & 0x0f) << 12;
			bytes_left = 2;
		}
		else {
			// unsupported, should just continue ignoring 0x80-0xbf bytes
		}
	}
	if (line.length()) {
		lines.push_back(line);
		widths.push_back(width);
	}

}

void font_data::make_dlist(ushort ch) {
	make_dlist(ch,ch);
}

///Create a display list corresponding to the given character.
///glyph_id = actual font glyph id (32 bits), ch = id for printing to screen (16 bits)
void font_data::make_dlist(uint glyph_id, ushort ch) {

	// get the texture reference
	m.alloc(ch);
	GLuint tex = m.get_t(ch);

	// Translate Win-1252 with additions to Unicode
	if (remapflag) {
		if (glyph_id >= 16 && glyph_id < 32) glyph_id = unichar_low[glyph_id-16];
		else if (glyph_id >= 128 && glyph_id < 160) glyph_id = unichar_high[glyph_id-128];
	}

	// Load the Glyph for our character.
	if (FT_Load_Glyph(face, FT_Get_Char_Index(face, glyph_id), FT_LOAD_DEFAULT))
		throw std::runtime_error("FT_Load_Glyph failed");

	// Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph))
		throw std::runtime_error("FT_Get_Glyph failed");

	// Convert the glyph to a bitmap.
	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) glyph;

	// This reference will make accessing the bitmap easier
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	// Use our helper function to get the widths of
	// the bitmap data that we will need in order to create
	// our texture.
	// The max(2, ...) is used to correct for OpenGL, which 
	// apparently does not like to have rows less than 4 bytes
	// long.  Probably a packing thing.
	int width = max(2, next_p2(bitmap.width));
	int height = next_p2(bitmap.rows);

	// Allocate memory (temporarily) for the texture data.
	GLubyte* expanded_data = new GLubyte[2 * width * height];

	// Here we fill in the data for the expanded bitmap.
	// Notice that we are using two channel bitmap (one for
	// luminocity and one for alpha).
	// Luma always 255.  Alpha stays alpha.
	// (unless of course, it is premultiplied, which in OpenGL it is not.)
	// We use the ?: operator so that value which we use
	// will be 0 if we are in the padding zone, and whatever
	// is the Freetype bitmap otherwise.
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			expanded_data[ 2*(i+j*width)   ] = 255;
			expanded_data[ 2*(i+j*width)+1 ] = 
				(i >= bitmap.width || j >= bitmap.rows) ?
				0 : bitmap.buffer[i + bitmap.width*j];
		}
	}

	// Now we just setup some texture paramaters.
    glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Here we actually create the texture itself, notice
	// that we are using GL_LUMINANCE_ALPHA to indicate that
	// we are using 2 channel data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		  0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);


	// With the texture created, we don't need the expanded data anymore
    delete [] expanded_data;

	// So now we can create the display list
	glNewList(list_base + ch, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, tex);

	//glPushMatrix();

	// First we need to move over a little so that
	// the character has the right amount of space
	// between it and the one before it.
	//glTranslatef((float) bitmap_glyph->left,0,0);

	// Now we move down a little in the case that the
	// bitmap extends past the bottom of the line 
	// (this is only true for characters like 'g' or 'y'.
	//glTranslatef(0,(float)bitmap_glyph->top-bitmap.rows,0);
	float left_pad = (float) bitmap_glyph->left;
	float bot_pad = (float) bitmap.rows-bitmap_glyph->top;
	glTranslatef(left_pad, -bot_pad, 0);

	// Now we need to account for the fact that many of
	// our textures are filled with empty padding space.
	// We figure what portion of the texture is used by 
	// the actual character and store that information in 
	// the x and y variables, then when we draw the
	// quad, we will only reference the parts of the texture
	// that we contain the character itself.
	float	x = (float) bitmap.width / (float) width,
			y = (float) bitmap.rows / (float) height;

	// Here we draw the texturemapped quads.
	// The bitmap that we got from FreeType was not 
	// oriented quite like we would like it to be,
	// so we need to link the texture to the quad
	// so that the result will be properly aligned.
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, (float) bitmap.rows);
	glTexCoord2d(0, y); glVertex2f(0, 0);
	glTexCoord2d(x, y); glVertex2f((float) bitmap.width, 0);
	glTexCoord2d(x, 0); glVertex2f((float) bitmap.width, (float) bitmap.rows);
	glEnd();
	//glPopMatrix();

	// Get width, round, and translate
	short ch_width = (short)(face->glyph->linearHoriAdvance >> 16);
	if (face->glyph->linearHoriAdvance & 0x8000) ++ch_width;
	//glTranslatef(ch_width, 0, 0);
	glTranslatef(ch_width-left_pad, bot_pad, 0);

	// The two ways to do width:
	// face->glyph->linearHoriAdvance >> 16
	// face->glyph->advance.x >> 6 
	
	// Finish the display list
	glEndList();

	// Store the width of this character
	m.set_w(ch, ch_width);
}

void font_data::init(const char * fname, unsigned int size, ushort low, ushort high) {

	initflag = true;
	this->h = (float) size;

	// Create and initilize a freetype font library.
	library = 0;
	if (FT_Init_FreeType( &library )) 
		throw std::runtime_error("FT_Init_FreeType failed");

	// The object in which Freetype holds information on a given
	// font is called a "face".
	// This is where we load in the font information from the file.
	// Of all the places where the code might die, this is the most likely,
	// as FT_New_Face will die if the font file does not exist or is somehow broken.
	face = 0;
	if (FT_New_Face( library, fname, 0, &face )) 
		throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

	// For some twisted reason, Freetype measures font size
	// in terms of 1/64ths of pixels.  Thus, to make a font
	// h pixels high, we need to request a size of h*64.
	// (h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size(face, size << 6, size << 6, 96, 96);

	// Here we ask opengl to allocate resources for
	// all the display lists which we are about to create.  
	list_base=glGenLists(LIST_LEN);

	// This is where we actually create each of the fonts display lists.
	for (ushort i = low; i <= high; ++i) {
		make_dlist(i);
	}
}

// load glyphs into arbitrary positions
int font_data::load_as(uint glyph_id, ushort char_id, uint num) {
	int successes = 0;
	
	for ( uint pos = 0; pos < num; ++pos ) {
		int w = m.get_w(char_id + pos);
		if (w == -1) {
			make_dlist(glyph_id + pos, char_id + pos);
			++successes;
		}
	}

	return successes;
}

void font_data::clean() {
	glDeleteLists(list_base, LIST_LEN);

	//We don't need the face information now.
	if (face) FT_Done_Face(face);

	//Ditto for the library.
	if (library) FT_Done_FreeType(library);

	initflag = false;
}

font_data::~font_data() {
	if (initflag) clean();
}

// A fairly straight forward function that pushes
// a projection matrix that will make object world 
// coordinates identical to window coordinates.
inline void pushScreenCoordinateMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
}

// Pops the projection matrix without changing the current
// MatrixMode.
inline void pop_projection_matrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

// Quick print function.  Limited to 8-bit characters (0-255), left alignment, one line.
void font_data::qprint(float x, float y, int align, const char *fmt, ...) {
	// We want a coordinate system where things coresponding to window pixels.
	pushScreenCoordinateMatrix();	

	//We make the height about 1.5* that of
	float ht= h/.63f;						
	
	char		text[256];								// Holds the string (max of 255 bytes)
	va_list		ap;										// Pointer to list of arguments

	if (fmt == NULL)									// If there's no text
		*text=0;										// do nothing

	else {
		va_start(ap, fmt);								// Parses the string for variables
	    vsprintf(text, fmt, ap);						// and converts symbols to actual numbers
		va_end(ap);										// Results are stored in text
	}

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glListBase(list_base);

	float modelview_matrix[16];	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x,y,0);
	glMultMatrixf(modelview_matrix);

	// Call the display lists using the 8-bit text line.	
	glCallLists((GLsizei) strlen(text), GL_UNSIGNED_BYTE, text);

	glPopMatrix();
	glPopAttrib();		

	pop_projection_matrix();
}

// Much like Nehe's glPrint function, but modified to work
// with freetype fonts.
void font_data::print(float x, float y, int align, const char *fmt, ...) {	

	// We want a coordinate system where things coresponding to window pixels.
	pushScreenCoordinateMatrix();	

	//We make the height about 1.5* that of
	float ht= h/.63f;						
	
	char		text[256];								// Holds the string (max of 255 bytes)
	va_list		ap;										// Pointer to list of arguments

	if (fmt == NULL)									// If there's no text
		*text=0;										// do nothing

	else {
		va_start(ap, fmt);								// Parses the string for variables
	    vsprintf(text, fmt, ap);						// and converts symbols to actual numbers
		va_end(ap);										// Results are stored in text
	}

	vector<ucs2string> lines;
	vector<int> widths;

	split_and_convert((uchar*) text, lines, widths);

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glListBase(list_base);

	float modelview_matrix[16];	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	//This is where the text display actually happens.
	//For each line of text we reset the modelview matrix
	//so that the line's text will start in the correct position.
	//Notice that we need to reset the matrix, rather than just translating
	//down by h. This is because when each character is
	//draw it modifies the current matrix so that the next character
	//will be drawn immediatly after it.  
	for (unsigned int i = 0; i < lines.size(); ++i) {
		
		float xmod = x;
		float len =	(float) widths[i];		//= text_length(lines[i].c_str());
		if ( align == CENTER ) xmod -= floor(len / 2);
		else if ( align == RIGHT ) xmod -= len;

		glPushMatrix();
		glLoadIdentity();
		glTranslatef(xmod,y-ht*i,0);
		glMultMatrixf(modelview_matrix);

		// Call the display lists using the UCS-2 line.	
		glCallLists((GLsizei)lines[i].length(), GL_UNSIGNED_SHORT, lines[i].c_str());

		glPopMatrix();

	}


	glPopAttrib();		

	pop_projection_matrix();
}

}
