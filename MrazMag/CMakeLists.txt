cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(tests)

set(SOURCES
	catch.hpp

	interface.h
	tests.cpp

	implementation.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} "main.cpp")
