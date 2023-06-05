include_directories(include)
file(GLOB SOURCES src/*.c)
add_library(http STATIC ${SOURCES})

if (BUILD_TESTING) 
	add_executable(test_http tests/http.c)
	target_link_libraries(test_http http)
	add_test(NAME http COMMAND test_http)

	add_executable(test_read_line tests/read_line.c)
	target_link_libraries(test_read_line http)
	add_test(NAME read_line COMMAND test_read_line)

	add_executable(test_search_tree tests/search_tree.c)
	target_link_libraries(test_search_tree http)
	add_test(NAME search_tree COMMAND test_search_tree)

	add_executable(test_trim_whitespace tests/trim_whitespace.c)
	target_link_libraries(test_trim_whitespace http)
	add_test(NAME trim_whitespace COMMAND test_trim_whitespace)

	add_executable(test_files tests/files.c)
	target_link_libraries(test_files http)
	add_test(NAME test_files COMMAND test_files)

    set_tests_properties(http read_line search_tree trim_whitespace PROPERTIES LABELS "libhttp")
endif()