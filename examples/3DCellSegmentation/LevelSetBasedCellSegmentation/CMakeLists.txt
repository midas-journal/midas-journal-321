ADD_EXECUTABLE(cellSegment LevelSetBasedCellSegmentation.cxx)
TARGET_LINK_LIBRARIES(cellSegment ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(cellSegment2D LevelSetBasedCellSegmentation2D.cxx)
TARGET_LINK_LIBRARIES(cellSegment2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_TEST( cellSegment2DTest ${CXX_TEST_PATH}/cellSegment2D ${MY_DATAROOT}/30-feature.mha ${MY_DATAROOT}/30-seeds.txt ${MY_DATAROOT}/30-levelset.mha )

#ADD_EXECUTABLE(cellShapeSegment ShapeLevelSetBasedCellSegmentation.cxx)
#TARGET_LINK_LIBRARIES(cellShapeSegment ITKBasicFilters ITKAlgorithms #ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(cellShapeSegment2D ShapeLevelSetBasedCellSegmentation2D.cxx)
TARGET_LINK_LIBRARIES(cellShapeSegment2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)
