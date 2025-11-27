MOC_DIR = ./moc
OBJECTS_DIR = ./obj

INCLUDEPATH += $$PWD

HEADERS       = glwidget.h \
                maillage.h \
                meshdialog.h \
                window.h \
                mainwindow.h \
                SimplexNoise.h
SOURCES       = glwidget.cpp \
                SimplexNoise.cpp \
                maillage.cpp \
                main.cpp \
                meshdialog.cpp \
                window.cpp \
                mainwindow.cpp

RESOURCES += \
    resources.qrc \
    textures.qrc \
    shaders.qrc

QT           += widgets


