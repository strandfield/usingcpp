cmake_minimum_required(VERSION 3.9)
project(usingcpp)

# TODO: detect support for C++20
set(CMAKE_CXX_STANDARD 17)

function(add_cpp_executable name path)
  add_executable(${name} ${file_path})
  if (NOT DEFINED WIN32)
    target_link_libraries(${name} pthread)
  endif()
endfunction(add_cpp_executable)

function(add_executables)
  file(GLOB source_list RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*.cpp")
  foreach(file_path ${source_list})
    string(REPLACE ".cpp" "" name ${file_path})
    message("Generating ${file_path}...")
    add_cpp_executable("${name}" ${file_path})
  endforeach()
endfunction(add_executables)

add_executables()
