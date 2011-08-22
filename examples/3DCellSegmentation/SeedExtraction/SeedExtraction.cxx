#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSeedExtraction.h"
#include "itkCastImageFilter.h"
#include <map>
#include <fstream>
#include <stdio.h>

int main( int argc, char* argv[] )
{
  if( argc < 5 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] <<
      " foregroundImage distMap gaussCorrImage seedsFile "
      << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 3;
  typedef itk::Image< bool, Dimension >  fgImage;
  typedef itk::Image< float, Dimension > featureImage;
  typedef itk::ImageFileReader< fgImage > fgReaderType;
  typedef itk::ImageFileReader< featureImage > FeatureReaderType;
  typedef itk::ImageFileWriter< fgImage > WriterType;
  typedef itk::SeedExtraction< featureImage,fgImage > FilterType;

  fgReaderType::Pointer reader_fg = fgReaderType::New();
  reader_fg->SetFileName( argv[1] );
  reader_fg->Update();
  fgImage::Pointer fgMap = reader_fg->GetOutput();

  FeatureReaderType::Pointer reader_dist = FeatureReaderType::New();
  reader_dist->SetFileName( argv[2] );
  reader_dist->Update();
  featureImage::Pointer dist = reader_dist->GetOutput();

  FeatureReaderType::Pointer reader_gf = FeatureReaderType::New();
  reader_gf->SetFileName( argv[3] );
  reader_gf->Update();
  featureImage::Pointer gf = reader_gf->GetOutput();

  FilterType::Pointer filter = FilterType::New();
  filter->SetForeground( fgMap );
  filter->SetInput( 0,dist );
  filter->SetInput( 1,gf );
  filter->SetLargestCellRadius( 4.0 ); // real coordinates
  filter->Update();

  typedef featureImage::IndexType ImageIndexType;
  std::map< float, ImageIndexType > seeds = filter->seeds;
  std::map< float, ImageIndexType >::iterator loc;
  ImageIndexType idx;

  std::fstream outfile;
  outfile.open(argv[4],std::ios::out);

  loc = seeds.end();
  --loc;
  while(loc != seeds.begin() )
  {
    --loc;
    float val = loc->first;
    idx = loc->second;
    outfile << val;
    for( unsigned int i = 0; i < Dimension; i++)
    {
      outfile << ' ' << idx[i];
    }
    outfile << std::endl;
  }
  outfile.close();
/*
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[5] );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }
*/
  return EXIT_SUCCESS;
}
