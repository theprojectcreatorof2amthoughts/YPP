cmake_minimum_required(VERSION 3.20)
project(YPP VERSION 0.2.0 LANGUAGES C CXX)
include(GNUInstallDirs)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
file(GLOB_RECURSE HLYPP_LIBRARIES CONFIGURE_DEPENDS LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/std/*.hlypp")
list(SORT HLYPP_LIBRARIES)
set(GENERATED_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(EMBEDDED_SOURCE "${GENERATED_DIRECTORY}/embedded_libraries.c")
add_executable(ypp_hlypp_embedder EXCLUDE_FROM_ALL tools/embed_hlypp.c)
target_compile_features(ypp_hlypp_embedder PRIVATE c_std_11)
add_custom_command(
    OUTPUT "${EMBEDDED_SOURCE}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${GENERATED_DIRECTORY}"
    COMMAND "$<TARGET_FILE:ypp_hlypp_embedder>" "${EMBEDDED_SOURCE}" ${HLYPP_LIBRARIES}
    DEPENDS ypp_hlypp_embedder ${HLYPP_LIBRARIES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    VERBATIM
    COMMENT "Embedding YPP .hlypp header libraries")
set_source_files_properties("${EMBEDDED_SOURCE}" PROPERTIES GENERATED TRUE)
add_library(ypp_runtime STATIC src/runtime.c src/memory.c src/file_io.c "${EMBEDDED_SOURCE}")
target_include_directories(ypp_runtime PUBLIC src)
if(NOT WIN32)
    target_link_libraries(ypp_runtime PUBLIC m)
endif()
add_library(ypp_frontend STATIC src/lexer.cpp src/parser.cpp)
target_include_directories(ypp_frontend PUBLIC include)
target_compile_features(ypp_frontend PUBLIC cxx_std_20)
add_executable(ypp src/main.cpp)
target_include_directories(ypp PRIVATE include src)
target_link_libraries(ypp PRIVATE ypp_frontend ypp_runtime)
if(MSVC)
    target_compile_options(ypp_runtime PRIVATE /W4)
    target_compile_options(ypp_frontend PRIVATE /W4 /permissive-)
    target_compile_options(ypp PRIVATE /W4 /permissive-)
else()
    target_compile_options(ypp_runtime PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(ypp_frontend PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(ypp PRIVATE -Wall -Wextra -Wpedantic)
endif()
install(TARGETS ypp RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
