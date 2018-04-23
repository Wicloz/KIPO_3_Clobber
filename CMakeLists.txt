cmake_minimum_required(VERSION 3.9)
project(KIPO_3_Clobber)

set(CMAKE_CXX_STANDARD 11)

add_executable(KIPO_3_Clobber Clobber.cc)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
