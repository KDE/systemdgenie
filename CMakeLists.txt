project(SYSTEMD-GENIE)
set(PROJECT_VERSION "0.99.0")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")
set(KF5_MIN_VERSION "5.15.0")

# Silence a warning
cmake_policy(SET CMP0063 NEW)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             DBus
             Gui
             Widgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
             Auth
             CoreAddons
             Crash
             I18n
             XmlGui)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(CheckIncludeFiles)

add_definitions(-DQT_NO_CAST_FROM_ASCII
                -DQT_NO_URL_CAST_FROM_STRING)

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION}
                  VARIABLE_PREFIX SYSTEMDGENIE
                  VERSION_HEADER "systemdgenie_version.h")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD "systemd>=209" REQUIRED)
pkg_check_modules(JOURNALD "libsystemd>=209" REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

# Adding local CMake modules
set (
   CMAKE_MODULE_PATH
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
   ${CMAKE_MODULE_PATH}
   )

add_definitions(-DTRANSLATION_DOMAIN=\"systemdgenie\")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

add_subdirectory(src)

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
