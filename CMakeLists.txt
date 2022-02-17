cmake_minimum_required(VERSION 3.0)
project(IPA)

file(GLOB src_files
     "IPA/*.h"
     "IPA/*.cpp"
)

add_executable(IPA ${src_files})
