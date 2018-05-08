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
    : full_path(base + path),
      resource_identifier(path[0] == '/' ? path : '/' + path)
{
    const size_t period = path.rfind('.');
    if (period != std::string::npos)
    {
        inferred_resource_type = "text/" + path.substr(period + 1);
    }

    //
    // Load the file now, incase there's an error or something
    //
    load_file();
}


//
// ############################################################################
//

data_buffer file_resource::get_resource()
{
    //
    // We're going to do a quick check to see if the file has changed size
    //
    if (get_last_modified_time() > file.last_modified || get_file_size() != file.size)
    {
        load_file();
    }

    return {file.data, file.size};
}

//
// ############################################################################
//

time_t file_resource::get_last_modified_time()
{
    struct stat statbuf;
    if (stat(full_path.c_str(), &statbuf) < 0)
    {
        throw_errno("Unable to load last modified time");
    }

    return statbuf.st_mtime;
}

//
// ############################################################################
//

size_t file_resource::get_file_size()
{
    //
    // Figure out how much data we're loading
    //
    struct stat statbuf;
    if (stat(full_path.c_str(), &statbuf) < 0)
    {
        throw_errno("Unable to load file size");
    }

    return statbuf.st_size;
}

//
// ############################################################################
//

void file_resource::load_file()
{
    //
    // Load the file off the disk
    //
    int fd = open(full_path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw_errno("Unable to open file");
    }

    file.last_modified = get_last_modified_time();
    file.size = get_file_size();
    file.data = static_cast<char*>(mmap(nullptr, file.size, PROT_READ, MAP_SHARED, fd, 0));
}

//
// ############################################################################
//

void file_resource::throw_errno(const std::string& message)
{
    std::stringstream ss;
    ss << "Exception in resource at " << full_path << ": " << message << " Error: (" << errno << ") " << strerror(errno);

    throw std::runtime_error(ss.str());
}
}
