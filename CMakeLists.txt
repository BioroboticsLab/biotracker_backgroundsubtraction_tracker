cmake_minimum_required(VERSION 3.15)

exec_program("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "describe --dirty --always --exclude '*'" OUTPUT_VARIABLE SOURCE_VERSION)

project(biotracker-background_subtraction_tracker
    LANGUAGES CXX
)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    option(PACKAGE_MSI "Create MSI package" OFF)
endif()
option(PACKAGE_TXZ "Create .tar.xz package" OFF)

find_package(OpenCV 4 QUIET COMPONENTS bgsegm)
if(NOT OpenCV_FOUND)
    find_package(OpenCV 3 REQUIRED COMPONENTS bgsegm)
endif()
find_package(biotracker-utility REQUIRED)

add_subdirectory(Src)

include(cmake/Locate.cmake)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    option(INSTALL_REQUIRED_SYSTEM_LIBRARIES "Install required system libraries" OFF)
    if(INSTALL_REQUIRED_SYSTEM_LIBRARIES)
        set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
        include(InstallRequiredSystemLibraries)
    endif()

	#-DINSTALL_SHARED_LIBRARIES='boost_filesystem*' 
	#-DINSTALL_OPENCV_COMPONENTS='core;imgproc;video' 
	#qt5core qt5widgets qt5gui
    set(INSTALL_OPENCV_COMPONENTS "" CACHE STRING "OpenCV components to install")
    locate_opencv(shared_libraries COMPONENTS ${INSTALL_OPENCV_COMPONENTS})
	
    set(INSTALL_SHARED_LIBRARIES "" CACHE STRING "Additional shared libraries to install")
    locate_shared_library(shared_libraries ${INSTALL_SHARED_LIBRARIES})

    install(FILES ${shared_libraries} DESTINATION .)
endif()

set(CPACK_PACKAGE_NAME "BioTracker-Tracker-BackgroundSubtraction")
set(CPACK_PACKAGE_VENDOR "Biorobotics Lab / FU Berlin")
set(CPACK_PACKAGE_VERSION ${SOURCE_VERSION})

if(PACKAGE_MSI)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "f9c5c38a-76c3-49a6-ab41-ddfdde7a2c87")
    set(CPACK_WIX_PRODUCT_VERSION "0.0.0")

    set(CPACK_PACKAGE_INSTALL_DIRECTORY "BioTracker/Tracker/BackgroundSubtraction")

    set(CPACK_WIX_PROGRAM_MENU_FOLDER "BioTracker")
	
	set(CPACK_WIX_LICENSE_RTF ${CMAKE_SOURCE_DIR}/cmake/WIX/licence.rtf)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/WIX/WIX.template.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake/WIX/WIX.template")
	set(CPACK_WIX_TEMPLATE "${CMAKE_CURRENT_BINARY_DIR}/cmake/WIX/WIX.template")
elseif(PACKAGE_TXZ)
    set(CPACK_GENERATOR "TXZ")
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
string(APPEND CPACK_PACKAGE_FILE_NAME "-${CPACK_PACKAGE_VERSION}")
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    string(APPEND CPACK_PACKAGE_FILE_NAME "-${CMAKE_SYSTEM_NAME}")
endif()

include(CPack)
