# Build & Link
add_executable(systemdgeniehelper helper.cpp helper.h)
target_link_libraries(systemdgeniehelper Qt::DBus KF6::AuthCore)

# Install
install(TARGETS systemdgeniehelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(systemdgeniehelper org.kde.kcontrol.systemdgenie root)
kauth_install_actions(org.kde.kcontrol.systemdgenie systemdgenie.actions)
