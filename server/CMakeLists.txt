include_directories(include)
file (GLOB SOURCES src/*.c)
remove (SOURCES src/main.c)
add_library(server STATIC ${SOURCES})
target_link_libraries(server http pthread m)

add_executable(http_server src/main.c)
target_link_libraries(http_server server)

if (BUILD_TESTING)
    add_executable(test_server_config tests/config.c)
    target_link_libraries(test_server_config server)
    add_test(NAME server_config COMMAND test_server_config)
endif()

install(TARGETS http_server)
install(DIRECTORY default/html DESTINATION /var/www)
install(FILES default/http_server.conf DESTINATION /etc/http_server)