include_directories(include)
file (GLOB SOURCES src/*.c)
add_executable(http_server ${SOURCES})
target_link_libraries(http_server http pthread m)