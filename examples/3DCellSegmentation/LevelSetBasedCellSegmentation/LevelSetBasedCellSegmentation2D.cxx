#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLevelSetBasedCellSegmentation.h"
#include "itkCastImageFilter.h"
#include <map>
#include <fstream>
#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " featureImage seedfile cellSegmentImage"
      << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;
  typedef itk::Image< unsigned short, Dimension > CompImage;
  typedef itk::Image< float, Dimension >          FeatureImage;
  typedef itk::ImageFileReader< FeatureImage >    ReaderType;
  typedef itk::ImageFileWriter< CompImage >       WriterType;
  typedef itk::LevelSetBasedCellSegmentation< FeatureImage,CompImage >
    FilterType;

  ReaderType::Pointer reader_feature = ReaderType::New();
  reader_feature->SetFileName( argv[1] );
  reader_feature->Update();
  FeatureImage::Pointer featureImg = reader_feature->GetOutput();

  typedef FeatureImage::IndexType ImageIndexType;
  ImageIndexType idx;
  float val;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( featureImg );
  filter->SetLargestCellRadius( 5.0 ); // in real coordinates
  filter->SetSeedValue( 1.5 );
  filter->SetIterations( 500 );
  filter->SetPropagationScaling( 2 );
  filter->SetCurvatureScaling( 1 );
  filter->SetAdvectionScaling( 4 );
  filter->SetMaxRMSChange( 0.01 );

  std::fstream infile;
  infile.open(argv[2],std::ios::in);

  while( !infile.eof() )
  {
    infile >> val;
    for( unsigned int i = 0; i < Dimension; i++)
    {
      infile >> idx[i];
    }
    filter->seeds[val] = static_cast<ImageIndexType>( idx );
  }
  infile.close();

#ifndef NDEBUG
  std::cout << filter << std::endl;
#endif

  filter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[3] );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }

  return EXIT_SUCCESS;
}
