#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVoronoiBasedCellSegmentation.h"
#include "itkCastImageFilter.h"
#include <map>
#include <fstream>
#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " fgImage segmentImage voronoiSegmentImage"
      << std::endl;
    return EXIT_FAILURE;
    }

  const int Dimension = 2;
  typedef itk::Image< unsigned short, Dimension > ImageType;
	typedef itk::Image< bool, Dimension > FgImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileReader< FgImageType > FgReaderType;
	typedef itk::ImageFileWriter< ImageType > WriterType;
  typedef itk::VoronoiBasedCellSegmentation< ImageType,ImageType >
    FilterType;

  FgReaderType::Pointer reader_fg = FgReaderType::New();
  reader_fg->SetFileName( argv[1] );
  reader_fg->Update();
  FgImageType::Pointer fgMap = reader_fg->GetOutput();

  ReaderType::Pointer reader_segment = ReaderType::New();
  reader_segment->SetFileName( argv[2] );
  reader_segment->Update();
  ImageType::Pointer segmentImg = reader_segment->GetOutput();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( segmentImg );
  filter->SetForegroundImage( fgMap );
  filter->SetMinComponentSize( 400 );
#ifndef NDEBUG
  std::cout << filter << std::endl;
#endif
  filter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[3] );

  try
    {
      writer->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }

  return EXIT_SUCCESS;
}
