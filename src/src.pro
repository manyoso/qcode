HEADERS       = qcode.h \
                mainwindow.h \
                project.h \
                newproject.h \
                importproject.h \
                settings.h \
                configpage.h \
                projectconfig.h \
                messages.h \
                makesyntax.h \
                quickselection.h \
                dirsearchmodel.h \
                openedfilesmodel.h \
                faderwidget.h \
                fileview.h \
                filestack.h \
                highlighter.h

SOURCES       = main.cpp \
                qcode.cpp \
                mainwindow.cpp \
                project.cpp \
                newproject.cpp \
                importproject.cpp \
                settings.cpp \
                configpage.cpp \
                projectconfig.cpp \
                messages.cpp \
                makesyntax.cpp \
                quickselection.cpp \
                dirsearchmodel.cpp \
                openedfilesmodel.cpp \
                faderwidget.cpp \
                fileview.cpp \
                filestack.cpp \
                highlighter.cpp

FORMS         = ../forms/newproject.ui \
                ../forms/importproject.ui \
                ../forms/settings.ui \
                ../forms/configpage.ui \
                ../forms/projectconfig.ui

INCLUDEPATH   += ../widgets
LIBS          += -L../widgets -lqcodewidgets

RESOURCES     = src.qrc
TEMPLATE      = app
TARGET        = qcode

QMAKE_RPATHDIR += /home/manyoso/dev/build/home/manyoso/dev/qcode/widgets
