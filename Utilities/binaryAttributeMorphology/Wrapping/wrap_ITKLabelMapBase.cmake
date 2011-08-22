WRAP_CLASS("itk::ImageSource" POINTER)
  FOREACH(d ${WRAP_ITK_DIMS})
    WRAP_TEMPLATE("${ITKM_LM${d}}" "${ITKT_LM${d}}")
  ENDFOREACH(d)
END_WRAP_CLASS()

WRAP_CLASS("itk::ImageToImageFilter" POINTER)
  FOREACH(d ${WRAP_ITK_DIMS})
    # label collection -> label collection
    WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_LM${d}}" "${ITKT_LM${d}}, ${ITKT_LM${d}}")

    FOREACH(t ${WRAP_ITK_SCALAR})
      # image -> label collection image
      WRAP_TEMPLATE("${ITKM_I${t}${d}}${ITKM_LM${d}}" "${ITKT_I${t}${d}}, ${ITKT_LM${d}}")
      # label collection -> image
      WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_I${t}${d}}" "${ITKT_LM${d}}, ${ITKT_I${t}${d}}")
    ENDFOREACH(t)

    # label map -> rgb image
    IF(WRAP_rgb_unsigned_char)
      WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_IRGBUC${d}}" "${ITKT_LM${d}}, ${ITKT_IRGBUC${d}}")
    ENDIF(WRAP_rgb_unsigned_char)

    IF(WRAP_rgb_unsigned_short)
      WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_IRGBUS${d}}" "${ITKT_LM${d}}, ${ITKT_IRGBUS${d}}")
    ENDIF(WRAP_rgb_unsigned_short)

  ENDFOREACH(d)
END_WRAP_CLASS()
