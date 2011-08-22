#ifndef __itkCellFeatureGenerator_h
#define __itkCellFeatureGenerator_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkAbsImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

#include "itkImageFileWriter.h"

namespace itk {

template < class TInputImage, class
TFeatureImage = Image<float,TInputImage::ImageDimension> >
class ITK_EXPORT CellFeatureGenerator : public ImageToImageFilter< TInputImage,
TFeatureImage >
{
public:

  typedef CellFeatureGenerator                              Self;
  typedef ImageToImageFilter< TInputImage,TFeatureImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( CellFeatureGenerator, ImageToImageFilter );

  /** Display */
  void PrintSelf( std::ostream& os, Indent indent ) const;

  typedef Image< float,ImageDimension >    ImageType;
  typedef typename ImageType::Pointer      ImagePointer;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::PixelType    ImagePixelType;
  typedef typename ImageType::RegionType   ImageRegionType;
  typedef typename ImageType::SizeType     ImageSizeType;
  typedef typename ImageSizeType::SizeValueType ImageSizeValueType;
  typedef typename ImageType::SpacingType  ImageSpacingType;
  typedef typename ImageType::IndexType    ImageIndexType;
  typedef typename ImageType::PointType    ImagePointType;

  typedef Image< bool, ImageDimension >      FgImageType;
  typedef typename FgImageType::Pointer     FgImagePointer;
  typedef typename FgImageType::ConstPointer FgImageConstPointer;
  typedef typename FgImageType::IndexType   FgImageIndexType;
  typedef typename FgImageType::PixelType   FgImagePixelType;

  typedef CastImageFilter< TInputImage, ImageType > InputCastType;
  typedef typename InputCastType::Pointer InputCastPointer;
  typedef GradientMagnitudeRecursiveGaussianImageFilter< ImageType, ImageType >
    GradientFilterType;
  typedef typename GradientFilterType::Pointer GradientFilterPointer;
  typedef SigmoidImageFilter< ImageType,ImageType > SigmoidFilterType;
  typedef typename SigmoidFilterType::Pointer SigmoidFilterPointer;
  typedef AbsImageFilter< ImageType,ImageType > AbsFilterType;
  typedef typename AbsFilterType::Pointer AbsFilterPointer;
  typedef ThresholdImageFilter< ImageType > ThreshFilterType;
  typedef typename ThreshFilterType::Pointer ThreshFilterPointer;
  typedef RescaleIntensityImageFilter< ImageType,ImageType > RescaleType;
  typedef typename RescaleType::Pointer RescalePointer;
  typedef ImageRegionIterator<ImageType> IteratorType;
  typedef ImageRegionIteratorWithIndex<ImageType> IndexIteratorType;
  typedef CastImageFilter< ImageType, TFeatureImage > FeatureCastType;
  typedef typename FeatureCastType::Pointer FeatureCastPointer;
  typedef SignedMaurerDistanceMapImageFilter< FgImageType, ImageType >
    MaurerType;
  typedef typename MaurerType::Pointer MaurerPointer;
  typedef ResampleImageFilter<ImageType,ImageType > ResampleFilterType;
  typedef typename ResampleFilterType::Pointer ResampleFilterPointer;
  typedef LinearInterpolateImageFunction< ImageType > InterpolatorType;
  typedef typename InterpolatorType::Pointer InterpolatorPointer;
  typedef AffineTransform<double,ImageDimension> TransformType;
  typedef typename TransformType::Pointer TransformPointer;

  itkGetConstMacro( SigmaCell,double );
  itkSetMacro( SigmaCell,double );
  itkGetConstMacro( SigmaCorrelation,double );
  itkSetMacro( SigmaCorrelation,double );
  itkGetConstMacro( LargestCellRadius,double );
  itkSetMacro( LargestCellRadius,double );
  itkGetConstMacro( DistanceMapWeight,double );
  itkSetMacro( DistanceMapWeight,double );
  itkGetConstMacro( GradientMagnitudeWeight,double );
  itkSetMacro( GradientMagnitudeWeight,double );
  itkGetConstMacro( GaussCorrWeight,double );
  itkSetMacro( GaussCorrWeight,double );

  void SetSampling( float *sampling )
  {
    m_Sampling = sampling;
  }

  void SetForeground( FgImagePointer fg )
  {
    fgMap = fg;
  }

  ImagePointer GetDistanceMap()
  {
    return dist;
  }

  ImagePointer GetGradient()
  {
    return grad;
  }


protected:

  CellFeatureGenerator();
  ~CellFeatureGenerator() {}
  ImagePointer InputCast( unsigned int i );
  void DistanceMap( FgImagePointer input);
  ImagePointer Gradient( ImagePointer input, double sigma );
  ImagePointer FeatureImage(ImagePointer dist);
	void Normalize(ImagePointer grad1, ImagePointer grad2 );
  ImagePointer Resample( ImagePointer input, ImageSpacingType spacing,
    ImageSizeType size, ImagePointType origin );
  void GenerateData();

  double	m_SigmaCell;
  double  m_SigmaCorrelation;
  double  m_LargestCellRadius;
  double	m_DistanceMapWeight;
  double	m_GradientMagnitudeWeight;
  double  m_GaussCorrWeight;
	float *m_Sampling;
  FgImagePointer fgMap;
  ImagePointer dist;
	ImagePointer grad;

private:

  CellFeatureGenerator(Self&);   // intentionally not implemented
  void operator=(const Self&);   // intentionally not implemented
};

} /* namespace itk */

#include "itkCellFeatureGenerator.txx"
#endif
