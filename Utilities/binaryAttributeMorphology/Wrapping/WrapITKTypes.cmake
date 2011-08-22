
# define a new type

WRAP_TYPE("itk::LabelMap" "LM")
  FOREACH(d ${WRAP_ITK_DIMS})
    ADD_TEMPLATE("${d}" "itk::StatisticsLabelObject< ${ITKT_UL}, ${d} >")
  ENDFOREACH(d)
END_WRAP_TYPE()
SET(itk_Wrap_LabelMap ${WRAPPER_TEMPLATES})

#add corresponding header in default files

SET(WRAPPER_DEFAULT_INCLUDE
  ${WRAPPER_DEFAULT_INCLUDE}
  "itkStatisticsLabelObject.h"
  "itkLabelMap.h"
)
