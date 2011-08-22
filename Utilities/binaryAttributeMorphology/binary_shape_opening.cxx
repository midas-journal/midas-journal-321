#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryShapeOpeningImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 9 )
    {
    std::cerr << "usage: " << argv[0] << " input output foreground background lambda reverseOrdering connectivity attribute" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef itk::Image< unsigned char, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryShapeOpeningImageFilter< IType > BinaryOpeningType;
  BinaryOpeningType::Pointer opening = BinaryOpeningType::New();
  opening->SetInput( reader->GetOutput() );
  opening->SetForegroundValue( atoi(argv[3]) );
  opening->SetBackgroundValue( atoi(argv[4]) );
  opening->SetLambda( atof(argv[5]) );
  opening->SetReverseOrdering( atoi(argv[6]) );
  opening->SetFullyConnected( atoi(argv[7]) );
  opening->SetAttribute( argv[8] );
  itk::SimpleFilterWatcher watcher(opening, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( opening->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();
  return 0;
}

