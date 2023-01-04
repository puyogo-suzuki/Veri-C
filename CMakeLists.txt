project( veri-c )
cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(PD ${PROJECT_SOURCE_DIR})
set(SOURCES
	src/parse.c)
	

add_executable(veri-parse
	${SOURCES}
	${PROJECT_SOURCE_DIR}/src/veri-parse-main.c)
include_directories(${PROJECT_SOURCE_DIR}/inc)
