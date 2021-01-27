#ifndef LANMAI_COMMOON_H
#define LANMAI_COMMOON_H

#include "config.h"
#include "log.h"
#include "err.h"
#include "common.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <asm/errno.h>
#include <exception>

std::string read_file(const std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        LLOG(LL_ERROR, "open file:%s failed, %s", path.c_str(), strerror(fd));
        throw OPEN_FILE_ERROR;
    }
    Defer defer([fd](){ close(fd); });

    struct stat sb;
    fstat(fd, &sb);

    std::string s;
    s.resize(sb.st_size);

    read(fd, s.data(), sb.st_size);
    return s;
}

json readConfig(const std::string &path) {
    return json::parse(read_file(path));
}

#endif
