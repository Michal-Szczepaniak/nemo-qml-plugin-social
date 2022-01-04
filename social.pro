TEMPLATE = subdirs
SUBDIRS = src tests
tests.depends = src

OTHER_FILES += \
    rpm/nemo-qml-plugin-social.yaml \
    rpm/nemo-qml-plugin-social.spec \
    rpm/nemo-qml-plugin-social-qt5.yaml \
    rpm/nemo-qml-plugin-social-qt5.spec \
    doc/social.qdoc \
    doc/caching.qdoc \
    doc/social.qdocconf

include(doc/doc.pri)
