/**
 * @file		p10frame.hpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		Frame file
 * @version		0.1
 * @date		Aug-21-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */

#ifndef __P10FRAME_HPP__
#define __P10FRAME_HPP__

#include <fstream>
#include <iostream>
#include <assert.h>
#include <boost/crc.hpp>
#include <vector>
#include <iterator>
#include <cstring>

class p10frame
{
enum BasicColor {
	BLACK	=	0x00,
	RED		= 	0x01,
	GREEN	= 	0x02, 
	BLUE	= 	0x04,
	MAGENTA	= 	(BLUE | RED),
	YELLOW	= 	(GREEN | RED),
	CYAN	= 	(BLUE | GREEN),
	WHITE	= 	(BLUE | GREEN | RED)
};
struct DisplaySize {
	uint16_t nRows; // Number of segment's rows led
	uint16_t nCols; // Number of segment's  columns led
}__attribute__((packed));

struct HeaderSection
{
	char magic[6]; // "FRAMES" string
	uint8_t version;
	DisplaySize displaySize; // Size of led display
	uint8_t  nSegments; // Numer of Frame Segments
}__attribute__((packed));

struct FrameSegment {
	//uint32_t absOffset;//  
	uint32_t size;  // size of data
	DisplaySize frameSize;
}__attribute__((packed));

struct FooterSection {
	uint16_t crc16;
}__attribute__((packed));

private:
	std::fstream* _FrameFile;
	HeaderSection header;
	std::vector<FrameSegment> segments;
	FooterSection footer;
	std::vector<std::vector<uint8_t>> data;  // 3bit color pixel data //TODO: update to 8bit color
	void updateCRC();

public:
	void genTestFile();
	void save();
	void verify() {
		
	}
	p10frame();
	/**
	 * @brief 		Open existing file
	 * 
	 * @param		path to existing file
	 */
	p10frame(const char *path);
	//~p10frame();
};

#endif /* __P10FRAME_HPP__ */
