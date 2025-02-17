add_subdirectory(helper)

set_source_files_properties(dbus/org.freedesktop.systemd1.Manager.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "systemdunit.h"
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.systemd1.Manager.xml
    systemd_manager_interface
)

set(systemdgenie_SRCS main.cpp
                      mainwindow.cpp
                      unitmodel.cpp
                      unitsfetchjob.cpp
                      systemdunit.cpp
                      sortfilterunitmodel.cpp ${systemd_DBUS_SRCS})

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})
target_link_libraries(systemdgenie
                      KF${QT_MAJOR_VERSION}::AuthCore
                      KF${QT_MAJOR_VERSION}::CoreAddons
                      KF${QT_MAJOR_VERSION}::Crash
                      KF${QT_MAJOR_VERSION}::I18n
                      KF${QT_MAJOR_VERSION}::XmlGui
                      KF${QT_MAJOR_VERSION}::TextEditor
                      PkgConfig::JOURNALD)
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${KDE_INSTALL_APPDIR})
