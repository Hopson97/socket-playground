# Include directories for the libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# libnet, networking
add_subdirectory(net)