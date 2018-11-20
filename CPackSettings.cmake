set(CPACK_PACKAGE_NAME "mcpelauncher-ui-qt")
set(CPACK_PACKAGE_VENDOR "mcpelauncher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Metalauncher for mcpelauncher (allows you to download Minecraft and start the actual launcher)")
set(CPACK_PACKAGE_CONTACT "https://github.com/minecraft-linux/mcpelauncher-ui-manifest/issues")
set(CPACK_PACKAGE_VERSION "${MANIFEST_GIT_COMMIT_HASH}")
set(CPACK_GENERATOR "TGZ;DEB")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};mcpelauncher-ui-qt;mcpelauncher-ui-qt;/")
set(CPACK_OUTPUT_CONFIG_FILE CPackConfig.cmake)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>=2.14), libssl1.1, libuv1, libzip4, libqt5widgets5, libqt5webenginewidgets5, libqt5quick5, libqt5svg5, libqt5quickcontrols2-5, libqt5quicktemplates2-5, libqt5concurrent5, libprotobuf10, qml-module-qtquick2, qml-module-qtquick-layouts, qml-module-qtquick-controls, qml-module-qtquick-controls2, qml-module-qtquick-window2, qml-module-qtquick-dialogs, qml-module-qt-labs-settings, qml-module-qt-labs-folderlistmodel")
set(CPACK_DEBIAN_PACKAGE_VERSION "${BUILD_TIMESTAMP}-${MANIFEST_GIT_COMMIT_HASH}")

include(CPack)
