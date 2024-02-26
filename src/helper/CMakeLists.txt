# Build & Link
add_executable(systemdgeniehelper helper.cpp)
target_link_libraries(systemdgeniehelper Qt::DBus KF${QT_MAJOR_VERSION}::AuthCore)

# Install
install(TARGETS systemdgeniehelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(systemdgeniehelper org.kde.kcontrol.systemdgenie root)
kauth_install_actions(org.kde.kcontrol.systemdgenie systemdgenie.actions)
