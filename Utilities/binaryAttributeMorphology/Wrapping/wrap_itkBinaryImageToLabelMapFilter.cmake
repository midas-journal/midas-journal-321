WRAP_CLASS("itk::BinaryImageToLabelMapFilter" POINTER)
  FOREACH(t ${WRAP_ITK_USIGN_INT})
    FOREACH(d ${WRAP_ITK_DIMS})
      # image -> label collection image
      WRAP_TEMPLATE("${ITKM_I${t}${d}}${ITKM_LM${d}}" "${ITKT_I${t}${d}}, ${ITKT_LM${d}}")
    ENDFOREACH(d)
  ENDFOREACH(t)
END_WRAP_CLASS()
