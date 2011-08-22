#ifndef __itkSeedExtraction_h
#define __itkSeedExtraction_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"
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
Image< bool,TFeatureImage::ImageDimension > >
class ITK_EXPORT SeedExtraction :
public ImageToImageFilter< TFeatureImage, TOutputImage >
{
public:

  typedef SeedExtraction             Self;
  typedef ImageToImageFilter< TFeatureImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
TFeatureImage::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(SeedExtraction, ImageToImageFilter);

  /** Display */
  void PrintSelf( std::ostream& os, Indent indent ) const;

  typedef Image< float,ImageDimension >    ImageType;
  typedef typename ImageType::Pointer      ImagePointer;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::PixelType    ImagePixelType;
  typedef typename ImageType::IndexType    ImageIndexType;
  typedef typename ImageType::SizeType     ImageSizeType;
  typedef typename ImageSizeType::SizeValueType ImageSizeValueType;
  typedef typename ImageType::SpacingType  ImageSpacingType;

  typedef Image< bool,ImageDimension >       fgImageType;
  typedef typename fgImageType::Pointer      fgImagePointer;
  typedef typename fgImageType::ConstPointer fgImageConstPointer;
  typedef typename fgImageType::IndexType    fgImageIndexType;
  typedef typename fgImageType::PixelType    fgImagePixelType;

  typedef CastImageFilter< TFeatureImage, ImageType >  FeatureCastType;
  typedef ImageRegionIteratorWithIndex< ImageType >    IndexIteratorType;
  typedef CastImageFilter< fgImageType, TOutputImage > OutputCastType;
  typedef DiscreteGaussianImageFilter< ImageType,ImageType >
    GaussianFilterType;
  typedef BinaryBallStructuringElement< ImagePixelType, ImageDimension >
    SEType;
  typedef GrayscaleDilateImageFilter< ImageType, ImageType, SEType >
    DilateFilterType;

  itkGetConstMacro( LargestCellRadius, double );
  itkSetMacro(      LargestCellRadius, double );

  std::map< float, ImageIndexType > seeds;
  std::vector< unsigned long >      sizes;

  void SetForeground( fgImagePointer fg )
  {
    m_fgMap = fg;
    this->Modified();
  }

protected:

  SeedExtraction();
  ~SeedExtraction() {}
  ImagePointer InputCast( unsigned int i );
  void ImageSeeds( ImagePointer featureImg );
  void GenerateData();

  typename OutputCastType::Pointer      m_CastOutput;

  double         m_LargestCellRadius;
  fgImagePointer m_fgMap;

private:

  SeedExtraction(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented
};

} /* namespace itk */

#include "itkSeedExtraction.txx"
#endif
