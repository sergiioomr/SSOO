/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file backup-server.h
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-30
 * @brief Definición de las funciones de backup-server.cc
 */

#include "common_functions.h"
#include "copy.h"
#include <atomic>
#include <vector>

std::atomic<bool> quit_requested{false};

std::expected<void, std::system_error> create_fifo(const std::string& fifo_path);

std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path);

void signal_handler(int signum);

std::expected<void, std::system_error> setup_signal_handler();

std::expected<std::string, std::system_error> read_path_from_fifo(int fifo_fd);

void run_server(int fifo_fd, const std::string& backup_dir, const ServerOptions& config);

struct ServerOptions {
  enum class CompressionType {
    NONE,
    GZIP,
    BZIP2,
    XZ
  };
  CompressionType compression = CompressionType::NONE;
  std::string backup_dir;
};
enum class ParseArgsErrors {
  unknown_option,
  multiple_compression_options,
  too_many_arguments,
};

std::expected<ServerOptions, ParseArgsErrors> parse_arguments(int argc, char* argv[]);

std::string get_compression_command(ServerOptions::CompressionType compression);

bool is_command_available(const std::string& command);

std::string get_compression_extension(ServerOptions::CompressionType compression);

enum class CopyFileCompressedError {
  command_access_denied,
  command_execution_failed,
  command_not_found,
  output_access_denied,
  process_creation_failed,
  pipe_creation_failed,
  unknown_error
};


std::expected<void, CopyFileCompressedError> copy_file_compressed(const std::string& src_path, const std::string& dest_path, const std::string& compression_command);









