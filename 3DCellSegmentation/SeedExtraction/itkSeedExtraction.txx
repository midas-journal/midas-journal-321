#ifndef __itkSeedExtraction_txx
#define __itkSeedExtraction_txx

#include "itkSeedExtraction.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template < class TFeatureImage, class TOutputImage >
SeedExtraction< TFeatureImage, TOutputImage >
::SeedExtraction()
{
  m_CastOutput = OutputCastType::New();

  m_LargestCellRadius = 5;

  this->Superclass::SetNumberOfRequiredInputs( 2 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0, TOutputImage::New() );
}

// This function determines the voxel locations (or seeds) interior to a
// cells/nuclei. The level-set is initialized based on these seeds.
// The input volume is a distance-field volume. fg is the cell foreground mask.
// sample is a list where all the seed locations are stored.
template< class TFeatureImage, class TOutputImage >
void
SeedExtraction< TFeatureImage, TOutputImage >
::ImageSeeds(  ImagePointer featureImg )
{
  // Smooth the distance field to remove out small islands
  typename GaussianFilterType::Pointer m_smoother = GaussianFilterType::New();
  m_smoother->SetInput( featureImg );
  m_smoother->SetVariance( 0.1 );
  m_smoother->SetMaximumKernelWidth(3);
  m_smoother->Update();
  ImagePointer smoothImage = m_smoother->GetOutput();

  // Specify the dilation radius of the structuring mask. It correlated with the
  // minimum separation between the nuclei.
  ImageSpacingType spacing = featureImg->GetSpacing();
  ImageSizeType radius;
  for( unsigned int j = 0; j < ImageDimension; j++ )
  {
    radius[j] = static_cast<ImageSizeValueType>(
      0.3*m_LargestCellRadius/spacing[j] );
    std::cout << radius[j] << ' ';
  }

  SEType sE;
  sE.SetRadius( radius );
  sE.CreateStructuringElement();

  // Dilate the input volume
  typename DilateFilterType::Pointer grayscaleDilate = DilateFilterType::New();
  grayscaleDilate->SetKernel( sE );
  grayscaleDilate->SetInput( smoothImage );
  grayscaleDilate->Update();
  ImagePointer dilateImage = grayscaleDilate->GetOutput();
  std::cout << "Dilation complete..." << std::endl;

  ImageIndexType idx;
  ImagePixelType pixelSmooth,pixelDilate;
  fgImagePixelType pixelMask;

  // Iterate over the smooth distance field
  IndexIteratorType It( smoothImage,smoothImage->GetRequestedRegion() );

  for (It.GoToBegin();!It.IsAtEnd();++It)
  {
    idx = It.GetIndex();
    pixelSmooth = smoothImage->GetPixel(idx);
    pixelDilate = dilateImage->GetPixel(idx);
    pixelMask = m_fgMap->GetPixel( idx );

    // If the pixel after dilation has the same value prior to dilation, it is a
    //local maxima! Make sure it lies in the cell foreground.
    if ( ( pixelSmooth == pixelDilate ) && ( pixelMask > 0 ) &&
      ( pixelSmooth > 0.3*m_LargestCellRadius ) )
    {
      seeds[pixelSmooth] = idx;
    }
  }
}

template< class TFeatureImage, class TOutputImage >
typename SeedExtraction< TFeatureImage, TOutputImage >::ImagePointer
SeedExtraction< TFeatureImage, TOutputImage >
::InputCast
( unsigned int i)
{
  typename FeatureCastType::Pointer m_CastFeature = FeatureCastType::New();
  m_CastFeature->SetInput( this->GetInput( i ) );
  m_CastFeature->Update();

  return ( m_CastFeature->GetOutput() );
}

template< class TFeatureImage, class TOutputImage >
void
SeedExtraction< TFeatureImage, TOutputImage >::
GenerateData()
{

  ImagePointer dist = InputCast( 0 );
  ImagePointer gf = InputCast( 1 );
  std::cout << "Input images read..." << std::endl;

  ImageSeeds( dist );
//   ImageSeeds( gf );
  std::cout << "Size: " << seeds.size() << std::endl;

  m_CastOutput->SetInput( m_fgMap );
  m_CastOutput->GraftOutput( this->GetOutput() );
  m_CastOutput->Update();

  std::cout << "Finished processing..." << std::endl;
  this->GraftOutput( m_CastOutput->GetOutput() );
}


template < class TFeatureImage, class TOutputImage >
void
SeedExtraction< TFeatureImage, TOutputImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Class Name:          " << GetNameOfClass( ) << std::endl;
  os << indent << "Largest Cell Radius: " << GetLargestCellRadius() <<
    std::endl;
}

} /* end namespace itk */

#endif
