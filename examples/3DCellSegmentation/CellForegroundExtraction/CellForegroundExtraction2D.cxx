 #include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCellForegroundExtraction.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"

int main( int argc, char* argv[] )
{
  if( argc < 5 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " cellStain membraneStain foregroundImage";
    std::cerr << " GaussFitImage " << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 2;
  typedef itk::Image< float,Dimension > InputImage;
  typedef itk::Image< unsigned short,Dimension > fgImage;
  typedef itk::Image< float,Dimension > gfImage;

  typedef itk::ImageFileReader< InputImage > ReaderType;
  typedef itk::ImageFileWriter< fgImage > fgWriterType;
  typedef itk::ImageFileWriter< gfImage > gfWriterType;

  typedef itk::CellForegroundExtraction< InputImage, fgImage, gfImage >
    FilterType;

	InputImage::Pointer cellImg, membraneImg;

	{
		ReaderType::Pointer cellReader = ReaderType::New();
		cellReader->SetFileName( argv[1] );
		cellReader->Update();
		cellImg = cellReader->GetOutput();
		cellImg->DisconnectPipeline();

		ReaderType::Pointer membraneReader = ReaderType::New();
		membraneReader->SetFileName( argv[2] );
		membraneReader->Update();
		membraneImg = membraneReader->GetOutput();
		membraneImg->DisconnectPipeline();
	}

  InputImage::SpacingType spacing = cellImg->GetSpacing();
  std::cout << "Spacing: " << spacing[0] << ' ' << spacing[1] <<
    ' ' << std::endl;

  spacing[0] = spacing[1] = 0.2;
  cellImg->SetSpacing( spacing );
  membraneImg->SetSpacing( spacing );

  float sampling[Dimension] = {2,2};//{5,5,1};

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( 0, cellImg );
  filter->SetInput( 1, membraneImg );
  filter->SetSigmaForm( 2.0 ); // in real coordinates
  filter->SetThresholdCellmin( 70 );
  filter->SetThresholdCellmax( 120 );
  filter->SetThresholdMembrane( 255 );
  filter->SetThresholdForm( 0.5 );
  filter->SetLargestCellRadius( 4.0 ); // in real coordinates
  filter->SetSampling( sampling );
#ifndef NDEBUG
  std::cout << filter << std::endl;
#endif
  filter->Update();

  fgWriterType::Pointer writer_fg = fgWriterType::New();
  writer_fg->SetInput( filter->GetOutput() );
  writer_fg->SetFileName( argv[3] );

  try
  {
    writer_fg->Update();
  }
  catch ( itk::ExceptionObject e )
  {
    std::cerr << "Error: " << e << std::endl;
  }

  gfWriterType::Pointer writer_gf = gfWriterType::New();
  writer_gf->SetInput( filter->GetGaussCorrImage() );
  writer_gf->SetFileName( argv[4] );

  try
  {
    writer_gf->Update();
  }
  catch ( itk::ExceptionObject e )
  {
    std::cerr << "Error: " << e << std::endl;
  }

  return EXIT_SUCCESS;
}
