#################################################################################################
## File created by QxEntityEditor 1.2.5 (2020/07/07 22:14) : please, do NOT modify this file ! ##
#################################################################################################

include(C:/Qt/QxOrm/QxOrm.pri)

TEMPLATE = lib
CONFIG += dll
DEFINES += _BUILDING_LICENTA
INCLUDEPATH += C:/Qt/QxOrm/include
DESTDIR = $$PWD/bin/

CONFIG(debug, debug|release) {
TARGET = Licentad
} else {
TARGET = Licenta
} # CONFIG(debug, debug|release)

LIBS += -L"C:/Qt/QxOrm/lib"

CONFIG(debug, debug|release) {
LIBS += -l"QxOrmd"
} else {
LIBS += -l"QxOrm"
} # CONFIG(debug, debug|release)

include($$PWD/Licenta.gen.pri)

!contains(DEFINES, _QX_UNITY_BUILD) {
SOURCES += $$PWD/src/Licenta_main.gen.cpp
} # !contains(DEFINES, _QX_UNITY_BUILD)
