cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(MrazMag)

set(SOURCES
	catch.hpp
	interface.h
	implementation.cpp	
)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} test.cpp)
