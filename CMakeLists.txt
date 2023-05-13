cmake_minimum_required(VERSION 3.16)
project(qvisca-ip VERSION 0.1.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(libvisca-ip/visca)

find_package(Qt6 COMPONENTS Core Widgets)

add_executable(qvisca-ip
	main.cpp
	camerawindow.cpp
        camerawindow-thread.cpp
)

target_link_libraries(qvisca-ip
	PRIVATE
	visca
	Qt6::Core
	Qt6::Widgets
)
target_include_directories(qvisca-ip
	PRIVATE
	libvisca-ip/
)
