add_subdirectory(helper)

set(systemdgenie_SRCS main.cpp
                      mainwindow.cpp
                      unitmodel.cpp
                      sortfilterunitmodel.cpp)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/src)

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})
target_link_libraries(systemdgenie
                      KF5::AuthCore
                      KF5::CoreAddons
                      KF5::Crash
                      KF5::I18n
                      KF5::XmlGui
                      PkgConfig::JOURNALD)
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${KDE_INSTALL_APPDIR})
