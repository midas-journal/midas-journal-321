#include "itkImageFileReader.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkPoint.h"
#include "itkMatrix.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkRigid3DTransform.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkVector.h"
#include "itkImagePCAShapeModelEstimator.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageFileWriter.h"


int main( int argc, char* argv[] )
{
  if( argc < 3 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " valImg roiImageFormat"
      << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned short, Dimension > ImageType;
  typedef itk::Image< float, Dimension > FloatImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef unsigned char LabelType;
  typedef itk::ShapeLabelObject< LabelType, Dimension > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::LabelImageToShapeLabelMapFilter< ImageType, LabelMapType >
    ConverterType;
  typedef itk::NearestNeighborInterpolateImageFunction< ImageType >
    Interpolator;
  typedef itk::Rigid3DTransform< double > TransformType;
  typedef itk::Matrix< double, Dimension, Dimension > MatrixType;
  typedef itk::RegionOfInterestImageFilter<ImageType,ImageType> ROIFilterType;
  typedef itk::ResampleImageFilter< ImageType, ImageType>
    ResampleFilterType;
  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  typedef itk::ImageRegionIteratorWithIndex< ImageType > IndexIteratorType;
  typedef itk::ImageRegionIterator< FloatImageType > FloatIteratorType;
  typedef itk::SignedDanielssonDistanceMapImageFilter< ImageType,FloatImageType
    > DistanceFilterType;
  typedef itk::ImagePCAShapeModelEstimator< FloatImageType, FloatImageType >
    EstimatorType;
  typedef itk::NumericSeriesFileNames NameGeneratorType;
  typedef itk::ImageFileWriter< ImageType > WriterType;


  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  ImageType::Pointer valImg = reader->GetOutput();
  ImageType::SpacingType spacing = valImg->GetSpacing();
  ImageType::SizeType inputSize = valImg->GetLargestPossibleRegion().GetSize();

  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( valImg );
  converter->SetBackgroundValue( 0 );
  converter->Update();
  LabelMapType::Pointer labelMap = converter->GetOutput();

  std::cout << labelMap->GetNumberOfLabelObjects() << std::endl;

  ImageType::SizeType size;
  for(unsigned int i = 0; i < Dimension; i++)
  {
    size[i] = static_cast<ImageType::SizeType::SizeValueType>(12.0/spacing[i]);
  }

  Interpolator::Pointer interp = Interpolator::New();

  ROIFilterType::Pointer ROIfilter = ROIFilterType::New();
  ROIfilter->SetInput( valImg );

  ImageType::Pointer roiComp;
  ImageType::IndexType index;
  itk::Point< double, Dimension > center;
  itk::Point<double, Dimension> centroid;
//   unsigned short count = 0;

  std::vector<FloatImageType::Pointer>
    shapes( labelMap->GetNumberOfLabelObjects() );


  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  nameGenerator->SetSeriesFormat( argv[2] );
  nameGenerator->SetStartIndex( 1 );
  nameGenerator->SetEndIndex( 8 );
  nameGenerator->SetIncrementIndex( 1 );

WriterType::Pointer writer = WriterType::New();

  for( unsigned short label=1; label<=labelMap->GetNumberOfLabelObjects();
    label++)
  {
    const LabelObjectType *labelObject = labelMap->GetLabelObject( label );
    std::cout << label << std::endl;

    // centroid is in physical coordinates
    centroid = labelObject->GetCentroid();
    bool flag = true;
    for(unsigned int i = 0; i < Dimension; i++)
    {
      index[i] = static_cast<int>(centroid[i]/spacing[i] - size[i]/2);
      if ( (index[i] < 0) || ( index[i] + size[i] > inputSize[i] ) )
        flag = false;
      center[i] = static_cast<double>( size[i]/2 );
    }

    if (flag)
    {
      ImageType::RegionType bboxregion;
      bboxregion.SetIndex( index );
      bboxregion.SetSize( size );

      std::cout << centroid << std::endl;
      std::cout << bboxregion << std::endl;

      MatrixType rotationMatrix = labelObject->GetBinaryPrincipalAxes();

      // Transform needs to be in global coordinates
      TransformType::Pointer transform = TransformType::New();
      transform->SetRotationMatrix(
        static_cast<MatrixType>(rotationMatrix.GetInverse() ) );
      transform->SetCenter( centroid );

      // Origin of roiComp is in global coordinates but image index is local
      ROIfilter->SetRegionOfInterest( bboxregion );
      ROIfilter->Update();
      ImageType::Pointer roiComp = ROIfilter->GetOutput();

      IteratorType It( roiComp, roiComp->GetLargestPossibleRegion() );
      for(It.GoToBegin();!It.IsAtEnd();++It)
      {
        if ( ( It.Get()!=0 ) && ( It.Get()!=label ) )
          It.Set( 0 );
      }

      ResampleFilterType::Pointer resample = ResampleFilterType::New();
      resample->SetInterpolator( interp );
      resample->SetOutputSpacing( spacing );
      resample->SetOutputOrigin( roiComp->GetOrigin() );
      resample->SetSize( size );
      resample->SetDefaultPixelValue( 0 );
      resample->SetInput( roiComp );
      resample->SetTransform( transform );
      resample->Update();

      writer->SetInput( resample->GetOutput() );
      writer->SetFileName( nameGenerator->GetFileNames()[label-1] );
      writer->Update();

//       DistanceFilterType::Pointer dist = DistanceFilterType::New();
//       dist->SetInput( resample->GetOutput() );
//       dist->SetUseImageSpacing( 1 );
//       dist->Update();
//
//       shapes[count++] = dist->GetOutput();
    }
  }
/*
  unsigned int numOfPrinComp = 2;
  std::cout << count << std::endl;

  EstimatorType::Pointer shapeEstimator = EstimatorType::New();
  shapeEstimator->SetNumberOfTrainingImages( count );
  shapeEstimator->SetNumberOfPrincipalComponentsRequired( numOfPrinComp );

  for ( unsigned int i = 0 ; i < count; i++ )
    shapeEstimator->SetInput( i , shapes[i] );
  shapeEstimator->UpdateLargestPossibleRegion();

  std::cout << shapeEstimator->GetEigenValues() << std::endl;
  itk::Array< double > eigen( labelMap->GetNumberOfLabelObjects() - 1 );
  eigen = shapeEstimator->GetEigenValues();

  NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
  nameGenerator->SetSeriesFormat( argv[2] );
  nameGenerator->SetStartIndex( 0 );
  nameGenerator->SetEndIndex( numOfPrinComp );
  nameGenerator->SetIncrementIndex( 1 );

  if ( count > 0 )
  {
    WriterType::Pointer writer = WriterType::New();
    for ( unsigned int i = 0 ; i < numOfPrinComp+1; i++ )
    {
      FloatImageType::Pointer output = shapeEstimator->GetOutput(i);
      if ( i > 0 )
      {
        FloatIteratorType fIt( output, output->GetLargestPossibleRegion() );
        for(fIt.GoToBegin();!fIt.IsAtEnd();++fIt)
          fIt.Set( static_cast<float> ( eigen[i-1] * fIt.Get() ) );
      }
      writer->SetInput( output );
      writer->SetFileName( nameGenerator->GetFileNames()[i] );
      writer->Update();
    }
  }
*/
  return EXIT_SUCCESS;
}
