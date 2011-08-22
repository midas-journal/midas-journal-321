#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelReconstructionByDilationImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 5 )
    {
    std::cerr << "usage: " << argv[0] << " input mask output bg" << std::endl;
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
  
  typedef itk::LabelReconstructionByDilationImageFilter< IType > I2LType;
  I2LType::Pointer reconstruction = I2LType::New();
  reconstruction->SetMarkerImage( reader->GetOutput() );
  reconstruction->SetMaskImage( reader2->GetOutput() );
  reconstruction->SetBackgroundValue( atoi(argv[4]) );
  itk::SimpleFilterWatcher watcher(reconstruction, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( reconstruction->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();
  return 0;
}

