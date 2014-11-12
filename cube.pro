QT       += core gui widgets

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    playground.cpp \
    glann.cpp

qtHaveModule(opengl) {
    QT += opengl

    SOURCES +=

    HEADERS +=

    RESOURCES += \
        shaders.qrc
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target

HEADERS += \
    playground.h \
    glann.h
