add_subdirectory(helper)

set_source_files_properties(dbus/org.freedesktop.systemd1.Manager.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "systemdunit.h"
)

set_source_files_properties(dbus/org.freedesktop.login1.Manager.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "loginddbustypes.h"
)

set_source_files_properties(dbus/org.freedesktop.login1.Session.xml PROPERTIES
    NO_NAMESPACE TRUE
    INCLUDE "loginddbustypes.h"
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.systemd1.Manager.xml
    systemd_manager_interface
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.login1.Manager.xml
    login_manager_interface
)

qt_add_dbus_interface(systemd_DBUS_SRCS
    dbus/org.freedesktop.login1.Session.xml
    login_session_interface
)

set(systemdgenie_SRCS
    main.cpp
    mainwindow.cpp
    unitmodel.cpp
    sortfilterunitmodel.cpp
    systemdunit.cpp
    configfilemodel.cpp
    configfilemodel.h
    sessionmodel.cpp
    sessionmodel.h
    types.cpp
    types.h
    editor.cpp
    editor.h
    timermodel.cpp
    timermodel.h

    job/sessionsfetchjob.cpp
    job/sessionsfetchjob.h
    job/unitsfetchjob.cpp
    job/unitsfetchjob.h

    systemd_unit_interface.cpp
    systemd_unit_interface.h
    systemd_timer_interface.cpp
    systemd_timer_interface.h

    ${systemd_DBUS_SRCS}
)

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})

ecm_add_qml_module(systemdgenie
    GENERATE_PLUGIN_SOURCE
    URI org.kde.systemdgenie
)

qt_target_qml_sources(systemdgenie
    QML_FILES
        qml/Main.qml
        qml/ConfigFilesPage.qml
        qml/TableDelegate.qml
        qml/TablePage.qml
        qml/UnitsPage.qml
        qml/UnitPage.qml
        qml/SessionsPage.qml
        qml/TimersPage.qml
)

target_link_libraries(systemdgenie PRIVATE
    Qt::Quick
    Qt::Qml
    KF6::AuthCore
    KF6::CoreAddons
    KF6::Crash
    KF6::I18n
    KF6::I18nQml
    KF6::XmlGui
    KF6::TextEditor
    KF6::KIOGui
    PkgConfig::JOURNALD
)
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${KDE_INSTALL_APPDIR})
