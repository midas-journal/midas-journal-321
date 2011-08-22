ADD_EXECUTABLE(cellFeature CellFeatureGenerator.cxx)
TARGET_LINK_LIBRARIES(cellFeature ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE( cellFeature2D CellFeatureGenerator2D.cxx)
TARGET_LINK_LIBRARIES(cellFeature2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO )

ADD_TEST( cellFeature2DTest ${CXX_TEST_PATH}/cellFeature2D 
${MY_DATAROOT}/30.png ${MY_DATAROOT}/30-fg.mha ${MY_DATAROOT}/30-gf.mha ${MY_DATAROOT}/30-feature.mha ${MY_DATAROOT}/30-dist.mha )

