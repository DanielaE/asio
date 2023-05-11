cmake_minimum_required(VERSION 3.26)

project(asio
        DESCRIPTION "Modularized Asio for C++20"
        HOMEPAGE_URL "https://github.com/DanielaE/asio/tree/module"
        LANGUAGES CXX
)

add_subdirectory(asio)

if (PROJECT_IS_TOP_LEVEL)
    add_library(asio-install INTERFACE)

    install(TARGETS asio-install)

    install(DIRECTORY
        asio/include/asio
        DESTINATION include
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.ipp"
    )

    install(FILES
        asio/include/asio.hpp
        DESTINATION include/
    )

    install(FILES
        asio/module/asio.ixx asio/module/asio-gmf.h
        DESTINATION module/
    )

    install(FILES
        asio/CMakeLists.txt
        DESTINATION .
    )
endif()