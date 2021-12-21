#include <iostream>
#include <map>
#include <fstream>
#include <cstring>

std::map<int, std::ifstream *> file_map;
std::map<int, std::ofstream *> writer_map;
int idx = 1;

extern "C" {

int fs_open(const char *path, int mode) {
    if (mode == 1) {
        std::ifstream *reader = new std::ifstream(path);
        if (reader->is_open()) {
            ++idx;
            file_map[idx] = reader;
            return idx;
        }
    } else if (mode == 2) {
        std::ofstream *writer = new std::ofstream(path);
        writer->open(path);
        if (writer->is_open()) {
            ++idx;
            writer_map[idx] = writer;
            return idx;
        }
    }
    return 0;
}

bool fs_eof(int fd) {
    std::ifstream *reader = file_map[fd];
    if (reader == nullptr) return true;
    return reader->eof();
}

int fs_get(int fd) {
    if (fs_eof(fd)) return 0;
    std::ifstream *reader = file_map[fd];
    return reader->get();
}

void fs_writeln(int fd, const char *input) {
    std::ofstream *writer = writer_map[fd];
    if (writer == nullptr) return;
    //writer->write(input, strlen(input));
    *writer << std::string(input);
    *writer << std::endl;
}

void fs_close(int fd) {
    std::ifstream *reader = file_map[fd];
    if (reader == nullptr) {
        std::ofstream *writer = writer_map[fd];
        if (writer == nullptr) return;
        writer->close();
        return;
    }
    reader->close();
    file_map[fd] = nullptr;
}

}