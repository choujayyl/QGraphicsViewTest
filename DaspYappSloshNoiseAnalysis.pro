QT += charts

HEADERS += \
    callout.h \
    view.h \
    operatorandlist.h

SOURCES += \
    callout.cpp \
    main.cpp\
    view.cpp \
    operatorandlist.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/charts/callout
INSTALLS += target

DISTFILES +=

RESOURCES +=

VERSION = 1.0.0.3

