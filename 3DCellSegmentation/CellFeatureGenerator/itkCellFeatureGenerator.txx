#ifndef __itkCellFeatureGenerator_txx
#define __itkCellFeatureGenerator_txx

#include "itkCellFeatureGenerator.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template < class TInputImage,class TFeatureImage >
CellFeatureGenerator< TInputImage, TFeatureImage >
::CellFeatureGenerator()
{
  m_SigmaCell = 0.2;
  m_SigmaCorrelation = 0.2;
  m_LargestCellRadius = 4.0;
  m_DistanceMapWeight = 1.0;
  m_GradientMagnitudeWeight = 0.0;
  m_GaussCorrWeight = 0.0;
  fgMap = NULL;
  m_Sampling = NULL;

  this->Superclass::SetNumberOfRequiredInputs( 2 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0, TFeatureImage::New() );
}

template< class TInputImage,class TFeatureImage >
void
CellFeatureGenerator< TInputImage,TFeatureImage >
::DistanceMap
( FgImagePointer input )
{
  MaurerPointer m_Maurer = MaurerType::New();
  m_Maurer->SetInput( input );
  m_Maurer->SetSquaredDistance( 0 );
  m_Maurer->SetUseImageSpacing( 1 );
  m_Maurer->SetInsideIsPositive( 0 );
  m_Maurer->Update();

  AbsFilterPointer  m_absFilter = AbsFilterType::New();
  m_absFilter->SetInput( m_Maurer->GetOutput() );
  m_absFilter->Update();

  ThreshFilterPointer m_thresh = ThreshFilterType::New();
  m_thresh->SetInput( m_absFilter->GetOutput() );
  m_thresh->ThresholdAbove( m_LargestCellRadius );
  m_thresh->SetOutsideValue( m_LargestCellRadius );
  m_thresh->Update();

  dist = m_thresh->GetOutput();
}

template< class TInputImage,class TFeatureImage >
typename CellFeatureGenerator< TInputImage,TFeatureImage >::ImagePointer
CellFeatureGenerator< TInputImage,TFeatureImage >
::FeatureImage
( ImagePointer dist )
{
  ImagePointer featureImg = ImageType::New();
  featureImg->SetRegions( dist->GetLargestPossibleRegion() );
  featureImg->SetSpacing( dist->GetSpacing() );
  featureImg->SetOrigin( dist->GetOrigin() );
  featureImg->Allocate();
  featureImg->FillBuffer(0);

  float sum = m_DistanceMapWeight + m_GradientMagnitudeWeight;
  ImageIndexType idx;
  ImagePixelType pdist,pfg,pgradient,pfeature;
  IndexIteratorType indexIt(
    featureImg,featureImg->GetLargestPossibleRegion());
  for( indexIt.GoToBegin();!indexIt.IsAtEnd();++indexIt )
  {
    idx = indexIt.GetIndex();
    pdist = dist->GetPixel( idx );
    pgradient = grad->GetPixel( idx );
    pfg = fgMap->GetPixel( idx );

    pfeature =
      ( pdist*m_DistanceMapWeight + pgradient*m_GradientMagnitudeWeight )/sum;

    indexIt.Set( pfeature );
  }
  return featureImg;
}

template< class TInputImage,class TFeatureImage >
typename CellFeatureGenerator< TInputImage,TFeatureImage >::ImagePointer
CellFeatureGenerator< TInputImage,TFeatureImage >
::InputCast
( unsigned int i)
{
  InputCastPointer m_CastInput = InputCastType::New();
  m_CastInput->SetInput( this->GetInput( i ) );
  m_CastInput->Update();

  return ( m_CastInput->GetOutput() );
}

template< class TInputImage,class TFeatureImage >
typename CellFeatureGenerator< TInputImage,TFeatureImage >::ImagePointer
CellFeatureGenerator< TInputImage,TFeatureImage >
::Resample
(ImagePointer input, ImageSpacingType spacing, ImageSizeType size,
ImagePointType origin)
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

template< class TInputImage,class TFeatureImage >
void
CellFeatureGenerator< TInputImage,TFeatureImage >
::Normalize
( ImagePointer grad1, ImagePointer grad2 )
{
  ImagePointer grad3 = ImageType::New();
  grad3->SetRegions( grad1->GetLargestPossibleRegion() );
  grad3->Allocate();
  grad3->FillBuffer( 0 );
  grad3->SetSpacing( grad1->GetSpacing() );

  IteratorType It1( grad1, grad1->GetLargestPossibleRegion() );
  IteratorType It2( grad2, grad2->GetLargestPossibleRegion() );
  IteratorType It( grad3, grad3->GetLargestPossibleRegion() );

  double p = m_GaussCorrWeight/( 1 + m_GaussCorrWeight );

  for( It.GoToBegin(),It1.GoToBegin(),It2.GoToBegin(); !It1.IsAtEnd() ;
    ++It1, ++It2, ++It )
  {
      It.Set( sqrt( It1.Get()*It1.Get() + p*It2.Get()*It2.Get() ) );
  }

  RescalePointer m_rescale_grad = RescaleType::New();
  m_rescale_grad->SetInput( grad3 );
  m_rescale_grad->SetOutputMinimum( 0 );
  m_rescale_grad->SetOutputMaximum( 1 );
  m_rescale_grad->Update();

  grad = m_rescale_grad->GetOutput();
}

template< class TInputImage,class TFeatureImage >
typename CellFeatureGenerator< TInputImage,TFeatureImage >::ImagePointer
CellFeatureGenerator< TInputImage,TFeatureImage >
::Gradient
( ImagePointer input, double sigma )
{
 // Compute the gradient magnitude
  GradientFilterPointer m_gradientMagnitude = GradientFilterType::New();
  m_gradientMagnitude->SetInput( input );
  m_gradientMagnitude->SetSigma( sigma );
  //m_gradientMagnitude->SetUseImageSpacingOn();
  m_gradientMagnitude->Update();

  ImagePointer gradient = m_gradientMagnitude->GetOutput();

  IteratorType distIt( dist,dist->GetLargestPossibleRegion() );
  IteratorType gradientIt( gradient,gradient->GetLargestPossibleRegion() );

  double  alpha = -1, beta = 0;
  float k1 = 0, k2 = 0;
  int n1 = 0, n2 = 0;
  for( distIt.GoToBegin(),gradientIt.GoToBegin(); !distIt.IsAtEnd() ; ++distIt,
    ++gradientIt )
  {
    if ( distIt.Get() < m_LargestCellRadius/3 )
    {
      k1 += gradientIt.Get();
      ++n1;
    }
    else
    {
      k2 += gradientIt.Get();
      ++n2;
    }
  }

  if ( (n1>0) && (n2>0) )
  {
    k1 = k1/static_cast<float>(n1);
    k2 = k2/static_cast<float>(n2);
    alpha = (k2-k1)/2;
    beta = (k2+k1)/2;
  }
  std::cout << k1 << ' ' << k2 << std::endl;
  std::cout << "Estimating Alpha and Beta complete..." << std::endl;
  std::cout << alpha << ' ' << beta << std::endl;

  SigmoidFilterPointer m_sigmoid = SigmoidFilterType::New();
  m_sigmoid->SetInput( m_gradientMagnitude->GetOutput() );
  m_sigmoid->SetOutputMinimum( 0.0 );
  m_sigmoid->SetOutputMaximum( 1 );
  m_sigmoid->SetAlpha( alpha );
  m_sigmoid->SetBeta( beta );
  m_sigmoid->Update();

  return( m_sigmoid->GetOutput() );
}

template< class TInputImage,class TFeatureImage >
void
CellFeatureGenerator<TInputImage,TFeatureImage >::
GenerateData()
{
  std::cout << "Begin processing..." << std::endl;

  DistanceMap( fgMap );
  std::cout << "Distance map computation complete..." << std::endl;

  ImagePointer rawImg = InputCast( 0 );
  ImagePointer grad_raw = Gradient( rawImg,m_SigmaCell );
  grad_raw->SetSpacing( rawImg->GetSpacing() );
  rawImg = 0;
  std::cout << "Raw gradient computation complete..." << std::endl;

  ImagePointer gaussCorrImg = InputCast( 1 );
  ImagePointer grad_gauss = Gradient( gaussCorrImg,m_SigmaCorrelation );
  grad_gauss->SetSpacing( grad_raw->GetSpacing() );
  gaussCorrImg = 0;
  std::cout << "Gradient computation complete..." << std::endl;

  // Combine normalized gradients here
  Normalize( grad_raw, grad_gauss );
  grad_raw = 0;
  grad_gauss = 0;

  ImagePointer featureImg;
  {
    RescalePointer m_rescale = RescaleType::New();
    m_rescale->SetInput( dist );
    m_rescale->SetOutputMinimum( 0 );
    m_rescale->SetOutputMaximum( 1 );
    m_rescale->Update();

    featureImg = FeatureImage( m_rescale->GetOutput() );
    std::cout << "Computed feature image..." << std::endl;
  }

  FeatureCastPointer m_CastFeature = FeatureCastType::New();
  m_CastFeature->SetInput( featureImg );
  m_CastFeature->GraftOutput( this->GetOutput() );
  m_CastFeature->Update();

  std::cout << "Finished processing..." << std::endl;
  this->GraftOutput( m_CastFeature->GetOutput() );
}

template < class TInputImage, class TFeatureImage >
void
CellFeatureGenerator< TInputImage,TFeatureImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Class Name:              " << this->GetNameOfClass( ) << 
    std::endl;
  os << indent << "Sigma Cell:              " << this->GetSigmaCell() << std::endl;
  os << indent << "Sigma COrrelation:       " << this->GetSigmaCorrelation() << 
    std::endl;
  os << indent << "Largest Cell Radius:     " << this->GetLargestCellRadius() << 
    std::endl;
  os << indent << "DistanceMapWeight:       " << this->GetDistanceMapWeight() <<
    std::endl;
  os << indent << "GradientMagnitudeWeight: " <<this->GetGradientMagnitudeWeight()
    << std::endl;
  os << indent << "GaussCorrWeight:         " << this->GetGaussCorrWeight() << 
    std::endl;
}

} /* end namespace itk */

#endif
