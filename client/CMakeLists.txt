include_directories(include)
file (GLOB SOURCES src/*.c)
remove (SOURCES src/main.c)
add_library(client STATIC ${SOURCES})
target_link_libraries(client http)

add_executable(http_client src/main.c)
target_link_libraries(http_client client)

if (BUILD_TESTING)
    add_executable(test_client tests/client.c)
    target_link_libraries(test_client client)
    add_test(NAME client COMMAND test_client)
endif()
