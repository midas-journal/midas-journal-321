#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCellFeatureGenerator.h"
#include "itkRescaleIntensityImageFilter.h"

int main( int argc, char* argv[] )
{
  if( argc < 6 )
    {
      std::cerr << "Usage: " << std::endl;
      std::cerr << argv[0] << " rawImage foregroundImage gaussCorrImage ";
      std::cerr << "featureImage distanceMap " << std::endl;
      return EXIT_FAILURE;
    }

  const int Dimension = 3;
  typedef itk::Image< float, Dimension > InputImage;
  typedef itk::Image< float, Dimension > FeatureImage;
  typedef itk::Image< bool, Dimension > FgImage;

  typedef itk::ImageFileReader< InputImage > ReaderType;
  typedef itk::ImageFileReader< FgImage > FgReaderType;
  typedef itk::ImageFileWriter< FeatureImage > WriterType;
  typedef itk::CellFeatureGenerator< InputImage,FeatureImage > FilterType;
  typedef itk::RescaleIntensityImageFilter< InputImage,InputImage >
    RescaleType;

	InputImage::Pointer rawImg, gaussCorrImg;
	FgImage::Pointer fgMap;
	{
		ReaderType::Pointer reader_raw = ReaderType::New();
		reader_raw->SetFileName( argv[1] );
		reader_raw->Update();
		rawImg = reader_raw->GetOutput();
		rawImg->DisconnectPipeline();
		
		FgReaderType::Pointer reader_fg = FgReaderType::New();
		reader_fg->SetFileName( argv[2] );
		reader_fg->Update();
		fgMap = reader_fg->GetOutput();
		fgMap->DisconnectPipeline();
		rawImg->SetSpacing( fgMap->GetSpacing() );
		
		ReaderType::Pointer reader_gauss = ReaderType::New();
		reader_gauss->SetFileName( argv[3] );
		reader_gauss->Update();
		gaussCorrImg = reader_gauss->GetOutput();
		gaussCorrImg->DisconnectPipeline();
	}

	float sampling[Dimension] = { 1, 1, 1};

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( 0,rawImg );
  filter->SetInput( 1,gaussCorrImg );
  filter->SetForeground( fgMap );
  filter->SetLargestCellRadius( 6.0 );
  filter->SetSigmaCell( 0.4 );
  filter->SetSigmaCorrelation( 0.4 );
	filter->SetSampling( sampling );
  filter->SetDistanceMapWeight( 0.5 );
  filter->SetGradientMagnitudeWeight( 0.5 );
  filter->SetGaussCorrWeight( 0.0 );
  filter->Update();

  RescaleType::Pointer rescale_f = RescaleType::New();
  rescale_f->SetInput( filter->GetOutput() );
  rescale_f->SetOutputMinimum( 0 );
  rescale_f->SetOutputMaximum( 255 );
  rescale_f->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( rescale_f->GetOutput() );
  writer->SetFileName( argv[4] );

  WriterType::Pointer dwriter = WriterType::New();
  dwriter->SetInput( filter->GetDistanceMap() );
  dwriter->SetFileName( argv[5] );

  try
    {
    writer->Update();
    dwriter->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }

  return EXIT_SUCCESS;
}
