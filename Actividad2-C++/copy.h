#include <sys/stat.h>
#include <iostream>
#include <libgen.h>
#include <expected>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

bool check_args(int argc, char* argv[]);

bool is_directory(const std::string& path);

std::string get_filename(const std::string& path);

std::expected<void, std::system_error>  copy_file(const std::string& src_path, const std::string& dest_path, mode_t dst_perms=0);

