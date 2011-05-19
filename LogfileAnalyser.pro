TEMPLATE = app
TARGET = LogfileAnalyser
QT += core \
    gui
HEADERS += Models/LogEntryFilterChain.h \
    Models/LogEntryRemoveFilter.h \
    Models/LogEntryFilter.h \
    EntryToTextFormater_Logfile.h \
    LogData/LogEntryParserModelConfiguration.h \
    Models/LogEntryTableFilter.h \
    EntryToTextFormater.h \
    LogEntryCombinedWidget.h \
    Models/StringCacheTreeItem.h \
    Types.h \
    Models/StringCacheTreeModel.h \
    LogEntryTableWindow.h \
    logfileanalyser.h \
    Models/LogEntryTableModel.h \
    LogData/LogEntryParser_dummy.h \
    LogData/LogEntryParser_Logfile.h \
    Assert.h \
    LogData/LogEntryAttributes.h \
    LogData/LogEntryAttributeFactory.h \
    LogData/LogEntryParser.h \
    LogData/StringCache.h \
    LogData/LogEntryFactory.h \
    LogData/LogEntry.h \
    logfileanalyser.h
SOURCES += Models/LogEntryFilterChain.cpp \
    Models/LogEntryRemoveFilter.cpp \
    Models/LogEntryFilter.cpp \
    EntryToTextFormater_Logfile.cpp \
    LogData/LogEntryParserModelConfiguration.cpp \
    Models/LogEntryTableFilter.cpp \
    LogEntryCombinedWidget.cpp \
    Models/StringCacheTreeItem.cpp \
    Models/StringCacheTreeModel.cpp \
    LogEntryTableWindow.cpp \
    Models/LogEntryTableModel.cpp \
    LogData/LogEntryParser_dummy.cpp \
    LogData/LogEntryParser_Logfile.cpp \
    LogData/LogEntryAttributes.cpp \
    LogData/LogEntryAttributeFactory.cpp \
    LogData/StringCache.cpp \
    LogData/LogEntryFactory.cpp \
    LogData/LogEntry.cpp \
    main.cpp \
    logfileanalyser.cpp
FORMS += logfileanalyser.ui
RESOURCES += 
