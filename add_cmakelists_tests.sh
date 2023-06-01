#!/bin/bash
for f in ./tests/*.c; do
    f=$(echo $f | sed s/\.\\/tests\\///)
    if [ $(grep $f CMakeLists.txt | wc -l) -eq 0 ]; then
        test_target=$(echo $f | sed s/\\.c//)
        echo "Adding test $test_target to CMakeLists.txt"
        echo "add_executable(test_$test_target tests/$f)" >> CMakeLists.txt
        echo "target_link_libraries(test_$test_target httpserver)" >> CMakeLists.txt
        echo "add_test(NAME $test_target COMMAND test_$test_target)" >> CMakeLists.txt
    fi
done