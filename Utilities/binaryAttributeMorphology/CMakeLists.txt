#Change PROJECT_NAME to the name of your project
#PROJECT(binaryAttributeMorphology)

# set the name of the input image used to run the test
SET(BINARY_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/images/2th_cthead1.png)
SET(LABEL_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1-label.png)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
#ENABLE_TESTING()
#INCLUDE(Dart)
FIND_PROGRAM(ITK_TEST_DRIVER itkTestDriver
  PATHS
  ${ITK_DIR}/bin
  NO_DEFAULT_PATH
)
FIND_PROGRAM(ITK_TEST_DRIVER itkTestDriver)
SET(TEST_COMMAND ${ITK_TEST_DRIVER} --add-before-env PATH ${CMAKE_CURRENT_BINARY_DIR})

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
  ITKStatistics
)


# install devel files
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

# SET(CurrentExe "check")
# ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
# TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_shape_opening")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_shape_keep_n_objects")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_shape_opening")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_shape_keep_n_objects")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "shape_relabel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "binary_statistics_opening")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_statistics_keep_n_objects")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_statistics_opening")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_statistics_keep_n_objects")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "statistics_relabel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "mask")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_reconstruction_by_dilation")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_reconstruction_by_erosion")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_closing_by_reconstruction")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_opening_by_reconstruction")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_fillhole")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "binary_grind_peak")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "generic_attribute")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_threads")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "attribute_values")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "print_label_objects")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_statistics_opening_rgb")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_reconstruction_by_dilation")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_to_rgb")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label_overlay")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "change_region")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "reference_region")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "crop")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "pad")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "auto_crop")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "select")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "label")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "simple_generic_attribute")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "attribute_values_test")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

FOREACH(fg 100 200)
  FOREACH(bg 0 150)
    FOREACH(order 0 1)
      FOREACH(connectivity 0 1)
        FOREACH(size 100 10000)
          ADD_TEST(BinarySizeOpening-${fg}-${bg}-${size}-${order}-${connectivity} ${TEST_COMMAND}
            binary_shape_opening
              ${BINARY_IMAGE} binary_size_opening-${fg}-${bg}-${size}-${order}-${connectivity}.png
              ${fg} ${bg} ${size} ${order} ${connectivity} Size
            --compare binary_size_opening-${fg}-${bg}-${size}-${order}-${connectivity}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/binary_size_opening-${fg}-${bg}-${size}-${order}-${connectivity}.png
          )
        ENDFOREACH(size)

        FOREACH(nb 3 10)
          ADD_TEST(BinarySizeKeepNObjects-${fg}-${bg}-${nb}-${order}-${connectivity} ${TEST_COMMAND}
            binary_shape_keep_n_objects
              ${BINARY_IMAGE} binary_size_keep_n_objects-${fg}-${bg}-${nb}-${order}-${connectivity}.png
              ${fg} ${bg} ${nb} ${order} ${connectivity} Size
            --compare binary_size_keep_n_objects-${fg}-${bg}-${nb}-${order}-${connectivity}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/binary_size_keep_n_objects-${fg}-${bg}-${nb}-${order}-${connectivity}.png
          )
        ENDFOREACH(nb)
      ENDFOREACH(connectivity)
    ENDFOREACH(order)
  ENDFOREACH(bg)
ENDFOREACH(fg)

FOREACH(bg 0 176)
  FOREACH(order 0 1)
      FOREACH(size 3000 10000)
        ADD_TEST(LabelSizeOpening-${bg}-${size}-${order} ${TEST_COMMAND}
          label_shape_opening
            ${LABEL_IMAGE} label_size_opening-${bg}-${size}-${order}.png
            ${bg} ${size} ${order} Size
          --compare label_size_opening-${bg}-${size}-${order}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/label_size_opening-${bg}-${size}-${order}.png
        )
      ENDFOREACH(size)

      FOREACH(nb 1 3)
        ADD_TEST(LabelSizeKeepNObjects-${bg}-${nb}-${order} ${TEST_COMMAND}
          label_shape_keep_n_objects
            ${LABEL_IMAGE} label_size_keep_n_objects-${bg}-${nb}-${order}.png
            ${bg} ${nb} ${order} Size
          --compare label_size_keep_n_objects-${bg}-${nb}-${order}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/label_size_keep_n_objects-${bg}-${nb}-${order}.png
        )
      ENDFOREACH(nb)

      ADD_TEST(SizeRelabel-${bg}-${order} ${TEST_COMMAND}
        shape_relabel
          ${LABEL_IMAGE} size_relabel-${bg}-${order}.png
          ${bg} ${order} Size
        --compare size_relabel-${bg}-${order}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/size_relabel-${bg}-${order}.png
      )
  ENDFOREACH(order)
ENDFOREACH(bg)

ADD_TEST(BinaryFilloles ${TEST_COMMAND}
  binary_fillhole
  ${BINARY_IMAGE} binary_fillhole.png
  0 200
  --compare binary_fillhole.png ${CMAKE_CURRENT_SOURCE_DIR}/images/binary_fillhole.png
)


ADD_TEST(BinaryOpeningByReconstruction ${TEST_COMMAND}
  binary_opening_by_reconstruction 
  ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1-bin.png binary_opening_by_reconstruction.png
  0 255 0 10
  --compare binary_opening_by_reconstruction.png ${CMAKE_CURRENT_SOURCE_DIR}/images/binary_opening_by_reconstruction.png
)


ADD_TEST(LabelMapOverlay ${TEST_COMMAND}
  label_overlay
  ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1-label2.png ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png label_overlay.png
  0 0.5
  --compare label_overlay.png ${CMAKE_CURRENT_SOURCE_DIR}/images/label_overlay.png
)


ADD_TEST(LabelMapToRGB ${TEST_COMMAND}
  label_to_rgb
  ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1-label2.png label_to_rgb.png
  0
  --compare label_to_rgb.png ${CMAKE_CURRENT_SOURCE_DIR}/images/label_to_rgb.png
)


FOREACH(label 0 100)
  FOREACH(bg 0 1)
    FOREACH(neg 0 1)
      ADD_TEST(Mask-${label}-${bg}-${neg} ${TEST_COMMAND}
        mask
        ${CMAKE_CURRENT_SOURCE_DIR}/images/2th_cthead1.png ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png mask-${label}-${bg}-${neg}.png
        ${label} ${bg} ${neg} 0 0
        --compare mask-${label}-${bg}-${neg}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/mask-${label}-${bg}-${neg}.png
      )
        
      FOREACH(border 0 10)
        ADD_TEST(MaskCrop-${label}-${bg}-${neg}-${border} ${TEST_COMMAND}
          mask
          ${CMAKE_CURRENT_SOURCE_DIR}/images/2th_cthead1.png ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png mask-${label}-${bg}-${neg}-${border}.png
          ${label} ${bg} ${neg} 1 ${border}
          --compare mask-${label}-${bg}-${neg}-${border}.png ${CMAKE_CURRENT_SOURCE_DIR}/images/mask-${label}-${bg}-${neg}-${border}.png
        )
      ENDFOREACH(border)
    ENDFOREACH(neg)
  ENDFOREACH(bg)
ENDFOREACH(label)


ADD_TEST(AttributeValues ${TEST_COMMAND}
  attribute_values
  ${CMAKE_CURRENT_SOURCE_DIR}/images/2th_cthead1.png
  200
)


ADD_TEST(PrintLabelObjects ${TEST_COMMAND}
  print_label_objects
  ${CMAKE_CURRENT_SOURCE_DIR}/images/2th_cthead1.png ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png
  200
)


ADD_TEST(GenericAttribute ${TEST_COMMAND}
  generic_attribute
  ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1-label.png ${CMAKE_CURRENT_SOURCE_DIR}/images/cthead1.png
  generic_attribute-attr.png generic_attribute-keep.png generic_attribute-open.png generic_attribute-relabel.png
  0 100 2
  --compare generic_attribute-attr.png ${CMAKE_CURRENT_SOURCE_DIR}/images/generic_attribute-attr.png
  --compare generic_attribute-keep.png ${CMAKE_CURRENT_SOURCE_DIR}/images/generic_attribute-keep.png
  --compare generic_attribute-open.png ${CMAKE_CURRENT_SOURCE_DIR}/images/generic_attribute-open.png
  --compare generic_attribute-relabel.png ${CMAKE_CURRENT_SOURCE_DIR}/images/generic_attribute-relabel.png
)


ADD_TEST(AttributeValues2 ${TEST_COMMAND}
  attribute_values_test
  ${CMAKE_CURRENT_SOURCE_DIR}/images/balls.nrrd
  ${CMAKE_CURRENT_SOURCE_DIR}/images/balls.nrrd
)
