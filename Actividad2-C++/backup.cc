/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Sistemas Operativos
 * 
 * @file backup.cc
 * @author Sergio Molina Ríos (alu0101718194@ull.edu.es)
 * @date 2025-11-29
 * @brief Archivo donde se definen las funciones del programa backup
 */

#include "common_functions.h"

bool check_args(int argc, char* argv[]) {

}

std::expected<void, std::system_error> check_work_dir_exists(const std::string& work_dir) {

}

std::expected<int, std::system_error> open_fifo_write(const std::string& fifo_path) {

}

std::expected<void, std::system_error> write_path_to_fifo(int fifo_fd, const std::string& file_path) {

}

