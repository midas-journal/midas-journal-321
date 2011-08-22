WRAP_CLASS("itk::LabelMapToRGBImageFilter" POINTER)

  FOREACH(d ${WRAP_ITK_DIMS})

    IF(WRAP_rgb_unsigned_char)
      WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_IRGBUC${d}}" "${ITKT_LM${d}}, ${ITKT_IRGBUC${d}}")
    ENDIF(WRAP_rgb_unsigned_char)

    IF(WRAP_rgb_unsigned_short)
      WRAP_TEMPLATE("${ITKM_LM${d}}${ITKM_IRGBUS${d}}" "${ITKT_LM${d}}, ${ITKT_IRGBUS${d}}")
    ENDIF(WRAP_rgb_unsigned_short)

  ENDFOREACH(d)

END_WRAP_CLASS()
