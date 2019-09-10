add_library(
    client
    application.cpp
    client_manager.cpp
    input/keyboard.cpp
)

target_link_libraries(client common)