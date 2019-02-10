QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_containersandalgorithms.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ContainersAndAlgorithms/release/ -lContainersAndAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ContainersAndAlgorithms/debug/ -lContainersAndAlgorithms
else:unix: LIBS += -L$$OUT_PWD/../../ContainersAndAlgorithms/ -lContainersAndAlgorithms

INCLUDEPATH += $$PWD/../../ContainersAndAlgorithms
DEPENDPATH += $$PWD/../../ContainersAndAlgorithms
