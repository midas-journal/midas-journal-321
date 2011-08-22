#include "itkImageFileReader.h"
#include "itkStatisticsLabelObject.h"
#include "itkLabelMap.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkStatisticsLabelMapFilter.h"
#include "itkTimeProbe.h"
#include "itkSimpleFilterWatcher.h"

#include <iomanip>

int main(int, char * argv[])
{
  const int dim = 3;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, dim >    ImageType;
  
  // read the input image
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef unsigned long LabelType;
  typedef itk::StatisticsLabelObject< LabelType, dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelCollectionType;

  typedef itk::BinaryImageToLabelMapFilter< ImageType, LabelCollectionType > ConverterType;
  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( reader->GetOutput() );
  converter->SetForegroundValue( 200 );
  
  converter->Update();
  
  // to be sure that the converter is not run again at that point
  itk::SimpleFilterWatcher watcher(converter, "converter");

  typedef itk::StatisticsLabelMapFilter< LabelCollectionType, ImageType > StatisticsFilterType;
  StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
  statistics->SetInPlace( false );  // to avoid running the converter each time
  statistics->SetInput( converter->GetOutput() );
  statistics->SetFeatureImage( reader->GetOutput() );
  
  typedef itk::ShapeLabelMapFilter< LabelCollectionType > ShapeFilterType;
  ShapeFilterType::Pointer shape = ShapeFilterType::New();
  shape->SetInPlace( false );  // to avoid running the converter each time
  shape->SetInput( converter->GetOutput() );
  
  typedef itk::InPlaceLabelMapFilter< LabelCollectionType > InPlaceFilterType;
  InPlaceFilterType::Pointer inPlace = InPlaceFilterType::New();
  inPlace->SetInPlace( false );  // to avoid running the converter each time
  inPlace->SetInput( converter->GetOutput() );
  
  typedef itk::LabelMapFilter< LabelCollectionType, LabelCollectionType > EmptyFilterType;
  EmptyFilterType::Pointer empty = EmptyFilterType::New();
  empty->SetInput( converter->GetOutput() );

  std::cout << "#nb" << "\t" 
            << "stats" << "\t"
            << "shape" << "\t"
            << "inPlace" << "\t"
            << "empty" << "\t"
            << std::endl;

  for( int t=1; t<=10; t++ )
    {
    itk::TimeProbe statisticsTime;
    itk::TimeProbe shapeTime;
    itk::TimeProbe inPlaceTime;
    itk::TimeProbe emptyTime;
  
    statistics->SetNumberOfThreads( t );
    shape->SetNumberOfThreads( t );
    inPlace->SetNumberOfThreads( t );
    empty->SetNumberOfThreads( t );
    
    for( int i=0; i<50; i++ )
      {
      statisticsTime.Start();
      statistics->Update();
      statisticsTime.Stop();

      shapeTime.Start();
      shape->Update();
      shapeTime.Stop();

      inPlaceTime.Start();
      inPlace->Update();
      inPlaceTime.Stop();
      
      statistics->Modified();
      shape->Modified();
      inPlace->Modified();
      empty->Modified();
      
      emptyTime.Start();
      empty->Update();
      emptyTime.Stop();
      }
      
    std::cout << std::setprecision(3) << t << "\t" 
              << statisticsTime.GetMeanTime() << "\t"
              << shapeTime.GetMeanTime() << "\t"
              << inPlaceTime.GetMeanTime() << "\t"
              << emptyTime.GetMeanTime() << "\t"
              << std::endl;
    }
  
  
  return 0;
}

