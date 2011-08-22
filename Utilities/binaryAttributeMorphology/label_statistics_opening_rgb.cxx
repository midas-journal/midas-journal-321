#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"
#include "itkLabelToRGBImageFilter.h"

#include "itkLabelStatisticsOpeningImageFilter.h"
#include "itkMultiThreader.h"


int main(int argc, char * argv[])
{

  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);

  if( argc != 8 )
    {
    std::cerr << "usage: " << argv[0] << " input input output background lambda reverseOrdering attribute" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
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

  typedef itk::RGBPixel< unsigned char > RGBPixelType;
  typedef itk::Image< RGBPixelType, dim > RGBImageType;

  typedef itk::LabelToRGBImageFilter< IType, RGBImageType > ColorizerType;
  ColorizerType::Pointer colorizer = ColorizerType::New();
  colorizer->SetInput( opening->GetOutput() );

  typedef itk::ImageFileWriter< RGBImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( colorizer->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

