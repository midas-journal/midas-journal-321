#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapOverlayImageFilter.h"


int main(int argc, char * argv[])
{
  if( argc != 6 )
    {
    std::cerr << "usage: " << argv[0] << " input input output background opacity" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef itk::Image< unsigned char, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::LabelImageToLabelMapFilter< IType > ConverterType;
  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( reader->GetOutput() );
  converter->SetBackgroundValue( atoi(argv[4]) );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );

//  typedef itk::RGBPixel< unsigned char > RGBPixelType;
//  typedef itk::Image< RGBPixelType, dim > RGBImageType;

  typedef itk::LabelMapOverlayImageFilter< ConverterType::OutputImageType, IType > ColorizerType;
  ColorizerType::Pointer colorizer = ColorizerType::New();
  colorizer->SetInput( converter->GetOutput() );
  colorizer->SetFeatureImage( reader2->GetOutput() );
 colorizer->SetOpacity( atof(argv[5]) );

  itk::SimpleFilterWatcher watcher(colorizer, "filter");

  typedef itk::ImageFileWriter< ColorizerType::OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( colorizer->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

