#ifndef _gl_freetype_h_
#define _gl_freetype_h_

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

//OpenGL Headers 
#ifdef WINDOWS
#include <windows.h>		//(the GL headers need it)
#endif
#include <GL/gl.h>
#include <GL/glu.h>

//Some STL headers
#include <vector>
#include <string>

#include <math.h>

//Using the STL exception library increases the
//chances that someone else using our code will corretly
//catch any exceptions that we throw.
//#include <stdexcept>
#include <iostream>

//MSVC will spit out all sorts of useless warnings if
//you create vectors of strings, this pragma gets rid of them.
#pragma warning(disable: 4786) 

//Required freetype library.
#pragma comment(lib, "freetype6.lib")

///Wrap everything in a namespace, that we can use common
///function names like "print" without worrying about
///overlapping with anyone else's code.
namespace freetype {

	struct entry {
		int w;		// width
		GLuint t;	// texture pointer
		entry() { w = -1; t = 0; }
	};

	struct block {
		GLuint t[128];		// texture pointers
		short w[128];		// widths
		block() {
			glGenTextures( 128, t );
			memset((void*) w, 0xff, 128*sizeof(short));
		}
		~block() {
			glDeleteTextures( 128, t );
		}
	};

	class mem {

	private:
		block* blocks[512];  // blocks of 128 entries
		void create_block(unsigned int blk);

	public:
		mem();
		~mem();
		int get_w(unsigned int ch);
		void set_w(unsigned int ch, int w);
		GLuint get_t(unsigned int ch);
		void set_t(unsigned int ch, GLuint t);
		bool is_alloc(unsigned int ch);
		void alloc(unsigned int ch);

	};

	// various typedefs
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef std::basic_string<ushort> ucs2string;

	// number of textures (characters)
	const unsigned int LIST_LEN = 65536;

	// list for box drawing stuff
	const unsigned short unichar_low[] = { 
		0x25ba, 0x25c4, 0x0020, 0x0020,
		0x2591, 0x2592, 0x2593, 0x2588,
		0x2191, 0x2193, 0x2192, 0x2190,
		0x0020, 0x0020, 0x25b2, 0x25bc };
	// list for Win-1252 + extras
	const unsigned short unichar_high[] = {
		0x20ac, 0x2212, 0x201a, 0x0192,
		0x201e, 0x2026, 0x2020, 0x2021,
		0x02c6, 0x2030, 0x0160, 0x2039,
		0x0152, 0x0020, 0x017d, 0x0020,
		0x2007, 0x2018, 0x2019, 0x201c,
		0x201d, 0x2022, 0x2013, 0x2014,
		0x02dc, 0x2122, 0x0161, 0x203a,
		0x0153, 0x0020, 0x017e, 0x0178 };

	//Inside of this namespace, give ourselves the ability
	//to write just "vector" instead of "std::vector"
	using std::vector;

	//Ditto for string.
	using std::string;

	//This holds all of the information related to any
	//freetype font that we want to create.  
	class font_data {
	private:
		bool initflag;
		bool remapflag;
		float h;				///< Holds the height of the font.
		GLuint list_base;		///< Holds the first display list id
		mem m;					///< Holds the widths and textures of the characters

		// Freetype stuff (now needed all the time for dynamic loading)
		FT_Library library;
		FT_Face face;

		void make_dlist( ushort ch );
		void make_dlist( uint glyph_id, ushort ch );
		inline int load_and_get_width(ushort us);
		void split_and_convert(uchar* text, vector<ucs2string>& lines, vector<int>& widths);
		//float text_length(const ushort* text);

	public:
		// alignment constants
		const static int LEFT = 0;
		const static int CENTER = 1;
		const static int RIGHT = 2;

		// The varargs print function.
		void print(float x, float y, int align, const char *fmt, ...);
		void qprint(float x, float y, int align, const char *fmt, ...);

		//The init function will create a font of
		//of the height h from the file fname.
		void init(const char * fname, unsigned int size, ushort low, ushort high);

		//Load glyphs to different positions.
		int load_as(uint glyph_id, ushort char_id, uint num);

		//Set the remap flag.
		void remap(bool state) { remapflag = state; }

		//Free all the resources assosiated with the font.
		void clean();
		font_data() { initflag = false; remapflag = true; }
		~font_data();
	};

	//The flagship function of the library - this thing will print
	//out text at window coordinates x,y, using the font ft_font.
	//The current modelview matrix will also be applied to the text. 
	// Replaced with member function.
	// void print(const font_data &ft_font, float x, float y, const char *fmt, va_list apin) ;

}

#endif
