add_library(
    client
    application.cpp
    client_manager.cpp
)

target_link_libraries(client common)