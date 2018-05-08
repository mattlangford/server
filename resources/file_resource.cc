#include "resources/file_resource.hh"

#include <sstream>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

namespace resources
{

file_resource::file_resource(const std::string& base, const std::string& path)
    : resource_identifier(path[0] == '/' ? path : '/' + path)
{
    const size_t period = path.rfind('.');
    if (period != std::string::npos)
    {
        inferred_resource_type = "text/" + path.substr(period + 1);
    }

    load_file(base + path);
}

//
// ############################################################################
//

void file_resource::load_file(const std::string& file_path)
{
    //
    // Load the file off the disk
    //
    const char* path = file_path.c_str();
    int file = open(path, O_RDONLY);
    if (file < 0)
    {
        throw_errno("Unable to open file by the name of: " + file_path);
    }

    //
    // Figure out how much data we're loading
    //
    struct stat statbuf;
    if (fstat(file, &statbuf) < 0)
    {
        throw_errno("Unable to load filesize of file: " + file_path);
    }
    data_size = statbuf.st_size;

    data = static_cast<char*>(mmap(nullptr, data_size, PROT_READ, MAP_SHARED, file, 0));
}

//
// ############################################################################
//

void file_resource::throw_errno(const std::string& message)
{
    std::stringstream ss;
    ss << message << " Error: (" << errno << ") " << strerror(errno);

    throw std::runtime_error(ss.str());
}
}
