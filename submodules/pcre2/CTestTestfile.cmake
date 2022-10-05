# CMake generated Testfile for 
# Source directory: /sdcard/libunalix/submodules/pcre2
# Build directory: /sdcard/libunalix/submodules/pcre2
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(pcre2_test "sh" "/sdcard/libunalix/submodules/pcre2/pcre2_test.sh")
set_tests_properties(pcre2_test PROPERTIES  _BACKTRACE_TRIPLES "/sdcard/libunalix/submodules/pcre2/CMakeLists.txt;995;ADD_TEST;/sdcard/libunalix/submodules/pcre2/CMakeLists.txt;0;")
add_test(pcre2_grep_test "sh" "/sdcard/libunalix/submodules/pcre2/pcre2_grep_test.sh")
set_tests_properties(pcre2_grep_test PROPERTIES  _BACKTRACE_TRIPLES "/sdcard/libunalix/submodules/pcre2/CMakeLists.txt;1008;ADD_TEST;/sdcard/libunalix/submodules/pcre2/CMakeLists.txt;0;")
