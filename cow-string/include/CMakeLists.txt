add_library(CowString INTERFACE)

target_include_directories(
    CowString
    INTERFACE FILE_SET
            CowStringHeaders
            TYPE
            HEADERS
            BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}
            )

target_compile_features(CowString INTERFACE cxx_std_20)