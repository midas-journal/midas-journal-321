#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryStatisticsOpeningImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 10 )
    {
    std::cerr << "usage: " << argv[0] << " input input output foreground background lambda reverseOrdering connectivity attribute" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 3;
  
  typedef itk::Image< unsigned char, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  
  typedef itk::BinaryStatisticsOpeningImageFilter< IType, IType > BinaryOpeningType;
  BinaryOpeningType::Pointer opening = BinaryOpeningType::New();
  opening->SetInput( reader->GetOutput() );
  opening->SetFeatureImage( reader2->GetOutput() );
  opening->SetForegroundValue( atoi(argv[4]) );
  opening->SetBackgroundValue( atoi(argv[5]) );
  opening->SetLambda( atof(argv[6]) );
  opening->SetReverseOrdering( atoi(argv[7]) );
  opening->SetFullyConnected( atoi(argv[8]) );
  opening->SetAttribute( argv[9] );
  itk::SimpleFilterWatcher watcher(opening, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( opening->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

