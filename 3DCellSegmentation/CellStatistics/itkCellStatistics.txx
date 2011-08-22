#ifndef __itkCellStatistics_txx
#define __itkCellStatistics_txx

#include "itkCellStatistics.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template < class TSegmentImage,class TOutputImage >
CellStatistics< TSegmentImage, TOutputImage >
::CellStatistics()
{
  m_CastSegment = SegmentCastType::New();
  m_Shape = ShapeFilterType::New();
  m_Relabel = RelabelType::New();
  m_CastOutput = OutputCastType::New();

  this->Superclass::SetNumberOfRequiredInputs( 1 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0, TOutputImage::New() );
}

template< class TSegmentImage,class TOutputImage >
void
CellStatistics< TSegmentImage,TOutputImage >::
GenerateData()
{
  m_CastSegment->SetInput( this->GetInput() );
  m_CastSegment->Update();
  ImagePointer vSegment = m_CastSegment->GetOutput();

  m_Relabel->SetInput( vSegment );
  m_Relabel->SetBackgroundValue( 0 );
  m_Relabel->SetReverseOrdering( false );
  m_Relabel->SetAttribute( "Size" );
  m_Relabel->Update();

 // convert the image into a collection of objects
  typename ShapeConverterType::Pointer shapeConverter =
    ShapeConverterType::New();
  shapeConverter->SetInput( m_Relabel->GetOutput() );
  shapeConverter->SetBackgroundValue( 0 );
  shapeConverter->Update();

  typename StatConverterType::Pointer statConverter =
    StatConverterType::New();
  statConverter->SetInput( m_Relabel->GetOutput() );
  statConverter->SetFeatureImage( m_RawImg );
  statConverter->SetBackgroundValue( 0 );
  statConverter->Update();

  typename ShapeLabelMapType::Pointer shapeLabelMap =
    shapeConverter->GetOutput();
  typename StatLabelMapType::Pointer statLabelMap =
    statConverter->GetOutput();

#ifndef NDEBUG
  std::cout << shapeLabelMap->GetNumberOfLabelObjects() << std::endl;

  for( unsigned short label=1; label<=shapeLabelMap->GetNumberOfLabelObjects();
    label++)
  {
    const ShapeLabelObjectType *shapeObject
      = shapeLabelMap->GetLabelObject( label );

    const StatLabelObjectType *statObject
      = statLabelMap->GetLabelObject( label );

    std::cout << label << std::endl;
    std::cout << shapeObject->Size() << ' ';
    std::cout << shapeObject->GetPhysicalSize() << ' ';
    std::cout << shapeObject->GetBinaryElongation() << std::endl;
    std::cout << shapeObject->GetCentroid() << std::endl;
    std::cout << shapeObject->GetBinaryPrincipalMoments() << std::endl;
    std::cout << shapeObject->GetBinaryPrincipalAxes() << std::endl;
    std::cout << statObject->GetMean() << ' ';
    std::cout << statObject->GetSigma() << ' ';
    std::cout << statObject->GetElongation() << std::endl;
    std::cout << statObject->GetPrincipalMoments() << std::endl;
    std::cout << statObject->GetPrincipalAxes() << std::endl;
  }
#endif

  m_CastOutput->SetInput( m_Relabel->GetOutput() );
  m_CastOutput->GraftOutput( this->GetOutput() );
  m_CastOutput->Update();

#ifndef NDEBUG
  std::cout << "Finished processing..." << std::endl;
#endif

  this->GraftOutput( m_CastOutput->GetOutput() );
}

template < class TSegmentImage, class TOutputImage >
void
CellStatistics< TSegmentImage,TOutputImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Class Name: " << GetNameOfClass( ) << std::endl;
}

} /* end namespace itk */

#endif
