#include <iostream>
#include <string>
#include <expected>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>

std::string get_work_dir_path();

std::string get_fifo_path();

std::string get_pid_file_path();

std::expected<std::string, std::system_error> get_absolute_path(const std::string& path);

bool file_exists(const std::string& path);

bool is_regular_file(const std::string& path);

bool is_directory(const std::string& path);

std::string get_current_dir();

std::string get_filename(const std::string& path);


