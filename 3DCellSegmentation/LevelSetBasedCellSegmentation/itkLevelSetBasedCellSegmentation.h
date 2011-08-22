#ifndef __itkLevelSetBasedCellSegmentation_h
#define __itkLevelSetBasedCellSegmentation_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkNormalVariateGenerator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"
#include <map>
#include <vector>

#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"

namespace itk {

  template < class TFeatureImage, class TOutputImage =
Image< unsigned short, TFeatureImage::ImageDimension > >
class ITK_EXPORT LevelSetBasedCellSegmentation : public ImageToImageFilter<
TFeatureImage, TOutputImage >
{
public:

  typedef LevelSetBasedCellSegmentation             Self;
  typedef ImageToImageFilter< TFeatureImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
    TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(LevelSetBasedCellSegmentation, ImageToImageFilter);

  /** Display */
  void PrintSelf( std::ostream& os, Indent indent ) const;

  typedef Image< unsigned short,ImageDimension >    CompImageType;
  typedef typename CompImageType::Pointer           CompImagePointer;
  typedef typename CompImageType::ConstPointer      CompImageConstPointer;
  typedef typename CompImageType::PixelType         CompImagePixelType;
  typedef typename CompImageType::RegionType        CompImageRegionType;
  typedef typename CompImageType::SizeType          CompImageSizeType;
  typedef typename CompImageType::IndexType         CompImageIndexType;

  typedef Image< float,ImageDimension >    ImageType;
  typedef typename ImageType::Pointer      ImagePointer;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::PixelType    ImagePixelType;
  typedef typename ImageType::RegionType   ImageRegionType;
  typedef typename ImageType::SizeType     ImageSizeType;
  typedef typename ImageSizeType::SizeValueType ImageSizeValueType;
  typedef typename ImageType::IndexType    ImageIndexType;
  typedef typename ImageIndexType::IndexValueType ImageIndexValueType;
  typedef typename ImageType::SpacingType  ImageSpacingType;
  typedef typename ImageType::PointType    ImagePointType;

  typedef CastImageFilter< TFeatureImage, ImageType > FeatureCastType;
  typedef RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;

  typedef GeodesicActiveContourLevelSetImageFilter< ImageType, ImageType >
    GeodesicActiveContourFilterType;
  typedef Statistics::NormalVariateGenerator GeneratorType;

  typedef BinaryThresholdImageFilter<ImageType,CompImageType>
    ThresholdingFilterType;
  typedef DanielssonDistanceMapImageFilter< ImageType,ImageType >
    DistanceFilterType;
  typedef ImageRegionIterator< CompImageType>
    CompIteratorType;
  typedef ImageRegionIterator< ImageType >
    IteratorType;
  typedef CastImageFilter< CompImageType, TOutputImage >
    OutputCastType;


  itkGetConstMacro( LargestCellRadius, double );
  itkSetMacro( LargestCellRadius, double );
  itkGetConstMacro( SeedValue, double );
  itkSetMacro( SeedValue, double );
  itkGetConstMacro( Iterations, int );
  itkSetMacro( Iterations, int );
  itkGetConstMacro( PropagationScaling, double );
  itkSetMacro( PropagationScaling, double );
  itkGetConstMacro( CurvatureScaling, double );
  itkSetMacro( CurvatureScaling, double );
  itkGetConstMacro( AdvectionScaling, double );
  itkSetMacro( AdvectionScaling, double );
  itkGetConstMacro( MaxRMSChange, double );
  itkSetMacro( MaxRMSChange, double );

  std::map< float, ImageIndexType > seeds;

protected:

  LevelSetBasedCellSegmentation();
  ~LevelSetBasedCellSegmentation() {}
  CompImagePointer Threshold( ImagePointer input, double thresh_low,
    double thresh_high, unsigned int in_value, unsigned int out_value );
  CompImagePointer GAC( ImagePointer speed, int NumOfCells, ImageIndexType
    start, ImageSizeType size );
  CompImagePointer Resample (CompImagePointer input, ImageSpacingType spacing,
    ImageSizeType size, ImagePointType origin);
  void InitializeLevelSet(ImageSpacingType spacing );
  void GenerateData();

  typename FeatureCastType::Pointer m_CastFeature;
  typename ROIFilterType::Pointer m_ROIfilter;
  typename OutputCastType::Pointer m_CastOutput;
  typename DistanceFilterType::Pointer m_Dist;

  double        m_LargestCellRadius;
  double        m_SeedValue;
  int           m_Iterations;
  double        m_PropagationScaling;
  double        m_CurvatureScaling;
  double        m_AdvectionScaling;
  double        m_MaxRMSChange;
  double        m_ShapePriorScaling;
  int           m_NumberOfPCAModes;

  ImagePointer m_levelSet;

private:

  LevelSetBasedCellSegmentation(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented
};

} /* namespace itk */

#include "itkLevelSetBasedCellSegmentation.txx"
#endif
