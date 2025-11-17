MOC_DIR = ./moc
OBJECTS_DIR = ./obj

INCLUDEPATH += $$PWD

HEADERS       = glwidget.h \
                maillage.h \
                window.h \
                mainwindow.h
SOURCES       = glwidget.cpp \
                maillage.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp

RESOURCES += \
    resources.qrc \
    shaders.qrc

QT           += widgets


