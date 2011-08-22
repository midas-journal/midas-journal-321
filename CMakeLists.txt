CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

PROJECT(CELLSEGMENTATION)

ADD_LIBRARY( Option ${CELLSEGMENTATION_SOURCE_DIR}/Options/anyoption.cxx )

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


SET( MY_DATAROOT ${CELLSEGMENTATION_SOURCE_DIR}/test/Data )
FIND_PATH( MY_DATAROOT zebrafish2D.png
  ${CELLSEGMENTATION_SOURCE_DIR}/test/Data
  $ENV{MY_DATAROOT} )

SET( DIRECTORIES
  Options
  3DCellSegmentation
  )

SET( LABELMAP_DIR
     ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/binaryAttributeMorphology
     PATH)
INCLUDE_DIRECTORIES( ${LABELMAP_DIR} )
OPTION( TEST_LABEL_FRAMEWORK
  "Do you want to test the Label framework code? (100+ test)..." OFF )

SUBDIRS( Utilities )

SUBDIRS( ${DIRECTORIES} )

SUBDIRS( examples )
