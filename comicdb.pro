TEMPLATE      = app
TARGET        = comicdb
INCLUDEPATH  += .
QT           += widgets sql
# Input
SOURCES      += main.cpp mainform.cpp \
    settings.cpp
FORMS        +=          mainform.ui
HEADERS      +=          mainform.h \
    settings.h
RESOURCES    += comicdb.qrc
TRANSLATIONS += comicdb_en.ts

DISTFILES    += comic.dump aufgaben.txt
