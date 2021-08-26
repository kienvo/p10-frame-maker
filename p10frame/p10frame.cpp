/**
 * @file		p10frame.cpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		Frame file
 * @version		0.1
 * @date		Aug-21-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */
#include "p10frame/p10frame.hpp"

p10frame::p10frame() {

}
p10frame::p10frame(const char *path) {
	_FrameFile = new std::fstream(path,
		std::fstream::binary 
		| std::fstream::in
		| std::fstream::out 
	);
	assert(_FrameFile->is_open()); // TODO: assert test
}
void p10frame::genTestFile() {
	memcpy(header.magic,"FRAMES",6);
	header.version = 1;
	header.displaySize.nCols = 32;
	header.displaySize.nRows = 16;
	header.nSegments = 2;

	uint32_t colsz = header.displaySize.nCols*(header.displaySize.nRows/8);
	segments.push_back({colsz, header.displaySize});
	segments.push_back({colsz, header.displaySize});

	data.resize(header.nSegments);
	uint8_t j=1;
	for(int i(0); i<header.nSegments; i++) {
		j <<=1;
		data[i].resize(segments[i].size);
		for(auto& ii: data[i]) {
			ii = j;
		}
	}
}
void p10frame::save() {
	_FrameFile->write((const char*)&header,sizeof(HeaderSection));
	for(int i(0); i<header.nSegments; i++) {
		_FrameFile->write((const char*)&segments[i],sizeof(FrameSegment));
		_FrameFile->write((const char*)data[i].data(),data[i].size());
	}
	updateCRC();
	_FrameFile->write((const char*)&footer,sizeof(FooterSection));
	_FrameFile->close();
}
void p10frame::updateCRC() {
	boost::crc_16_type crc16;

	crc16.process_bytes(&header,sizeof(HeaderSection));
	for(int i(0); i<header.nSegments; i++) {
		crc16.process_bytes(&segments[i],sizeof(FrameSegment));
		crc16.process_bytes(data[i].data(),data[i].size());
	}
	footer.crc16 = crc16.checksum();
	std::cerr << "checksum test " << crc16.checksum() << std::endl;
}