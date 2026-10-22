#
# Part of CMake configuration for lexertl library
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
add_executable(regex_tests main.cpp)
target_link_libraries(regex_tests PRIVATE lexertl)
add_test(NAME regex_tests COMMAND $<TARGET_FILE:regex_tests>)
