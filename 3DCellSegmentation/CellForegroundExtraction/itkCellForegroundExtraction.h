#ifndef __itkCellForegroundExtraction_h
#define __itkCellForegroundExtraction_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegion.h"
#include "itkRegion.h"
#include "itkIndex.h"
#include "itkSize.h"

#include "itkImageFileWriter.h"

namespace itk {

template < class TInputImage, class TForegroundImage = Image<
  unsigned short,TInputImage::ImageDimension>, class TGaussFitImage = Image<
  float,TInputImage::ImageDimension> >
class ITK_EXPORT CellForegroundExtraction : public ImageToImageFilter<
  TInputImage, TForegroundImage >
{
public:

  typedef CellForegroundExtraction				Self;
  typedef ImageToImageFilter< TInputImage,TForegroundImage >	Superclass;
  typedef SmartPointer< Self >					Pointer;
  typedef SmartPointer< const Self >				ConstPointer;

  itkStaticConstMacro( ImageDimension, unsigned int,
    TInputImage::ImageDimension );

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(CellForegroundExtraction, ImageToImageFilter);

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

  typedef TForegroundImage                           ForegroundType;
  typedef typename ForegroundType::Pointer           ForegroundPointer;
  typedef typename ForegroundType::PixelType         ForegroundPixelType;
  typedef typename ForegroundType::SizeType          ForegroundSizeType;
  typedef typename ForegroundSizeType::SizeValueType ForegroundSizeValueType;

  typedef typename TGaussFitImage::Pointer GaussPointer;

  typedef CastImageFilter< TInputImage, ImageType > InputCastType;
  typedef typename InputCastType::Pointer           InputCastPointer;
  typedef RecursiveGaussianImageFilter< ImageType, ImageType >
    SmoothingFilterType;
  typedef typename SmoothingFilterType::Pointer     SmoothingFilterPointer;
  typedef RegionOfInterestImageFilter< ImageType,ImageType > ROIFilterType;
  typedef typename ROIFilterType::Pointer ROIFilterPointer;
  typedef BinaryThresholdImageFilter< ImageType, TForegroundImage >
    ThresholdFilterType;
  typedef typename ThresholdFilterType::Pointer ThresholdFilterPointer;
  typedef ResampleImageFilter<ImageType,ImageType > ResampleFilterType;
  typedef typename ResampleFilterType::Pointer ResampleFilterPointer;
  typedef LinearInterpolateImageFunction< ImageType > InterpolatorType;
  typedef typename InterpolatorType::Pointer InterpolatorPointer;
  typedef AffineTransform< double, ImageDimension > TransformType;
  typedef typename TransformType::Pointer TransformPointer;
  typedef RescaleIntensityImageFilter < ImageType, ImageType >
    RescaleFilterType;
  typedef typename RescaleFilterType::Pointer RescaleFilterPointer;
  typedef ImageRegionIterator< ImageType > IteratorType;
  typedef ImageRegionIterator< TForegroundImage > fgIteratorType;
  typedef ImageRegionIteratorWithIndex<ImageType > IndexIteratorType;
  typedef CastImageFilter< TForegroundImage, TForegroundImage >
    ForegroundCastType;
  typedef typename ForegroundCastType::Pointer ForegroundCastPointer;
  typedef CastImageFilter< ImageType, TGaussFitImage > GaussFitCastType;
  typedef typename GaussFitCastType::Pointer GaussFitCastPointer;
  typedef BinaryBallStructuringElement< ImagePixelType, ImageDimension >
    SEType;
  typedef BinaryMorphologicalClosingImageFilter< TForegroundImage,
    TForegroundImage, SEType > CloseFilterType;
  typedef typename CloseFilterType::Pointer CloseFilterPointer;
  typedef BinaryFillholeImageFilter< TForegroundImage > I2LType;
  typedef typename I2LType::Pointer I2LPointer;
  typedef ConnectedComponentImageFilter< TForegroundImage, TForegroundImage >
    ConnectedComponentFilterType;
  typedef typename ConnectedComponentFilterType::Pointer 
    ConnectedComponentFilterPointer;
  typedef RelabelComponentImageFilter< TForegroundImage, TForegroundImage >
    RelabelComponentFilterType;
  typedef typename RelabelComponentFilterType::Pointer 
    RelabelComponentFilterPointer;

  itkGetConstMacro( SigmaForm,double );
  itkSetMacro( SigmaForm,double );
  itkGetConstMacro( ThresholdCellmin,double );
  itkSetMacro( ThresholdCellmin,double );
  itkGetConstMacro( ThresholdCellmax,double );
  itkSetMacro( ThresholdCellmax,double );
  itkGetConstMacro( ThresholdMembrane,double );
  itkSetMacro( ThresholdMembrane,double );
  itkGetConstMacro( ThresholdForm,double );
  itkSetMacro( ThresholdForm,double );
  itkGetConstMacro( LargestCellRadius,double );
  itkSetMacro( LargestCellRadius,double );

	void SetSampling( float *sampling )
	{
		m_Sampling = sampling;
	}

  GaussPointer GetGaussCorrImage(void) const
  {
    return gauss;
  }

protected:

  CellForegroundExtraction();
  ~CellForegroundExtraction() {}
  ImagePointer InputCast( unsigned int i );
  ImagePointer GaussianCorrelation( ImagePointer rawImg );
  inline ImagePixelType
    PearsonCorrelation( ImagePointer input1, ImagePointer input2 );
  ImagePointer InitializeBlob( ImageSpacingType spacing, ImageSizeType size );
  ForegroundPointer Threshold( ImagePointer input, double thresh_low, double
  thresh_high, double in_value, double out_value );
  ImagePointer Resample( ImagePointer input, ImageSpacingType spacing,
    ImageSizeType size, ImagePointType origin );
  void GenerateData();

  InputCastPointer         m_CastInput;
  ForegroundCastPointer    m_CastForeground;
  GaussFitCastPointer      m_CastGaussFit;

  double m_SigmaForm;
  double m_ThresholdCellmin;
  double m_ThresholdCellmax;
  double m_ThresholdMembrane; // Change to size threshold
  double m_ThresholdForm;
  double m_LargestCellRadius;
  float *m_Sampling;
  GaussPointer gauss;

private:

  CellForegroundExtraction(Self&);   // intentionally not implemented
  void operator=(const Self&);   // intentionally not implemented
};

} /* namespace itk */

#include "itkCellForegroundExtraction.txx"
#endif
