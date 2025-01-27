project(SYSTEMD-GENIE)
set(PROJECT_VERSION "0.100.0")

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Silence a warning
cmake_policy(SET CMP0063 NEW)

find_package(ECM 5.240 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(CheckIncludeFiles)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             DBus
             Gui
             Widgets)

find_package(KF${QT_MAJOR_VERSION} REQUIRED COMPONENTS
             Auth
             CoreAddons
             Crash
             I18n
             XmlGui
             TextEditor)

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION}
                  VARIABLE_PREFIX SYSTEMDGENIE
                  VERSION_HEADER "systemdgenie_version.h")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD "systemd>=209" REQUIRED IMPORTED_TARGET)
pkg_check_modules(JOURNALD "libsystemd>=209" REQUIRED IMPORTED_TARGET)

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
