TARGET = facebooksocialtest
PREFIX = /opt/tests/nemo-qml-plugins-qt5/social/

QT = core gui qml quick

target.path = $${PREFIX}

SOURCES += main.cpp
OTHER_FILES += $$system(./qt5ify.sh)

DEFINES *= PLUGIN_PATH=\"\\\"\"$${DEFINES_PREFIX}/$$[QT_INSTALL_QML]/$$PLUGIN_IMPORT_PATH\"\\\"\"
DEFINES *= DEPLOYMENT_PATH=\"\\\"\"$${DEFINES_PREFIX}/$${PREFIX}share/\"\\\"\"

qml.files = $${OTHER_FILES}
qml.path = $${PREFIX}/share

INSTALLS += target qml
