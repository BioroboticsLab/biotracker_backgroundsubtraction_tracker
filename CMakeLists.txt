cmake_minimum_required(VERSION 3.12)

exec_program("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "describe --tags --abbrev=0" OUTPUT_VARIABLE GIT_TAG)

project(BT_Backgroundsubtraction.tracker)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    option(PACKAGE_WINDOWS_MSI "Create .msi package" OFF)
endif()
option(PACKAGE_TXZ "Create .tar.xz package" OFF)

find_package(BT_Utility 0.1.1 REQUIRED)

add_subdirectory(Src)

set(CPACK_PACKAGE_NAME "BioTracker Plugin BackgroundSubtraction")
set(CPACK_PACKAGE_VENDOR "Biorobotics Lab / FU Berlin")

set(CPACK_PACKAGE_VERSION ${GIT_TAG})

if(PACKAGE_WINDOWS_MSI)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "f9c5c38a-76c3-49a6-ab41-ddfdde7a2c87")

    set(CPACK_PACKAGE_INSTALL_DIRECTORY "RoboFish/BioTracker/Plugins")

    set(CPACK_WIX_PROGRAM_MENU_FOLDER "RoboFish")
elseif(PACKAGE_TXZ)
    set(CPACK_GENERATOR "TXZ")
endif()

include(CPack)
