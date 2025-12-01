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
#include <atomic>
#include "copy.h"

std::atomic<bool> quit_requested{false};

std::expected<void, std::system_error> create_fifo(const std::string& fifo_path);

std::expected<void, std::system_error> write_pid_file(const std::string& pid_file_path);

void signal_handler(int signum);

std::expected<void, std::system_error> setup_signal_handler();

std::expected<std::string, std::system_error> read_path_from_fifo(int fifo_fd);

void run_server(int fifo_fd, const std::string& backup_dir);







