#ifndef __itkVoronoiBasedCellSegmentation_txx
#define __itkVoronoiBasedCellSegmentation_txx

#include "itkVoronoiBasedCellSegmentation.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template < class TSegmentImage,class TVoronoiImage >
VoronoiBasedCellSegmentation< TSegmentImage, TVoronoiImage >
::VoronoiBasedCellSegmentation()
{
  m_CastSegment = SegmentCastType::New();
  m_ROIfilter1 = ROIFilterType::New();
  m_ROIfilter2 = ROIFilterType::New();
  m_CastVoronoi = VoronoiCastType::New();
  m_Map2Image = MapToImageType::New();

  m_MinComponentSize = 2000;

  this->Superclass::SetNumberOfRequiredInputs( 1 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0, TVoronoiImage::New() );
}

template< class TSegmentImage, class TVoronoiImage >
typename VoronoiBasedCellSegmentation< TSegmentImage,TVoronoiImage >
::ImagePointer
VoronoiBasedCellSegmentation< TSegmentImage, TVoronoiImage >
::Threshold
( ImagePointer input, double thresh_low, double thresh_high, double in_value,
double out_value )
{
  typename ThresholdingFilterType::Pointer m_thresholder =
    ThresholdingFilterType::New();
  m_thresholder->SetLowerThreshold( thresh_low );
  m_thresholder->SetUpperThreshold( thresh_high );
  m_thresholder->SetInsideValue( in_value );
  m_thresholder->SetOutsideValue( out_value );
  m_thresholder->SetInput( input );
  m_thresholder->Update();

  ImagePointer thresholdImg = m_thresholder->GetOutput();

  return ( thresholdImg );
}

template< class TSegmentImage, class TVoronoiImage >
typename VoronoiBasedCellSegmentation< TSegmentImage,TVoronoiImage >
::ImagePointer
VoronoiBasedCellSegmentation< TSegmentImage, TVoronoiImage >
::ComputeVoronoi
( ImagePointer roiComp, ImagePointer roiSegment, unsigned short label)
{
  ImagePointer seedSegments = ImageType::New();
  seedSegments->SetRegions( roiComp->GetLargestPossibleRegion() );
  seedSegments->Allocate();
  seedSegments->FillBuffer( 0 );
  seedSegments->SetSpacing( roiComp->GetSpacing() );

  // Check for complete components with no segmentations
  IteratorType cIt( roiComp,roiComp->GetLargestPossibleRegion() );
  IteratorType sIt( roiSegment,roiSegment->GetLargestPossibleRegion() );
  IteratorType seedIt( seedSegments,seedSegments->GetLargestPossibleRegion() );
  for ( cIt.GoToBegin(),sIt.GoToBegin(),seedIt.GoToBegin();
    !cIt.IsAtEnd();++cIt,++sIt,++seedIt )
  {
    if ( cIt.Get() == label )
    {
      seedIt.Set( sIt.Get() );
    }
  }

  // Compute distances within the component alone from segmentations
    typename DistanceFilterType::Pointer m_Dist = DistanceFilterType::New();
    m_Dist->SetInput( seedSegments );
    m_Dist->UseImageSpacingOn();
    m_Dist->UpdateLargestPossibleRegion();

  return ( m_Dist->GetVoronoiMap() );
}

template< class TSegmentImage,class TVoronoiImage >
void
VoronoiBasedCellSegmentation< TSegmentImage,TVoronoiImage >::
GenerateData()
{
  m_CastSegment->SetInput( this->GetInput() );
  m_CastSegment->Update();
  ImagePointer segmentImg = m_CastSegment->GetOutput();

  ImageSizeType size = segmentImg->GetLargestPossibleRegion().GetSize();
  ImageSpacingType spacing = segmentImg->GetSpacing();
  ImagePointType origin = segmentImg->GetOrigin();
  std::cout << "Segment image read..." << std::endl;

  // Allocate a subVoronoi image
  ImagePointer voronoi = ImageType::New();
  voronoi->SetRegions( m_fgMap->GetLargestPossibleRegion() );
  voronoi->Allocate();
  voronoi->FillBuffer( 0 );
  voronoi->SetSpacing( spacing );

  // convert the image in a collection of objects
  typename ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( m_fgMap );
  converter->SetBackgroundValue( 0 );
  // converter->SetForegroundValue( 1 );
  converter->Update();

  ImagePointer roiComp, roiSegment, localVoronoi;
  typename LabelMapType::Pointer labelMap = converter->GetOutput();

  m_Map2Image->SetInput( labelMap );
  m_Map2Image->Update();
  ImagePointer comp = m_Map2Image->GetOutput();

  m_ROIfilter1->SetInput( comp );
  m_ROIfilter2->SetInput( segmentImg );

  for( unsigned short label=1; label<=labelMap->GetNumberOfLabelObjects();
    label++)
  {
    const LabelObjectType *labelObject = labelMap->GetLabelObject( label );

    std::cout << label << std::endl;

    if ( labelObject->Size() > m_MinComponentSize )
    {
      // Determine ROIs of comp region
      m_ROIfilter1->SetRegionOfInterest( labelObject->GetRegion() );
      m_ROIfilter1->Update();
      roiComp = m_ROIfilter1->GetOutput();

      m_ROIfilter2->SetRegionOfInterest( labelObject->GetRegion() );
      m_ROIfilter2->Update();
      roiSegment = m_ROIfilter2->GetOutput();

      localVoronoi = ComputeVoronoi( roiComp,roiSegment,label );

      // Update the voronoi assignment
      IteratorType cIt( roiComp,roiComp->GetLargestPossibleRegion() );
      IteratorType lIt( localVoronoi,localVoronoi->GetLargestPossibleRegion());
      IteratorType vIt( voronoi,labelObject->GetRegion() );
      for ( cIt.GoToBegin(),lIt.GoToBegin(),vIt.GoToBegin();
        !cIt.IsAtEnd();++cIt,++lIt,++vIt )
      {
        if ( cIt.Get() == label )
          vIt.Set( lIt.Get() );
      }
    }
  }
  std::cout << "Generated voronoi map..." << std::endl;

  m_CastVoronoi->SetInput( voronoi );
  m_CastVoronoi->GraftOutput( this->GetOutput() );
  m_CastVoronoi->Update();

  std::cout << "Finished processing..." << std::endl;
  this->GraftOutput( m_CastVoronoi->GetOutput() );
}

template < class TSegmentImage, class TVoronoiImage >
void
VoronoiBasedCellSegmentation< TSegmentImage,TVoronoiImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Class Name:            " << GetNameOfClass() << std::endl;
  os << indent << "Minimum ComponentSize: " << GetMinComponentSize() <<
    std::endl;
}

} /* end namespace itk */

#endif
