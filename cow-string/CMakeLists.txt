cmake_minimum_required(VERSION 3.24)

project(
    cow-string
    VERSION 0.0.1
    DESCRIPTION "DIY implementation of COW-string"
    # TODO: add HOMEPAGE_URL
)

add_subdirectory(include)

add_executable(CowStringTest)
target_sources(CowStringTest PRIVATE main.cc)
target_link_libraries(CowStringTest PUBLIC CowString)