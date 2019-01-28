#-------------------------------------------------
#
# Project created by QtCreator 2016-08-27T18:18:33
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Acrifil
TEMPLATE = app

INCLUDEPATH += $(STAR_TSP_INCLUDE)
LIBS += "$(STAR_TSP_LIB)/StarIOPort.lib"

SOURCES += main.cpp\
    Helpers/errorhelper.cpp \
	Helpers/appdatadirhelper.cpp \
    Views/mainwindow.cpp \
	Views/newentrydialog.cpp \
    Views/manageclientsdialog.cpp \
    Views/manageusersdialog.cpp \
    Views/selectclientdialog.cpp \
    Helpers/errorhelper.cpp \
    Helpers/databasehelper.cpp \
    Views/selectdatetimedialog.cpp \
    Views/edittissuesdialog.cpp \
    Helpers/receiptprinterhelper.cpp \
    Helpers/datetimehelper.cpp \
    Views/visualizereceiptdialog.cpp \
    Views/printreportdialog.cpp \
    Views/logindialog.cpp \
    Helpers/startspprinterhelper.cpp \
    Views/settingsdialog.cpp \
    Views/financialreportdialog.cpp \
    Helpers/reportprinthelper.cpp \
    Helpers/usersessionhelper.cpp \
    Views/managepricesdialog.cpp \
    Views/changesinglepricedialog.cpp

HEADERS  += Exceptions/splashexception.h \
    Helpers/errorhelper.h \
	Helpers/appdatadirhelper.h \
    Views/mainwindow.h \
	Views/newentrydialog.h \
    Views/manageclientsdialog.h \
    Views/manageusersdialog.h \
    Views/selectclientdialog.h \
    Helpers/errorhelper.h \
    Helpers/databasehelper.h \
    Views/selectdatetimedialog.h \
    Views/edittissuesdialog.h \
    Helpers/receiptprinterhelper.h \
    Helpers/datetimehelper.h \
    Views/visualizereceiptdialog.h \
    Views/printreportdialog.h \
    Views/logindialog.h \
    Helpers/startspprinterhelper.h \
    Views/settingsdialog.h \
    Views/financialreportdialog.h \
    Helpers/reportprinthelper.h \
    Helpers/usersessionhelper.h \
    Views/managepricesdialog.h \
    Views/changesinglepricedialog.h

FORMS    += Views/mainwindow.ui \
    Views/newentrydialog.ui \
    Views/manageclientsdialog.ui \
    Views/manageusersdialog.ui \
    Views/selectclientdialog.ui \
    Views/selectdatetimedialog.ui \
    Views/edittissuesdialog.ui \
    Views/visualizereceiptdialog.ui \
    Views/printreportdialog.ui \
    Views/logindialog.ui \
    Views/settingsdialog.ui \
    Views/financialreportdialog.ui \
    Views/managepricesdialog.ui \
    Views/changesinglepricedialog.ui

RESOURCES += \
    Resources/resources.qrc

RC_FILE = app.rc
