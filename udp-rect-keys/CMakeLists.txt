cmake_minimum_required(VERSION 3.10)

#Set up project
project(udp-rects VERSION 1.0)

set(SOURCES
	src/main.cpp
	src/client/application.cpp
	src/client/client_manager.cpp
	src/client/input/keyboard.cpp

	src/server/server.cpp

	src/common/commands.cpp
    src/common/net_helper.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib)

#Set C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
  	target_compile_options(${PROJECT_NAME} PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic -Werror)		#Warning flags
endif()

#Set module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules" ${CMAKE_MODULE_PATH})

#Find libraries
find_package(Threads)
find_package(SFML REQUIRED network graphics window system)

#Finally
target_link_libraries(${PROJECT_NAME} 
	net
    Threads::Threads 
    ${SFML_LIBRARIES} 
    ${SFML_DEPENDENCIES}
)
