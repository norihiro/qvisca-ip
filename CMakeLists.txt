cmake_minimum_required(VERSION 3.16)
project(qvisca-ip VERSION 0.1.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_POSITION_INDEPENDENT_CODE true)
set(WITH_VISCA_SERIAL OFF)
add_definitions("-DVISCA_API= ")
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

if(WIN32)
    set_target_properties(qvisca-ip PROPERTIES WIN32_EXECUTABLE ON)
else()
    # Linux and macOS
    install(
	TARGETS qvisca-ip
	DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(APPLE AND NOT "${CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY}" STREQUAL "")
    set(CMAKE_XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--timestamp")
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO)
    set_target_properties(qvisca-ip
	PROPERTIES
	BUNDLE ON
	MACOSX_BUNDLE TRUE
	OUTPUT_NAME qvisca-ip
	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos-bundle.plist.in
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${BUNDLE_ID}"
    )
endif()
