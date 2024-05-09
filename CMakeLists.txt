cmake_minimum_required(VERSION 3.22)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/quest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/kaleb.cmake)
project(beatsavervoting VERSION ${PACKAGE_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB_RECURSE c_files ${SRC_DIR}/*.c)
file(GLOB_RECURSE cpp_files ${SRC_DIR}/*.cpp)

add_library(
    beatsavervoting
    SHARED
    ${c_files}
    ${cpp_files}
)

file(GLOB so_files ${EXTERN_DIR}/libs/lib*.so)
file(GLOB a_files ${EXTERN_DIR}/libs/lib*.a)

add_assets(assets_beatsavervoting STATIC ${CMAKE_CURRENT_LIST_DIR}/assets ${INCLUDE_DIR}/assets.hpp)

target_link_libraries(
    beatsavervoting
    PRIVATE
    log
    z
    assets_beatsavervoting
    ${so_files}
    ${a_files}
)

target_include_directories(beatsavervoting PRIVATE ${INCLUDE_DIR})
target_include_directories(beatsavervoting PRIVATE ${SHARED_DIR})
target_include_directories(beatsavervoting PRIVATE ${EXTERN_DIR}/includes)
target_include_directories(beatsavervoting SYSTEM PRIVATE ${EXTERN_DIR}/includes/paper/shared/utfcpp/source)
target_include_directories(beatsavervoting SYSTEM PRIVATE ${EXTERN_DIR}/includes/fmt/fmt/include/)
target_include_directories(beatsavervoting SYSTEM PRIVATE ${EXTERN_DIR}/includes/beatsaber-hook/shared/rapidjson/include/)
target_include_directories(beatsavervoting SYSTEM PRIVATE ${EXTERN_DIR}/includes/libil2cpp/il2cpp/libil2cpp)
target_include_directories(beatsavervoting SYSTEM PRIVATE ${EXTERN_DIR}/includes/bs-cordl/include)
target_compile_options(beatsavervoting PUBLIC -fdeclspec -Wno-invalid-offsetof)
target_compile_definitions(beatsavervoting PRIVATE FMT_HEADER_ONLY)
target_compile_definitions(beatsavervoting PRIVATE MOD_ID="${PACKAGE_NAME}")
target_compile_definitions(beatsavervoting PRIVATE VERSION="${PACKAGE_VERSION}")
target_compile_definitions(beatsavervoting PRIVATE UNITY_2021)
target_compile_definitions(beatsavervoting PRIVATE KALEB_VERSION="${KALEB_VERSION}")

add_custom_command(
    TARGET beatsavervoting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_BINARY_DIR}/debug
    COMMENT "Ensure debug dir exists"
)

add_custom_command(
    TARGET beatsavervoting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_CURRENT_BINARY_DIR}/libbeatsaverplusplus.so ${CMAKE_CURRENT_BINARY_DIR}/debug/libbeatsaverplusplus.so
    COMMENT "move normal binary into debug folder"
)

add_custom_command(
    TARGET beatsavervoting POST_BUILD
    COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
    ${CMAKE_CURRENT_BINARY_DIR}/debug/libbeatsaverplusplus.so -o ${CMAKE_CURRENT_BINARY_DIR}/libbeatsaverplusplus.so
    COMMENT "Strip binary"
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/debug)
foreach(bin_file IN LISTS so_list a_list)
    cmake_path(GET bin_file FILENAME file_name)

    add_custom_command(
        TARGET beatsavervoting POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${bin_file} ${CMAKE_CURRENT_BINARY_DIR}/debug/${file_name}
        COMMENT "Copy so files for ndk stack"
    )

    add_custom_command(
        TARGET beatsavervoting POST_BUILD
        COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
        ${bin_file} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
        COMMENT "Strip debug symbols from the dependencies"
    )
endforeach()
