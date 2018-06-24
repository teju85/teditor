#include "files_hist.h"


namespace teditor {

FilesHist::FilesHist(const std::string& f, int max): file(f), maxLen(max) {
    if(!isFile(file.c_str()))
        return;
    auto arr = slurpToArr(file);
    for(const auto& a : arr)
        push_back(readFileInfo(a));
    prune();
}

void FilesHist::prune() {
    if((int)size() > maxLen)
        erase(begin()+maxLen, end());
}

void FilesHist::store() const {
    if(empty())
        return;
    FILE* fp = fopen(file.c_str(), "w");
    if(fp == NULL)
        return;
    for(const auto& fi : *this)
        fprintf(fp, "%s:%d\n", fi.first.c_str(), fi.second);
    fclose(fp);
}

void FilesHist::add(const std::string& file, int line) {
    FileInfo fi(file, line);
    // remove duplicates
    for(int i=0;i<(int)size();++i) {
        if(at(i).first == file) {
            erase(begin()+i);
            --i;
        }
    }
    insert(begin(), fi);
    prune();
}

Strings FilesHist::toString() const {
    Strings vec;
    for(const auto& fi : *this)
        vec.push_back(format("%s:%d", fi.first.c_str(), fi.second));
    return vec;
}


} // end namespace teditor
