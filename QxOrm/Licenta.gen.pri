#################################################################################################
## File created by QxEntityEditor 1.2.5 (2020/07/07 22:14) : please, do NOT modify this file ! ##
#################################################################################################

!contains(DEFINES, _QX_NO_PRECOMPILED_HEADER) {
PRECOMPILED_HEADER += $$PWD/include/Licenta_precompiled_header.gen.h
} # !contains(DEFINES, _QX_NO_PRECOMPILED_HEADER)

HEADERS += $$PWD/include/Licenta_precompiled_header.gen.h
HEADERS += $$PWD/include/Licenta_export.gen.h

HEADERS += $$PWD/include/Camera.gen.h
HEADERS += $$PWD/include/Violation.gen.h

HEADERS += $$PWD/custom/include/Camera.h
HEADERS += $$PWD/custom/include/Violation.h

contains(DEFINES, _QX_UNITY_BUILD) {

SOURCES += $$PWD/src/Licenta_all.gen.cpp

} else {

SOURCES += $$PWD/src/Camera.gen.cpp
SOURCES += $$PWD/src/Violation.gen.cpp

SOURCES += $$PWD/custom/src/Camera.cpp
SOURCES += $$PWD/custom/src/Violation.cpp

} # contains(DEFINES, _QX_UNITY_BUILD)

include($$PWD/custom/Licenta.pri)
