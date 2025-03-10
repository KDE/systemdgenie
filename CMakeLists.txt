project(SYSTEMD-GENIE)
set(PROJECT_VERSION "0.100.0")

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Silence a warning
cmake_policy(SET CMP0063 NEW)

find_package(ECM 6.9.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)
include(CheckIncludeFiles)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)

include(ECMQmlModule)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    DBus
    Gui
    Widgets
    Quick
)

find_package(KF6 REQUIRED COMPONENTS
    Auth
    CoreAddons
    Crash
    I18n
    TextEditor
    KIO
)

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

ecm_set_disabled_deprecation_versions(
    QT 6.9.0
    KF 6.12.0
)

add_subdirectory(src)

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
