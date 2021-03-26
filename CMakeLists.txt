cmake_minimum_required(VERSION 3.5)

project(finalQT LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Widgets Core Quick Sql REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})


if (ANDROID)
    add_library(finalQT SHARED
            main.cpp
            widget.cpp
            widget.h
            widget.ui
            )
else ()
    add_executable(finalQT
            main.cpp
            widget.cpp
            widget.h
            widget.ui
            number.h
            number.cpp
            hmac.h
            hmac.cpp
            login.h
            login.cpp
            login.ui
            dec_2_bin.py
            bin_2_dec.py
            attack.py
            )
endif ()


target_link_libraries(finalQT PRIVATE Qt5::Widgets Qt5::Core Qt5::Quick Qt5::Sql OpenSSL::SSL ${PYTHON_LIBRARIES})
