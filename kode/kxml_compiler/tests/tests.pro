######################################################################
# Automatically generated by qmake (2.01a) So Mrz 4 11:30:58 2007
######################################################################

CONFIG += qtestlib
TEMPLATE = app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += . ../../common ../../compat ../.. ..

# Input
HEADERS += parserxsdtest.h
SOURCES += parserxsdtest.cpp

QT += xml

LIBS += -L.. -lkschema
LIBS += -L../../schema -lschema
LIBS += -L../../common -lkxmlcommon

DUMMY_MOCS = parserxsdtest.moc

system( touch $$DUMMY_MOCS )
