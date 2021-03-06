SET(LFA_LOGDATA_MOC_CLASSES
  LogEntryParser_Logfile.h
  LogEntryParser_LogfileLWI.h
  NewLogEntryMessage.h
  ObjectCache.hxx
  ParserStreamGetter.h
)

SET(LFA_LOGDATA_LIB_SOURCES
  EntryToTextFormaterDefault.cpp
  LogDataTypeRegistration.cpp
  LogEntry.cpp
  LogEntryAttributeNames.cpp
  LogEntryFactory.cpp
  LogEntryParser_Logfile.cpp
  LogEntryParser_LogfileLWI.cpp
  LogEntryParserModelConfiguration.cpp
  NewLogEntryMessage.cpp
  ParserStreamGetter.cpp
  ParserStreamGetterFile.cpp
)

IF(USEQT5)
  QT5_WRAP_CPP(LFA_LOGDATA_MOC_SOURCES ${LFA_LOGDATA_MOC_CLASSES})
ELSE()
  QT4_WRAP_CPP(LFA_LOGDATA_MOC_SOURCES ${LFA_LOGDATA_MOC_CLASSES})
ENDIF()

ADD_LIBRARY(${LFA_LOGDATA_LIB} STATIC ${LFA_LOGDATA_LIB_SOURCES} ${LFA_LOGDATA_MOC_SOURCES} )

TARGET_LINK_LIBRARIES(${LFA_LOGDATA_LIB} 
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Network_LIBRARIES}
)

################################################################################
# TEST
IF(NOT WITHOUT_TEST)
  FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
  
  SET(TEST_LFA_LOGDATA_MOC_CLASSES
    test/LogEntryStorerForTesting.h
  )
  
  SET(TEST_LFA_LOGDATA_LIB_SOURCES
    test/test_init.cpp
    test/LogEntryStorerForTesting.cpp
    
    test/test_LogEntryParser_Logfile.cpp
  )
  
  IF(USEQT5)
    QT5_WRAP_CPP(TEST_LFA_LOGDATA_MOC_SOURCES ${TEST_LFA_LOGDATA_MOC_CLASSES})
  ELSE()
    QT4_WRAP_CPP(TEST_LFA_LOGDATA_MOC_SOURCES ${TEST_LFA_LOGDATA_MOC_CLASSES})
  ENDIF()
  
  ADD_EXECUTABLE( testLogData ${TEST_LFA_LOGDATA_LIB_SOURCES} ${TEST_LFA_LOGDATA_MOC_SOURCES} )
  
  TARGET_LINK_LIBRARIES(testLogData 
      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
      ${Qt5Core_LIBRARIES}
      ${LFA_LOGDATA_LIB}
  )
  
  ADD_TEST(NAME testLogData COMMAND ${CMAKE_CURRENT_BINARY_DIR}/testLogData)
ENDIF()
