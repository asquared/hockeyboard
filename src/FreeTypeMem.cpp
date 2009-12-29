#include "FreeType.h"

namespace freetype {

	mem::mem() {
		for (int i = 0; i < 512; ++i) {
			blocks[i] = 0;
		}
	}

	mem::~mem() {
		for (int i = 0; i < 512; ++i) {
			if (blocks[i]) { 
				delete blocks[i];
#ifdef _DEBUG
				std::cout << "block " << i << " deallocated" << std::endl; 
#endif
			}
		}
	}

	void mem::create_block(unsigned int blk) {
		blocks[blk] = new block;
#ifdef _DEBUG
		std::cout << "block " << blk << " allocated" << std::endl;
#endif
	}

	int mem::get_w(unsigned int ch) {
		block* b = blocks[ch >> 7];
		if (!b) return -1;
		return blocks[ch >> 7]->w[ch & 0x7f];
	}

	void mem::set_w(unsigned int ch, int w) {
		if (!blocks[ch >> 7]) create_block(ch >> 7);
		blocks[ch >> 7]->w[ch & 0x7f] = w;
	}
	
	GLuint mem::get_t(unsigned int ch) {
		if (!blocks[ch >> 7]) return 0xffffffff;
		return blocks[ch >> 7]->t[ch & 0x7f];
	}
	
	void mem::set_t(unsigned int ch, GLuint t) {
		if (!blocks[ch >> 7]) create_block(ch >> 7);
		blocks[ch >> 7]->t[ch & 0x7f] = t;		
	}
	
	bool mem::is_alloc(unsigned int ch) {
		block* b = blocks[ch >> 7];
		if (!b) return false;
		if (b->w[ch & 0x7f] == -1) return false;
		return true;
	}

	void mem::alloc(unsigned int ch) {
		if (!blocks[ch >> 7]) create_block(ch >> 7);
	}

}