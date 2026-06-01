add_executable(parser
    parser.cpp
)

target_include_directories(parser PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/
)

add_executable(usb_parser
    usb_parser.cpp
)

target_include_directories(usb_parser PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/
)

target_compile_options(usb_parser PRIVATE
    -fsanitize=undefined,address
)

target_link_options(usb_parser PRIVATE
    -fsanitize=undefined,address
)
