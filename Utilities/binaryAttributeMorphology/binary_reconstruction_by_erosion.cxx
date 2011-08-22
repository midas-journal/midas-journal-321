#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkBinaryReconstructionByErosionImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 7 )
    {
    std::cerr << "usage: " << argv[0] << " marker mask output conn fg bg" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 3;
  
  typedef itk::Image< unsigned char, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();
  
 typedef itk::BinaryReconstructionByErosionImageFilter< IType > I2LType;
  I2LType::Pointer reconstruction = I2LType::New();
  reconstruction->SetMarkerImage( reader->GetOutput() );
  reconstruction->SetMaskImage( reader2->GetOutput() );
  reconstruction->SetFullyConnected( atoi(argv[4]) );
  reconstruction->SetForegroundValue( atoi(argv[5]) );
  reconstruction->SetBackgroundValue( atoi(argv[6]) );
  itk::SimpleFilterWatcher watcher(reconstruction, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( reconstruction->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

