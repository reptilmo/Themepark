cmake_minimum_required(VERSION 3.23)

add_library(glad STATIC
	include/glad/glad.h
	src/glad.c
)

target_include_directories(glad PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
)
