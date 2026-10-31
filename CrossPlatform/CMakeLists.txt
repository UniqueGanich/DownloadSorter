cmake_minimum_required(VERSION 3.20)
project(DownloadsSorter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES 
    src/main.cpp 
    src/Hasher.cpp 
    src/Platform.cpp
    src/Data.cpp
    src/Mover.cpp
    src/Scanner.cpp
    src/Deduplicator.cpp
    src/Sorter.cpp
    src/Console.cpp
    src/Localization.cpp
    src/Config.cpp
    src/Menu.cpp
)

if(WIN32)
    message(STATUS "Building for Windows")
    add_executable(DownloadsSorter ${SOURCES})

    target_compile_options(DownloadsSorter PRIVATE /utf-8)
    target_link_libraries(DownloadsSorter PRIVATE bcrypt shell32 ole32)
else()
    message(STATUS "Building for Linux")
    find_package(OpenSSL REQUIRED)
    find_package(Threads REQUIRED)
    add_executable(DownloadsSorter ${SOURCES})
    target_link_libraries(DownloadsSorter PRIVATE OpenSSL::Crypto Threads::Threads)
endif()