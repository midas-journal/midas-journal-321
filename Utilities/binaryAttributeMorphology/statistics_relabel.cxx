#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkStatisticsRelabelImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 8 )
    {
    std::cerr << "usage: " << argv[0] << " input input output background useBg reverseOrdering attribute" << std::endl;
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
  
  typedef itk::StatisticsRelabelImageFilter< IType, IType > RelabelType;
  RelabelType::Pointer relabel = RelabelType::New();
  relabel->SetInput( reader->GetOutput() );
  relabel->SetFeatureImage( reader2->GetOutput() );
  relabel->SetBackgroundValue( atoi(argv[4]) );
//  relabel->SetUseBackground( atoi(argv[5]) );
  relabel->SetReverseOrdering( atoi(argv[6]) );
  relabel->SetAttribute( argv[7] );
  itk::SimpleFilterWatcher watcher(relabel, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( relabel->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

