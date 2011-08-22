#ifndef __itkVoronoiBasedCellSegmentation_h
#define __itkVoronoiBasedCellSegmentation_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"
#include <map>

#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

#include "itkNumericTraits.h"

namespace itk {

  template < class TSegmentImage, class TVoronoiImage =
Image<unsigned short, TSegmentImage::ImageDimension> >
class ITK_EXPORT VoronoiBasedCellSegmentation : public ImageToImageFilter<
TSegmentImage, TVoronoiImage >
{
public:

  typedef VoronoiBasedCellSegmentation             Self;
  typedef ImageToImageFilter< TSegmentImage,TVoronoiImage> Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
    TSegmentImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro( VoronoiBasedCellSegmentation, ImageToImageFilter );

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
	
  typedef Image< bool, ImageDimension >      FgImageType;
  typedef typename FgImageType::Pointer      FgImagePointer;
  typedef typename FgImageType::ConstPointer FgImageConstPointer;
  typedef typename FgImageType::IndexType    FgImageIndexType;
  typedef typename FgImageType::PixelType    FgImagePixelType;	

  typedef CastImageFilter< TSegmentImage, ImageType> SegmentCastType;
  typedef RegionOfInterestImageFilter<ImageType,ImageType> ROIFilterType;
  typedef BinaryThresholdImageFilter<ImageType,ImageType>
    ThresholdingFilterType;
  typedef DanielssonDistanceMapImageFilter< ImageType,ImageType >
    DistanceFilterType;
  typedef ImageRegionIterator< ImageType > IteratorType;
  typedef CastImageFilter< ImageType, TVoronoiImage > VoronoiCastType;

  typedef unsigned short LabelType;
  typedef ShapeLabelObject< LabelType, ImageDimension > LabelObjectType;
  typedef LabelMap< LabelObjectType > LabelMapType;
  typedef BinaryImageToShapeLabelMapFilter< FgImageType, LabelMapType >
    ConverterType;
  typedef LabelMapToLabelImageFilter< LabelMapType, ImageType >
    MapToImageType;

  itkGetConstMacro( MinComponentSize, int );
  itkSetMacro( MinComponentSize, int );

  void SetForegroundImage( FgImagePointer comp )
  {
    m_fgMap = comp;
    this->Modified();
  }

protected:

  VoronoiBasedCellSegmentation();
  ~VoronoiBasedCellSegmentation() {}
  ImagePointer ComputeVoronoi( ImagePointer roiComp, ImagePointer roiSegment,
    unsigned short label);
  ImagePointer Threshold( ImagePointer input, double thresh_low,
    double thresh_high, double in_value, double out_value );
  void GenerateData();

  typename SegmentCastType::Pointer m_CastSegment;
  typename ROIFilterType::Pointer   m_ROIfilter1;
  typename ROIFilterType::Pointer   m_ROIfilter2;
  typename VoronoiCastType::Pointer m_CastVoronoi;
  typename MapToImageType::Pointer  m_Map2Image;

  int m_MinComponentSize;
  FgImagePointer m_fgMap;

private:

  VoronoiBasedCellSegmentation(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented
};

} /* namespace itk */

#include "itkVoronoiBasedCellSegmentation.txx"
#endif
