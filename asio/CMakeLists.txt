cmake_minimum_required(VERSION 3.26)

if (CMAKE_CXX_STANDARD LESS 20)
  message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif()

project(asio
        DESCRIPTION "Modularized Asio for C++20"
        HOMEPAGE_URL "https://github.com/DanielaE/asio/tree/module"
        LANGUAGES CXX
)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_EXTENSIONS OFF)

set(asio-sources module/asio.ixx)

add_library(asio STATIC)
add_library(asio::asio ALIAS asio)
target_sources(asio
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/module
      FILES ${asio-sources}
)
target_compile_features(asio PUBLIC cxx_std_20)
target_include_directories(asio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MSVC)
  target_compile_options(asio PRIVATE /dxifcInlineFunctions-)
  target_compile_options(asio PRIVATE /wd5244)
endif()
