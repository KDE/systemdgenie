add_subdirectory(helper)

set_source_files_properties(dbus/org.freedesktop.systemd1.Manager.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "systemdunit.h"
)

set_source_files_properties(dbus/org.freedesktop.login1.Manager.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "systemdunit.h"
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.systemd1.Manager.xml
    systemd_manager_interface
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.login1.Manager.xml
    login_manager_interface
)

set(systemdgenie_SRCS
    main.cpp
    mainwindow.cpp
    unitmodel.cpp
    unitsfetchjob.cpp
    unitsfetchjob.h
    sessionsfetchjob.cpp
    sessionsfetchjob.h
    sortfilterunitmodel.cpp
    systemdunit.cpp
    configfilemodel.cpp
    configfilemodel.h
    ${systemd_DBUS_SRCS}
)

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})
target_link_libraries(systemdgenie
                      KF6::AuthCore
                      KF6::CoreAddons
                      KF6::Crash
                      KF6::I18n
                      KF6::XmlGui
                      KF6::TextEditor
                      KF6::KIOGui
                      PkgConfig::JOURNALD)
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${KDE_INSTALL_APPDIR})
