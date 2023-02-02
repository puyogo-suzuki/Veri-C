project( veri-c )
cmake_minimum_required(VERSION 3.22)
set(default_build_type "Debug")
set(CMAKE_C_STANDARD 11)
set(PD ${PROJECT_SOURCE_DIR})
set(SOURCES
        src/ast.c
	src/parse.c
	src/alpha.c
	src/check.c
      src/get_type.c)
	

add_executable(veri-parse
	${SOURCES}
	${PROJECT_SOURCE_DIR}/src/veri-parse-main.c)
	
add_executable(veri-alpha
	${SOURCES}
	${PROJECT_SOURCE_DIR}/src/veri-alpha-main.c)

add_executable(veri-check
	${SOURCES}
	${PROJECT_SOURCE_DIR}/src/veri-check-main.c)

include_directories(${PROJECT_SOURCE_DIR}/inc)
