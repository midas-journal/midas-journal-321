#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCellStatistics.h"
#include "itkCastImageFilter.h"
#include <map>
#include <fstream>
#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " voronoiSegmentImage rawImage segmentImage"
      << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< float, Dimension > RawImageType;
  typedef itk::Image< unsigned short, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType >       ReaderType;
  typedef itk::ImageFileReader< RawImageType >       RawReaderType;
  typedef itk::ImageFileWriter< ImageType >       WriterType;
  typedef itk::CellStatistics< ImageType,ImageType > FilterType;

  ReaderType::Pointer vreader = ReaderType::New();
  vreader->SetFileName( argv[1] );
  vreader->Update();
  ImageType::Pointer voronoi = vreader->GetOutput();

  RawReaderType::Pointer reader_raw = RawReaderType::New();
  reader_raw->SetFileName( argv[2] );
  reader_raw->Update();
  RawImageType::Pointer rawImg = reader_raw->GetOutput();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( voronoi );
  filter->SetRawImage( rawImg );
  filter->Update();

  ImageType::Pointer out = filter->GetOutput();

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
