#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkShapeLevelSetBasedCellSegmentation.h"
#include "itkNumericSeriesFileNames.h"
#include "itkCastImageFilter.h"
#include <map>
#include <vector>
#include <fstream>
#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " featureImage seedfile cellSegmentImage ";
    std::cerr << "meanShape shapeFormat numberOfPCAModes" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned short, Dimension > CompImage;
  typedef itk::Image< float, Dimension >          FeatureImage;
  typedef itk::ImageFileReader< FeatureImage >    ReaderType;
  typedef itk::ImageFileWriter< CompImage >       WriterType;
  typedef itk::ShapeLevelSetBasedCellSegmentation< FeatureImage,CompImage >
    FilterType;

  ReaderType::Pointer reader_feature = ReaderType::New();
  reader_feature->SetFileName( argv[1] );
  reader_feature->Update();
  FeatureImage::Pointer featureImg = reader_feature->GetOutput();

  typedef FeatureImage::IndexType ImageIndexType;
  ImageIndexType idx;
  float val;
  int numberOfPCAModes = atoi( argv[6] );

  ReaderType::Pointer meanShapeReader = ReaderType::New();
  meanShapeReader->SetFileName( argv[4] );
  meanShapeReader->Update();

  itk::NumericSeriesFileNames::Pointer fileNamesCreator =
    itk::NumericSeriesFileNames::New();
  fileNamesCreator->SetStartIndex( 1 );
  fileNamesCreator->SetEndIndex( numberOfPCAModes );
  fileNamesCreator->SetSeriesFormat( argv[5] );
  const std::vector<std::string> &shapeModeFileNames =
    fileNamesCreator->GetFileNames();

  std::vector< FeatureImage::Pointer > shapeModeImages( numberOfPCAModes );
  for( int k = 0; k < numberOfPCAModes; k++)
  {
    ReaderType::Pointer shapeModeReader = ReaderType::New();
    shapeModeReader->SetFileName(shapeModeFileNames[k].c_str());
    shapeModeReader->Update();
    shapeModeImages[k] = shapeModeReader->GetOutput();
  }

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( featureImg );
  filter->SetLargestCellRadius( 6.0 ); // in real coordinates
  filter->SetSeedValue( 1.5 );
  filter->SetIterations( 1000 );
  filter->SetPropagationScaling( 3 );
  filter->SetCurvatureScaling( 1 );
  filter->SetAdvectionScaling( 3 );
  filter->SetMaxRMSChange( 0.01 );
  filter->SetShapePriorScaling( 2.0 );
  filter->SetNumberOfPCAModes( numberOfPCAModes );

  filter->m_meanShape = meanShapeReader->GetOutput();
  filter->m_shapeModeImages = shapeModeImages;

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
