cmake_minimum_required(VERSION 3.20)
project(YPP VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ypp_frontend
    src/lexer.cpp
    src/parser.cpp
)
target_include_directories(ypp_frontend PUBLIC include)
target_compile_features(ypp_frontend PUBLIC cxx_std_20)

add_executable(ypp src/main.cpp)
target_link_libraries(ypp PRIVATE ypp_frontend)

if(MSVC)
  target_compile_options(ypp_frontend PRIVATE /W4 /permissive-)
  target_compile_options(ypp PRIVATE /W4 /permissive-)
else()
  target_compile_options(ypp_frontend PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(ypp PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS ypp RUNTIME DESTINATION bin)
