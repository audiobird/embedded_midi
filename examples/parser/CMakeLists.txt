add_executable(parser
    parser.cpp
)

target_include_directories(parser PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/
)
