#include "itkImageFileReader.h"
#include "itkSimpleFilterWatcher.h"
#include "itkBinaryImageToStatisticsLabelMapFilter.h"

int main(int argc, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, dim >    ImageType;
  
  if( argc != 4)
    {
    std::cerr << "usage: " << argv[0] << " input featureImg foreground" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  // read the input image
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  
  // convert the image in a collection of objects and valuate the attributes
  typedef itk::BinaryImageToStatisticsLabelMapFilter< ImageType, ImageType > ConverterType;
  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( reader->GetOutput() );
  converter->SetFeatureImage( reader2->GetOutput() );
  converter->SetForegroundValue( atoi(argv[3]) );
  converter->SetFullyConnected( true );
  itk::SimpleFilterWatcher watcher(converter, "filter");

  // update the statistics filter, so its output will be up to date
  converter->Update();

  converter->GetOutput()->PrintLabelObjects();

  return 0;
}

