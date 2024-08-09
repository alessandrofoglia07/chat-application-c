cmake_minimum_required(VERSION 3.22)
project(chat_app C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

set(SERVER_RESOURCES
    src/server.c
    src/utils.c
)

set(CLIENT_RESOURCES
    src/client.c
    src/utils.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(server ${SERVER_RESOURCES})
add_executable(client ${CLIENT_RESOURCES})
