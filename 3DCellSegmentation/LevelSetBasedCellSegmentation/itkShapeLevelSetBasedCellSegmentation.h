#ifndef __itkShapeLevelSetBasedCellSegmentation_h
#define __itkShapeLevelSetBasedCellSegmentation_h

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
#include "itkPCAShapeSignedDistanceFunction.h"
#include "itkEuler2DTransform.h"
#include "itkShapePriorMAPCostFunction.h"
#include "itkNormalVariateGenerator.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkSpatialFunctionImageEvaluatorFilter.h"
#include "itkGeodesicActiveContourShapePriorLevelSetImageFilter.h"
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
class ITK_EXPORT ShapeLevelSetBasedCellSegmentation : public ImageToImageFilter<
TFeatureImage, TOutputImage >
{
public:

  typedef ShapeLevelSetBasedCellSegmentation             Self;
  typedef ImageToImageFilter< TFeatureImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
    TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(ShapeLevelSetBasedCellSegmentation, ImageToImageFilter);

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

  typedef PCAShapeSignedDistanceFunction< double, ImageDimension, ImageType >
    ShapeFunctionType;
  typedef Euler2DTransform< double > TransformType;
  typedef ShapePriorMAPCostFunction< ImageType, float > CostFunctionType;
  typedef OnePlusOneEvolutionaryOptimizer OptimizerType;
  typedef Statistics::NormalVariateGenerator GeneratorType;
  typedef SpatialFunctionImageEvaluatorFilter< ShapeFunctionType,
    ImageType, ImageType > EvaluatorFilterType;
  typedef GeodesicActiveContourShapePriorLevelSetImageFilter<
    ImageType, ImageType > GeodesicActiveContourShapeFilterType;


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


  itkGetMacro( LargestCellRadius, double );
  itkSetMacro( LargestCellRadius, double );
  itkGetMacro( SeedValue, double );
  itkSetMacro( SeedValue, double );
  itkGetMacro( Iterations, int );
  itkSetMacro( Iterations, int );
  itkGetMacro( PropagationScaling, double );
  itkSetMacro( PropagationScaling, double );
  itkGetMacro( CurvatureScaling, double );
  itkSetMacro( CurvatureScaling, double );
  itkGetMacro( AdvectionScaling, double );
  itkSetMacro( AdvectionScaling, double );
  itkGetMacro( MaxRMSChange, double );
  itkSetMacro( MaxRMSChange, double );
  itkGetMacro( ShapePriorScaling, double );
  itkSetMacro( ShapePriorScaling, double );
  itkGetMacro( NumberOfPCAModes, int );
  itkSetMacro( NumberOfPCAModes, int );

  std::map< float, ImageIndexType > seeds;
  ImagePointer m_meanShape;
  std::vector< ImagePointer > m_shapeModeImages;

protected:

  ShapeLevelSetBasedCellSegmentation();
  ~ShapeLevelSetBasedCellSegmentation() {}
  CompImagePointer Threshold( ImagePointer input, double thresh_low,
    double thresh_high, unsigned int in_value, unsigned int out_value );
  CompImagePointer GACShape( ImagePointer speed, int NumOfCells, ImageIndexType
    start, ImageSizeType size );
  CompImagePointer Resample (CompImagePointer input, ImageSpacingType spacing,
    ImageSizeType size, ImagePointType origin);
  void InitializeLevelSet(ImageSpacingType spacing );
  void GenerateData();

  typename FeatureCastType::Pointer m_CastFeature;
  typename ROIFilterType::Pointer m_ROIfilter;
  typename OutputCastType::Pointer m_CastOutput;
  typename DistanceFilterType::Pointer m_Dist;

	double           m_LargestCellRadius;
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

  ShapeLevelSetBasedCellSegmentation(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented
};

} /* namespace itk */

#include "itkShapeLevelSetBasedCellSegmentation.txx"
#endif
