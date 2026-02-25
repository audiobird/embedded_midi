cmake_minimum_required(VERSION 3.30)

add_library(embedded_midi INTERFACE)

target_include_directories(embedded_midi INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
