#ifndef __itkCellForegroundExtraction_txx
#define __itkCellForegroundExtraction_txx

#include "itkCellForegroundExtraction.h"

namespace itk
{
template < class TInputImage,class TForegroundImage,class TGaussFitImage >
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::CellForegroundExtraction() : m_SigmaForm( 2.0 ),
  m_ThresholdCellmin( 10 ), m_ThresholdCellmax( 30 ),  m_ThresholdMembrane( 0 ),
  m_ThresholdForm( 0.15 ), m_LargestCellRadius( 4.0 )
{
  m_CastInput = InputCastType::New();
  m_CastForeground = ForegroundCastType::New();
  m_CastGaussFit = GaussFitCastType::New();

  this->Superclass::SetNumberOfRequiredInputs( 2 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0,TForegroundImage::New() );
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage, TForegroundImage,
  TGaussFitImage >::ImagePointer
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::InitializeBlob
( ImageSpacingType spacing, ImageSizeType m_CellExtent )
{
  ImageRegionType blobRegion;
  ImageIndexType blobStart, blobOrigin;
  unsigned int i;

  for( i=0; i<ImageDimension; i++ )
  {
    blobOrigin[i] = static_cast<ImageSizeValueType>(m_CellExtent[i]/2);
    blobStart[i] = 0;
  }
  blobRegion.SetSize( m_CellExtent );
  blobRegion.SetIndex(blobStart);

  ImagePointer blob = ImageType::New();
  blob->SetRegions(blobRegion);
  blob->Allocate();
  blob->SetSpacing( spacing );

  float T = -1./(2.*m_SigmaForm*m_SigmaForm);
  ImageIndexType idx;
  float dx, sum;

  IndexIteratorType blobIt( blob,blob->GetLargestPossibleRegion() );
  for ( blobIt.GoToBegin(); !blobIt.IsAtEnd(); ++blobIt )
  {
    idx = blobIt.GetIndex();
    sum = 0;
    for( i=0; i<ImageDimension; i++ )
    {
      dx = (idx[i]-blobOrigin[i])*spacing[i];
			sum += dx*dx;
    }
    blobIt.Set( static_cast<ImagePixelType>( vcl_exp( sum * T ) ) );
  }
  return(blob);
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::ImagePixelType
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::PearsonCorrelation
( ImagePointer input1, ImagePointer input2 )
{
  int n(0);
  float x(0),y(0),xy(0),x2(0),y2(0);
  IteratorType inputIt1(input1,input1->GetLargestPossibleRegion());
  IteratorType inputIt2(input2,input2->GetLargestPossibleRegion());
  ImagePixelType p1, p2;
  for( inputIt1.GoToBegin(), inputIt2.GoToBegin(); !inputIt1.IsAtEnd();
    ++inputIt1, ++inputIt2, ++n )
  {
    p1 = inputIt1.Get();
    p2 = inputIt2.Get();
    x += p1;
    y += p2;
    xy += p1*p2;
    x2 += p1*p1;
    y2 += p2*p2;
  }
  float d = (n*x2 - x*x)*(n*y2 - y*y);

  if (d==0)
  {
    return 0;
  }else
  {
    return ((n*xy)-(x*y))/vcl_sqrt(d);
  }
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::ImagePointer
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::GaussianCorrelation
(ImagePointer input)
{
  ImageSizeType inputSize = input->GetLargestPossibleRegion().GetSize();
  ImageSpacingType spacing = input->GetSpacing();

  ROIFilterPointer m_roi = ROIFilterType::New();

  ImageSizeType m_CellExtent;
  unsigned int i;
  for( i = 0; i < ImageDimension; i++ )
    {
    m_CellExtent[i] = static_cast< ImageSizeValueType >
			( 2 * m_LargestCellRadius/spacing[i] );
    }

  ImagePointer output = ImageType::New();
  output->SetRegions( input->GetLargestPossibleRegion() );
  output->SetSpacing( spacing );
  output->SetOrigin( input->GetOrigin() );
  output->Allocate();
  output->FillBuffer( 0 );

  // Define a image region to run windowing on
  ImageRegionType region;
  ImageIndexType start;
  ImageSizeType size;
  for( i=0; i<ImageDimension; i++ )
    {
    start[i] = static_cast< ImageSizeValueType >( m_CellExtent[i]/2 );
    size[i] = inputSize[i] - m_CellExtent[i] + 1;
    }
  region.SetSize(size);
  region.SetIndex(start);

  //Generate window regions
  m_roi->SetInput( input );

  //Set window region size and start locations
  ImageRegionType windowRegion;
  windowRegion.SetSize( m_CellExtent );

  // Initialize Gaussian blob
  ImagePointer blob = InitializeBlob( input->GetSpacing(), m_CellExtent );

  IndexIteratorType indexIt( input,region );
  int count = 0;
  ImageIndexType idx, windowStart;
  ImagePixelType p;
  for ( indexIt.GoToBegin(); !indexIt.IsAtEnd(); ++indexIt, count++ )
    {
    idx = indexIt.GetIndex();
    for( i = 0; i < ImageDimension; i++ )
      {
      windowStart[i] = idx[i]-static_cast<ImageSizeValueType>(
        m_CellExtent[i]/2 );
      }
    windowRegion.SetIndex(windowStart);

    m_roi->SetRegionOfInterest( windowRegion );
    m_roi->Update();

    p = PearsonCorrelation(blob,m_roi->GetOutput());
    output->SetPixel( idx, p );
    }
  return(output);
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::ForegroundPointer
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::Threshold
( ImagePointer input, double thresh_low, double thresh_high, double in_value,
	double out_value )
{
  ThresholdFilterPointer m_thresholder = ThresholdFilterType::New();;
  m_thresholder->SetLowerThreshold( thresh_low );
  m_thresholder->SetUpperThreshold( thresh_high );
  m_thresholder->SetInsideValue( in_value );
  m_thresholder->SetOutsideValue( out_value );
  m_thresholder->SetInput( input );
  m_thresholder->Update();

  ForegroundPointer thresholdImg = m_thresholder->GetOutput();

  return ( thresholdImg );
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::ImagePointer
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >
::Resample
(ImagePointer input, ImageSpacingType spacing, ImageSizeType size, ImagePointType origin)
{
  // create the resample filter, transform and interpolator
  TransformPointer transform = TransformType::New();
  transform->SetIdentity();

  InterpolatorPointer interp = InterpolatorType::New();

  ResampleFilterPointer m_resamp = ResampleFilterType::New();
  m_resamp->SetTransform(transform);
  m_resamp->SetInterpolator(interp);
  m_resamp->SetInput(input);
  m_resamp->SetSize(size);
  m_resamp->SetOutputOrigin(origin);
  m_resamp->SetOutputSpacing(spacing);
  m_resamp->SetDefaultPixelValue(0);
  m_resamp->Update();

  return(m_resamp->GetOutput());
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
typename CellForegroundExtraction< TInputImage, TForegroundImage,
  TGaussFitImage >::ImagePointer
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::
InputCast
( unsigned int i)
{
  InputCastPointer m_CastInput = InputCastType::New();
  m_CastInput->SetInput( this->GetInput( i ) );
  m_CastInput->Update();

  return ( m_CastInput->GetOutput() );
}

template< class TInputImage,class TForegroundImage,class TGaussFitImage >
void
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::
GenerateData()
{
  ImagePointer cellImg = InputCast( 0 );
  ImagePointer membraneImg = InputCast( 1 );

  {
  SmoothingFilterPointer m_smooth = SmoothingFilterType::New();
  m_smooth->SetInput( cellImg);
  m_smooth->SetSigma( m_LargestCellRadius/10 );
  m_smooth->Update();
  cellImg = m_smooth->GetOutput();
  cellImg->DisconnectPipeline();
  }

  ImageSpacingType  spacing = cellImg->GetSpacing();
  ImageSizeType     size = cellImg->GetLargestPossibleRegion().GetSize();
  ImagePointType    origin = cellImg->GetOrigin();

  ImageSpacingType  subSpacing;
  ImageSizeType     subSize;

  for(unsigned int i = 0; i < ImageDimension; i++)
  {
    subSpacing[i] = spacing[i] * m_Sampling[i];
    subSize[i]    = static_cast<ImageSizeValueType>(size[i]/m_Sampling[i]);
  }

  {
    ImagePointer gaussCorrImg;
    {
      ImagePointer rawSubsampledImage = Resample( cellImg, subSpacing, subSize, origin );
      gaussCorrImg = GaussianCorrelation( rawSubsampledImage );
      gaussCorrImg = Resample( gaussCorrImg, spacing, size, origin );
    }

		RescaleFilterPointer m_rescale = RescaleFilterType::New();
		m_rescale->SetInput( gaussCorrImg );
		m_rescale->SetOutputMinimum( 0 );
		m_rescale->SetOutputMaximum( 1 );
		m_rescale->Update();

		m_CastGaussFit->SetInput( m_rescale->GetOutput() );
		m_CastGaussFit->Update();
		gauss = m_CastGaussFit->GetOutput();
		gauss->DisconnectPipeline();
	}

	ForegroundPointer fgMap = ForegroundType::New();
	fgMap->SetSpacing( spacing );
	fgMap->SetRegions( cellImg->GetLargestPossibleRegion() );
	fgMap->SetOrigin( origin );
	fgMap->Allocate();

  fgIteratorType fgIt( fgMap, fgMap->GetLargestPossibleRegion() );
  IteratorType It( cellImg, cellImg->GetLargestPossibleRegion() );
  IteratorType Itm( membraneImg, membraneImg->GetLargestPossibleRegion() );
	IteratorType GaussIt( gauss, gauss->GetLargestPossibleRegion() );

  for ( fgIt.GoToBegin(),It.GoToBegin(),GaussIt.GoToBegin();
        !fgIt.IsAtEnd();
        ++fgIt,++It,++GaussIt )
    {
    if ( ( It.Get()  > m_ThresholdCellmax  ) &&
         ( Itm.Get() < m_ThresholdMembrane ) )
      {
      fgIt.Set(true);
      }
    else if ( ( It.Get()      > m_ThresholdCellmin ) &&
              ( GaussIt.Get() > m_ThresholdForm    ) &&
              ( Itm.Get()     < m_ThresholdMembrane ) )
      {
      fgIt.Set(true);
      }
    else
      {
      fgIt.Set(false);
      }
    }

	cellImg = 0;
	membraneImg = 0;

  ForegroundSizeType radius;
  for( unsigned int j = 0; j < ImageDimension; j++ )
  {
    radius[j] = static_cast<ForegroundSizeValueType>(
      0.3*m_LargestCellRadius/spacing[j] );
  }

  SEType sE;
  sE.SetRadius( radius );
  sE.CreateStructuringElement();

	{
		// Close the input volume
		CloseFilterPointer m_close = CloseFilterType::New();
		m_close->SetKernel( sE );
		m_close->SetInput( fgMap );
		m_close->Update();

		fgMap = m_close->GetOutput();
		fgMap->DisconnectPipeline();
	}

	{
		I2LPointer m_fill = I2LType::New();
		m_fill->SetInput( fgMap );
		m_fill->SetFullyConnected( 1 );
		m_fill->SetForegroundValue( 1 );
		m_fill->Update();

		fgMap = m_fill->GetOutput();
		fgMap->DisconnectPipeline();
	}

	{
		ConnectedComponentFilterPointer m_labelFilter =
			ConnectedComponentFilterType::New();
		m_labelFilter->SetInput( fgMap );
		m_labelFilter->Update();

		fgMap = m_labelFilter->GetOutput();
		fgMap->DisconnectPipeline();
	}

	{
		RelabelComponentFilterPointer m_relabelFilter =
			RelabelComponentFilterType::New();
		m_relabelFilter->SetInput( fgMap );
		m_relabelFilter->SetMinimumObjectSize( 300 );
		m_relabelFilter->Update();

		fgMap = m_relabelFilter->GetOutput();
		fgMap->DisconnectPipeline();
  }

  m_CastForeground->SetInput( fgMap );
  m_CastForeground->GraftOutput( this->GetOutput() );
  m_CastForeground->Update();

  this->GraftOutput( m_CastForeground->GetOutput() );
}

template < class TInputImage,class TForegroundImage,class TGaussFitImage >
void
CellForegroundExtraction< TInputImage,TForegroundImage,TGaussFitImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Class Name:        " << GetNameOfClass() << std::endl;
  os << indent << "SigmaForm:         " << GetSigmaForm() << std::endl;
  os << indent << "ThresholdCellmin:  " << GetThresholdCellmin() << std::endl;
  os << indent << "ThresholdCellmax:  " << GetThresholdCellmax() << std::endl;
  os << indent << "ThresholdMembrane: " << GetThresholdMembrane()
    << std::endl;
  os << indent << "ThresholdForm:     " << GetThresholdForm() << std::endl;
  os << indent << "Largest cell radius: " << GetLargestCellRadius() <<
    std::endl;
  os << indent << "Sampling factors:  ";
  for( unsigned int i = 0; i<ImageDimension; i++)
    {
    std::cout << ( this->m_Sampling )[i] << ' ';
    }
  std::cout << std::endl;
}

} /* end namespace itk */

#endif
