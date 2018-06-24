#pragma once

#include <vector>
#include <string>
#include "utils.h"


namespace teditor {

/** Stores visited files history */
class FilesHist: public std::vector<FileInfo> {
public:
    FilesHist(const std::string& f, int max);

    /** prune the history if it crosses 'maxLen' */
    void prune();

    /** store the history list back to a file */
    void store() const;

    /** adds a element into the history */
    void add(const std::string& file, int line);

    /** generate string version of the history */
    Strings toString() const;

private:
    /** file which is used to load/store the history */
    std::string file;
    /** max files in the history */
    int maxLen;
};

} // end namespace teditor
