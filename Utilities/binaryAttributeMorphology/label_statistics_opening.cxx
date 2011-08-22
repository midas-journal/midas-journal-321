#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelStatisticsOpeningImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 8 )
    {
    std::cerr << "usage: " << argv[0] << " input input output background lambda reverseOrdering attribute" << std::endl;
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
  
  typedef itk::LabelStatisticsOpeningImageFilter< IType, IType > LabelOpeningType;
  LabelOpeningType::Pointer opening = LabelOpeningType::New();
  opening->SetInput( reader->GetOutput() );
  opening->SetFeatureImage( reader2->GetOutput() );
  opening->SetBackgroundValue( atoi(argv[4]) );
  opening->SetLambda( atof(argv[5]) );
  opening->SetReverseOrdering( atoi(argv[6]) );
  opening->SetAttribute( argv[7] );
  itk::SimpleFilterWatcher watcher(opening, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( opening->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

