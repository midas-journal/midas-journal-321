ADD_EXECUTABLE(voronoiSegment VoronoiBasedCellSegmentation.cxx)
TARGET_LINK_LIBRARIES(voronoiSegment ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(voronoiSegment2D VoronoiBasedCellSegmentation2D.cxx)
TARGET_LINK_LIBRARIES(voronoiSegment2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_TEST( voronoiSegment2DTest ${CXX_TEST_PATH}/voronoiSegment2D ${MY_DATAROOT}/30-fg.mha ${MY_DATAROOT}/30-levelset.mha ${MY_DATAROOT}/30-voronoi.mha )


