#ifndef __itkCellStatistics_h
#define __itkCellStatistics_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"
#include <map>

#include "itkLabelObject.h"
#include "itkShapeLabelObject.h"
#include "itkStatisticsLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkShapeRelabelImageFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

#include "itkNumericTraits.h"

namespace itk {

  template < class TSegmentImage, class TOutputImage =
Image<unsigned short, TSegmentImage::ImageDimension> >
class ITK_EXPORT CellStatistics : public ImageToImageFilter<
TSegmentImage, TOutputImage >
{
public:

  typedef CellStatistics                            Self;
  typedef ImageToImageFilter< TSegmentImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
    TSegmentImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro( CellStatistics, ImageToImageFilter );

  /** Display */
  void PrintSelf( std::ostream& os, Indent indent ) const;

  typedef Image< unsigned short,ImageDimension >  ImageType;
  typedef typename ImageType::Pointer             ImagePointer;
  typedef typename ImageType::ConstPointer        ImageConstPointer;
  typedef typename ImageType::PixelType           ImagePixelType;
  typedef typename ImageType::RegionType          ImageRegionType;
  typedef typename ImageType::SizeType            ImageSizeType;
  typedef typename ImageSizeType::SizeValueType   ImageSizeValueType;
  typedef typename ImageType::IndexType           ImageIndexType;
  typedef typename ImageIndexType::IndexValueType ImageIndexValueType;
  typedef typename ImageType::SpacingType         ImageSpacingType;
  typedef typename ImageType::PointType           ImagePointType;

  typedef Image< float,ImageDimension >           FloatImageType;
  typedef typename FloatImageType::Pointer        FloatImagePointer;

  typedef CastImageFilter< TSegmentImage, ImageType> SegmentCastType;
  typedef CastImageFilter< ImageType,TOutputImage >  OutputCastType;

  typedef unsigned short                                LabelType;
  typedef ShapeLabelObject< LabelType, ImageDimension > ShapeLabelObjectType;
  typedef StatisticsLabelObject< LabelType, ImageDimension >
    StatLabelObjectType;
  typedef LabelMap< ShapeLabelObjectType >              ShapeLabelMapType;
  typedef LabelMap< StatLabelObjectType >               StatLabelMapType;
  typedef LabelImageToShapeLabelMapFilter< ImageType,   ShapeLabelMapType >
    ShapeConverterType;
  typedef LabelImageToStatisticsLabelMapFilter< ImageType, FloatImageType,
    StatLabelMapType > StatConverterType;

  typedef ShapeLabelMapFilter< ShapeLabelMapType > ShapeFilterType;
  typedef ShapeRelabelImageFilter< ImageType >     RelabelType;

  typedef LabelMapToLabelImageFilter< ShapeLabelMapType, ImageType >
    MapToImageType;

  void SetRawImage( FloatImagePointer raw )
    {
    m_RawImg = raw;
    this->Modified();
    }

protected:

  CellStatistics();
  ~CellStatistics() {}
  void GenerateData();

  typename SegmentCastType::Pointer m_CastSegment;
  typename ShapeFilterType::Pointer m_Shape;
  typename RelabelType::Pointer     m_Relabel;
  typename OutputCastType::Pointer  m_CastOutput;

  FloatImagePointer m_RawImg;

private:

  CellStatistics(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented
};

} /* namespace itk */

#include "itkCellStatistics.txx"
#endif
