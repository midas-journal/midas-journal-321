ADD_EXECUTABLE(seedExtract SeedExtraction.cxx)
TARGET_LINK_LIBRARIES(seedExtract ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE( seedExtract2D SeedExtraction2D.cxx)
TARGET_LINK_LIBRARIES(seedExtract2D ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO )

ADD_TEST( seedExtract2DTest ${CXX_TEST_PATH}/seedExtract2D ${MY_DATAROOT}/30-fg.mha ${MY_DATAROOT}/30-dist.mha ${MY_DATAROOT}/30-gf.mha ${MY_DATAROOT}/30-seeds.txt )


