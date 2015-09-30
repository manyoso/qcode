HEADERS       = pathselector.h \
                buildsource.h \
                qcodewidgets.h

SOURCES       = pathselector.cpp \
                buildsource.cpp \
                qcodewidgets.cpp

FORMS         = ../forms/pathselector.ui \
                ../forms/buildsource.ui

CONFIG        += designer plugin
TEMPLATE      = lib
TARGET        = qcodewidgets

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
