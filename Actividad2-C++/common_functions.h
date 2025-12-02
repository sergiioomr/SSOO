/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file common_functions.h
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-29
 * @brief Definición de algunas funciones de uso común de backup y backup-server
 */

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

std::string get_client_pid_file_path();

std::expected<std::string, std::system_error> get_absolute_path(const std::string& path);

bool file_exists(const std::string& path);

bool is_regular_file(const std::string& path);

bool is_directory(const std::string& path);

std::string get_current_dir();

std::string get_filename(const std::string& path);

std::expected<pid_t, std::system_error> read_server_pid(const std::string& pid_file);

std::expected<pid_t, std::system_error> read_client_pid(const std::string& pid_file);

bool is_server_running(pid_t pid);


