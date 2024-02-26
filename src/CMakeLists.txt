add_subdirectory(helper)

set(systemdgenie_SRCS main.cpp
                      mainwindow.cpp
                      unitmodel.cpp
                      sortfilterunitmodel.cpp)

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})
target_link_libraries(systemdgenie
                      KF${QT_MAJOR_VERSION}::AuthCore
                      KF${QT_MAJOR_VERSION}::CoreAddons
                      KF${QT_MAJOR_VERSION}::Crash
                      KF${QT_MAJOR_VERSION}::I18n
                      KF${QT_MAJOR_VERSION}::XmlGui
                      PkgConfig::JOURNALD)
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${KDE_INSTALL_APPDIR})
