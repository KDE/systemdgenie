add_subdirectory(helper)

set(systemdgenie_SRCS main.cpp
                      mainwindow.cpp
                      unitmodel.cpp
                      sortfilterunitmodel.cpp)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/src)

ki18n_wrap_ui(systemdgenie_SRCS mainwindow.ui)

add_executable(systemdgenie ${systemdgenie_SRCS})
target_link_libraries(systemdgenie
                      KF5::Auth
                      KF5::CoreAddons
                      KF5::Crash
                      KF5::I18n
                      KF5::XmlGui
                      ${JOURNALD_LIBRARIES})
install(TARGETS systemdgenie ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES systemdgenieui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/systemdgenie)
install(FILES org.kde.systemdgenie.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
