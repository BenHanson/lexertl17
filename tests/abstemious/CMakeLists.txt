#
# Part of CMake configuration for lexertl library
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
add_executable(abstemious_tests main.cpp)
target_link_libraries(abstemious_tests PRIVATE lexertl)
add_test(NAME abstemious_tests COMMAND $<TARGET_FILE:abstemious_tests>)
