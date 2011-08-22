#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkShapeRelabelImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 6)
    {
    std::cerr << "usage: " << argv[0] << " input output background reverseOrdering attribute" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef itk::Image< unsigned char, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::ShapeRelabelImageFilter< IType > RelabelType;
  RelabelType::Pointer relabel = RelabelType::New();
  relabel->SetInput( reader->GetOutput() );
  relabel->SetBackgroundValue( atoi(argv[3]) );
  relabel->SetReverseOrdering( atoi(argv[4]) );
  relabel->SetAttribute( argv[5] );
  itk::SimpleFilterWatcher watcher(relabel, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( relabel->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();
  return 0;
}

