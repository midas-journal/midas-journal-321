ADD_EXECUTABLE(cellStatistics CellStatistics.cxx )
TARGET_LINK_LIBRARIES(cellStatistics ITKBasicFilters ITKAlgorithms
ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(cellStatistics2D CellStatistics2D.cxx )
TARGET_LINK_LIBRARIES(cellStatistics2D ITKBasicFilters ITKAlgorithms
ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_TEST( cellStatistics2DTest ${CXX_TEST_PATH}/cellStatistics2D ${MY_DATAROOT}/30-voronoi.mha ${MY_DATAROOT}/30.png ${MY_DATAROOT}/30-segment.mha )

