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
const p10frame::FrameSegment* p10frame::getSegmentNData(int index, std::vector<uint8_t>& data) const {
	if(index >= header.nSegments) return nullptr;
	if(!_FrameFile->is_open()) return nullptr;
	data.resize(segments[index].frameSize.nPixels());
	for(int i(0); i<segments[index].frameSize.nPixels(); i++) {
		data[i] = mdata[index][i];
	}
	return &segments[index];
}
void p10frame::setSegmentData(int index, const FrameSegment& seg, const std::vector<uint8_t>& data) {
	if(index > header.nSegments){
		std::cerr << "Warning: Number of Segments overflow" << std::endl;
		segments.resize(index);
		mdata.resize(index);
	} 
	segments[index] = seg;
	mdata[index].resize(seg.frameSize.nPixels());
	for(int i(0); i<mdata[index].size(); i++) {
		mdata[index][i]= data[i];
	}
}
p10frame::p10frame() {

}
p10frame::p10frame(const char *path, std::ios_base::openmode mode) {
	_FrameFile = new std::fstream(path, mode);
	if(!_FrameFile->is_open()) {
		_error = "Cannot open file";
		return;
	}
	if(mode & std::fstream::in) {
		if(!readFile()) _error = "File is corrupted";
	}
}
void p10frame::genSampleFile() {
	memcpy(header.magic,"FRAMES",6);
	header.version = 1;
	header.displaySize.nCols = 16;
	header.displaySize.nRows = 8;
	header.nSegments = 2;

	segments.push_back({header.displaySize});
	segments.push_back({header.displaySize});

	mdata.resize(header.nSegments);
	uint8_t j=1;
	for(int i(0); i<header.nSegments; i++) {
		j <<=1;
		mdata[i].resize(segments[i].frameSize.nRows*segments[i].frameSize.nCols);
		for(auto& ii: mdata[i]) {
			ii = j;
		}
	}
}
void p10frame::save() {
	memcpy(header.magic,"FRAMES",6);
	header.version = 1;
	_FrameFile->write((const char*)&header,sizeof(HeaderSection));
	for(int i(0); i<header.nSegments; i++) {
		_FrameFile->write((const char*)&segments[i],sizeof(FrameSegment));
		_FrameFile->write((const char*)mdata[i].data(),mdata[i].size());
	}
	updateCRC();
	_FrameFile->write((const char*)&footer,sizeof(FooterSection));
	_FrameFile->close();
}

uint16_t p10frame::crc_process() {
	boost::crc_16_type crc16;

	crc16.process_bytes(&header,sizeof(HeaderSection));
	for(int i(0); i<header.nSegments; i++) {
		crc16.process_bytes(&segments[i],sizeof(FrameSegment));
		crc16.process_bytes(mdata[i].data(),mdata[i].size());
	}
	return crc16.checksum();
}

bool p10frame::verify() {
	return (crc_process() == footer.crc16) && (strncmp(header.magic, "FRAME",6));
}

void p10frame::updateCRC() {
	boost::crc_16_type crc16;

	crc16.process_bytes(&header,sizeof(HeaderSection));
	for(int i(0); i<header.nSegments; i++) {
		crc16.process_bytes(&segments[i],sizeof(FrameSegment));
		crc16.process_bytes(mdata[i].data(),mdata[i].size());
	}
	footer.crc16 = crc16.checksum();
	std::cerr << "checksum test " << crc16.checksum() << std::endl;
}
bool p10frame::readFile() {
	_FrameFile->seekg(0);
	_FrameFile->read((char*)&header,sizeof(HeaderSection));
	segments.resize(header.nSegments);
	mdata.resize(header.nSegments);
	for(int i(0); i<header.nSegments; i++) {
		_FrameFile->read((char*)&segments[i],sizeof(FrameSegment));
		mdata[i].resize(segments[i].frameSize.nPixels());
		_FrameFile->read((char*)&mdata[i][0],mdata[i].size()); // XXX: old school memcpy
	}
	_FrameFile->read((char*)&footer, sizeof(FooterSection));
	return verify();
}