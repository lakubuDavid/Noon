//
// Created by David Lakubu on 06/04/2023.
//

#include <sstream>
#include "StaticFile.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

FileType getFileType(const std::string& path) {
    auto p = boost::filesystem::path(path);
    std::string ext = p.extension().string();
    if (ext == ".gif") {
        return FILE_TYPE_IMAGE_GIF;
    } else if (ext == ".png") {
        return FILE_TYPE_IMAGE_PNG;
    } else if (ext == ".jpg" || ext == ".jpeg") {
        return FILE_TYPE_IMAGE_JPEG;
    } else if (ext == ".html" || ext == ".css" || ext == ".htm" || ext == ".js" || ext == ".json" || ext == ".xml") {
        return FILE_TYPE_TEXT;
    }
    return FILE_TYPE_UNKNOWN;
}

bool openFile(const std::string &path, File &info) {
    auto p = boost::filesystem::path(path);
//    file.path = path;
    auto type = getFileType(path);
    switch (type) {
        case FILE_TYPE_TEXT:
            info.data = readTextFile(path);
            break;
        case FILE_TYPE_IMAGE_JPEG:
        case FILE_TYPE_IMAGE_PNG:
        case FILE_TYPE_IMAGE_GIF:
//            file = readBinaryFile(path);
            info.image_data = readImageFile(path);
            info.should_free = true;
            break;
        case FILE_TYPE_OTHER:
        case FILE_TYPE_UNKNOWN:
            info.data =readBinaryFile(path);
            info.should_free = true;
            break;
    }
    info.filename = p.filename().string();
//    file.path = boost::filesystem::canonical(path).string();
    info.type = type;
    return info.data != NULL;
}

unsigned char * readImageFile(const std::string& path) {
    int width, height, nrChannels;
    auto data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    return data;
}

char* readTextFile(const std::string& path) {
    std::ifstream f = std::ifstream(path.c_str());
    std::string line;

    auto text = std::stringstream();
    while (std::getline(f, line)) {
        text << line << std::endl;
    }

    auto str = text.str();

    const size_t response_size = strlen(str.c_str());

    char *cstr = (char *) malloc(response_size);

    strcat(cstr, str.c_str());

    return  cstr;
}

char* readBinaryFile(const std::string& path) {
    FILE *f = fopen(path.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    auto size = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto data = (char *)malloc(size);
    fread(&data, sizeof(char), size, f);
    fclose(f);
    return data;
}

File::~File() {
if(this->should_free)
    free(this->data);
}
