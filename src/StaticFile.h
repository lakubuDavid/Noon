//
// Created by David Lakubu on 06/04/2023.
//

#ifndef STATICFILE_H
#define STATICFILE_H

#include <string>
#include <cstdio>
#include <fstream>
#include <boost/filesystem.hpp>


enum FileType {
	FILE_TYPE_UNKNOWN = 0,

	FILE_TYPE_TEXT,

	FILE_TYPE_IMAGE_JPEG,
	FILE_TYPE_IMAGE_PNG,
	FILE_TYPE_IMAGE_GIF,

	FILE_TYPE_OTHER
};

struct StaticFile {
	bool should_free = false;
	char *data;
	unsigned char *image_data;
	char *path = (char *) "";
	std::string filename;
	size_t size;
	FileType type;

	~StaticFile();
};

FileType getFileType(const std::string &path);

bool openFile(const std::string &path, StaticFile &info);

unsigned char *readImageFile(const std::string &path);

char *readTextFile(const std::string &path);

char *readBinaryFile(const std::string &path);

#endif //STATICFILE_H
