cmake_minimum_required(VERSION 3.28)

project(
        etherblocks
        VERSION 0.1.0
        DESCRIPTION "etherblocks"
        LANGUAGES CXX)

add_subdirectory(src)

add_subdirectory(app)