ADD_EXECUTABLE(cellForeground CellForegroundExtraction.cxx)
TARGET_LINK_LIBRARIES(cellForeground ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(cellForeground2D CellForegroundExtraction2D.cxx)
TARGET_LINK_LIBRARIES(cellForeground2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_TEST( CellForegroundExtraction2DTest ${CXX_TEST_PATH}/cellForeground2D ${MY_DATAROOT}/30.png ${MY_DATAROOT}/30.png ${MY_DATAROOT}/30-fg.mha ${MY_DATAROOT}/30-gf.mha )


