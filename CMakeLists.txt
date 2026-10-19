cmake_minimum_required(VERSION 3.23)
project(cpp-lox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cpp-lox
    src/error_reporter.cpp
    src/interpreter.cpp
    src/loxclass.cpp
    src/loxfunction.cpp
    src/loxinstance.cpp
    src/main.cpp
    src/parser.cpp
    src/resolver.cpp
    src/scanner.cpp
    )

target_include_directories(cpp-lox
    PUBLIC
    include

    )

