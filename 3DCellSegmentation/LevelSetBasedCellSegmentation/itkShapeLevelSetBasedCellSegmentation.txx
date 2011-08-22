#ifndef __itkShapeLevelSetBasedCellSegmentation_txx
#define __itkShapeLevelSetBasedCellSegmentation_txx

#include "itkShapeLevelSetBasedCellSegmentation.h"

namespace itk
{
//  Software Guide : BeginCodeSnippet
template < class TFeatureImage,class TOutputImage >
ShapeLevelSetBasedCellSegmentation< TFeatureImage, TOutputImage >
::ShapeLevelSetBasedCellSegmentation()
{
  m_CastFeature = FeatureCastType::New();
  m_ROIfilter = ROIFilterType::New();
  m_CastOutput = OutputCastType::New();
  m_Dist = DistanceFilterType::New();

  //m_ComponentImage = NULL;

  m_LargestCellRadius = 4.0;
  m_SeedValue = 2.0;
  m_Iterations = 1000;
  m_PropagationScaling = 3;
  m_CurvatureScaling = 1;
  m_AdvectionScaling = 3;
  m_MaxRMSChange = 0.001;
  m_ShapePriorScaling = 1.0;
  m_NumberOfPCAModes = 3;

  this->Superclass::SetNumberOfRequiredInputs( 1 );
  this->Superclass::SetNumberOfRequiredOutputs( 1 );

  this->Superclass::SetNthOutput( 0, TOutputImage::New() );
}

template< class TFeatureImage, class TOutputImage >
void
ShapeLevelSetBasedCellSegmentation< TFeatureImage, TOutputImage >
::InitializeLevelSet
(ImageSpacingType spacing )
{
  ImageIndexType start;
  ImageSizeType size;
  ImageIndexType index;

  for( unsigned int j = 0; j < ImageDimension; j++ )
  {
    index[j] = 0;
    size[j] = ( 2 * static_cast<ImageSizeValueType>
      ( 1.2 * m_LargestCellRadius/spacing[j] ) + 1 ) ;
    start[j] = - static_cast<ImageSizeValueType>
      ( 1.2 * m_LargestCellRadius/spacing[j] );
  }

  ImageRegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  m_levelSet = ImageType::New();
  m_levelSet->SetRegions( region );
  m_levelSet->Allocate();
  m_levelSet->FillBuffer( 0 );
  m_levelSet->SetSpacing( spacing );

  m_levelSet->SetPixel( index, 1);

  m_Dist->SetInput( m_levelSet );
  m_Dist->InputIsBinaryOn();
  m_Dist->SetUseImageSpacing( 1 );
  m_Dist->Update();

  m_levelSet = m_Dist->GetOutput();

  IteratorType It( m_levelSet,region );
  for(It.GoToBegin();!It.IsAtEnd();++It)
  {
    It.Set( It.Get() - m_SeedValue );
  }
}

template< class TFeatureImage, class TOutputImage >
typename ShapeLevelSetBasedCellSegmentation< TFeatureImage,TOutputImage >
::CompImagePointer
ShapeLevelSetBasedCellSegmentation< TFeatureImage, TOutputImage >
::Threshold
( ImagePointer input, double thresh_low, double thresh_high, unsigned
int in_value, unsigned int out_value )
{
  typename ThresholdingFilterType::Pointer m_thresholder =
    ThresholdingFilterType::New();
  m_thresholder->SetLowerThreshold( thresh_low );
  m_thresholder->SetUpperThreshold( thresh_high );
  m_thresholder->SetInsideValue( in_value );
  m_thresholder->SetOutsideValue( out_value );
  m_thresholder->SetInput( input );
  m_thresholder->Update();

  CompImagePointer thresholdImg = m_thresholder->GetOutput();

  return ( thresholdImg );
}

template< class TFeatureImage, class TOutputImage >
typename ShapeLevelSetBasedCellSegmentation< TFeatureImage,TOutputImage >
::CompImagePointer
ShapeLevelSetBasedCellSegmentation< TFeatureImage, TOutputImage >
::GACShape
( ImagePointer speed, int NumOfCells, ImageIndexType start, ImageSizeType size )
{
  ImageRegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  typename ROIFilterType::Pointer roi = ROIFilterType::New();
  roi->SetInput( m_levelSet );
  roi->SetRegionOfInterest( region );
  roi->Update();

  typename ShapeFunctionType::ParametersType
    pcaStandardDeviations( m_NumberOfPCAModes );
  pcaStandardDeviations.Fill(1.0);

  typename TransformType::Pointer transform = TransformType::New();

  typename ShapeFunctionType::Pointer shape = ShapeFunctionType::New();
  shape->SetNumberOfPrincipalComponents( m_NumberOfPCAModes );
  shape->SetMeanImage( m_meanShape );
  shape->SetPrincipalComponentImages( m_shapeModeImages );
  shape->SetPrincipalComponentStandardDeviations( pcaStandardDeviations );
  shape->SetTransform( transform );

  typename CostFunctionType::WeightsType weights;
  weights[0] = 1.0; //weight for contour fit term
  weights[1] = 20.0;//weight for image fit term
  weights[2] = 1.0; //weight for shape prior term
  weights[3] = 1.0; //weight for pose prior term

  typename CostFunctionType::ArrayType
    mean( shape->GetNumberOfShapeParameters() );
  mean.Fill( 0.0 );
  typename CostFunctionType::ArrayType
    stddev( shape->GetNumberOfShapeParameters() );
  stddev.Fill( 1.0 );

  typename CostFunctionType::Pointer costFunction = CostFunctionType::New();
  costFunction->SetWeights( weights );
  costFunction->SetShapeParameterMeans( mean );
  costFunction->SetShapeParameterStandardDeviations( stddev );

  typename GeneratorType::Pointer generator = GeneratorType::New();
  generator->Initialize(20020702);

  typename OptimizerType::ScalesType scales( shape->GetNumberOfParameters() );
  scales.Fill(1.0);
  for( int k = 0; k < m_NumberOfPCAModes; k++ )
  {
    scales[k] = 20.0; //scales for the pca mode multiplier
  }
  scales[m_NumberOfPCAModes] = 350.0; // scale for 2D rotation

  double initRadius=1.05, grow=1.1, shrink=pow(grow,-0.25);
  typename OptimizerType::Pointer optimizer = OptimizerType::New();
  optimizer->SetNormalVariateGenerator( generator );
  optimizer->SetScales( scales );
  optimizer->Initialize( initRadius, grow, shrink );
  optimizer->SetEpsilon( 1.0e-6 ); // minimal search radius
  optimizer->SetMaximumIteration( 15 );

  typename ShapeFunctionType::ParametersType
    parameters( shape->GetNumberOfParameters() );
  parameters.Fill(0.0);
  for( unsigned int k = 1; k <= ImageDimension; k++ )
  {
    parameters[m_NumberOfPCAModes+k] = 0;
  }

  typename GeodesicActiveContourShapeFilterType::Pointer
    geodesicActiveContourShape = GeodesicActiveContourShapeFilterType::New();
  geodesicActiveContourShape->SetPropagationScaling(m_PropagationScaling);
  geodesicActiveContourShape->SetCurvatureScaling( m_CurvatureScaling );
  geodesicActiveContourShape->SetAdvectionScaling( m_AdvectionScaling );
  geodesicActiveContourShape->SetMaximumRMSError( m_MaxRMSChange );
  geodesicActiveContourShape->SetNumberOfIterations( m_Iterations );
  geodesicActiveContourShape->SetShapePriorScaling( m_ShapePriorScaling );
  geodesicActiveContourShape->SetNumberOfLayers( 4 );
  geodesicActiveContourShape->SetInput( roi->GetOutput() );
  geodesicActiveContourShape->SetFeatureImage( speed );
  geodesicActiveContourShape->SetShapeFunction( shape );
  geodesicActiveContourShape->SetCostFunction( costFunction );
  geodesicActiveContourShape->SetOptimizer( optimizer );
  geodesicActiveContourShape->SetInitialParameters( parameters );
  geodesicActiveContourShape->UseImageSpacingOn();
  geodesicActiveContourShape->Update();

  std::cout << std::endl;
  std::cout << "Max. no. iterations: " <<
    geodesicActiveContourShape->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " <<
    geodesicActiveContourShape->GetMaximumRMSError() << std::endl;
  std::cout << std::endl;
  std::cout << "No. elapsed iterations: " <<
    geodesicActiveContourShape->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " <<
    geodesicActiveContourShape->GetRMSChange() << std::endl;

  std::cout << NumOfCells+1 << std::endl;

  CompImagePointer thresh = Threshold( geodesicActiveContourShape->GetOutput(),
    -1000, 0, NumOfCells+1, 0);

  return( thresh );
}

template< class TFeatureImage, class TOutputImage >
typename ShapeLevelSetBasedCellSegmentation< TFeatureImage,TOutputImage >
::CompImagePointer
ShapeLevelSetBasedCellSegmentation< TFeatureImage, TOutputImage >
::Resample
(CompImagePointer input, ImageSpacingType spacing, ImageSizeType size,
ImagePointType origin)
{
  typedef typename itk::ResampleImageFilter< CompImageType,CompImageType >
    ResampleFilter;
  typedef typename itk::NearestNeighborInterpolateImageFunction< CompImageType >
    Interpolator;

  // create the resample filter, transform and interpolator
  typename itk::AffineTransform<double,ImageDimension>::Pointer transform =
    itk::AffineTransform<double,ImageDimension>::New();
  transform->SetIdentity();

  typename Interpolator::Pointer interp = Interpolator::New();

  typename ResampleFilter::Pointer m_resamp = ResampleFilter::New();
  m_resamp->SetTransform(transform);
  m_resamp->SetInterpolator(interp);
  m_resamp->SetInput(input);
  m_resamp->SetSize(size);
  m_resamp->SetOutputOrigin(origin);
  m_resamp->SetOutputSpacing(spacing);
  m_resamp->SetDefaultPixelValue(0);
  m_resamp->Update();

  return(m_resamp->GetOutput());
}

template< class TFeatureImage,class TOutputImage >
void
ShapeLevelSetBasedCellSegmentation< TFeatureImage,TOutputImage >::
GenerateData()
{
  std::cout << "Processing started..." << std::endl;
  m_CastFeature->SetInput( this->GetInput() );
  m_CastFeature->Update();
  ImageConstPointer featureImg = m_CastFeature->GetOutput();
  ImageSpacingType spacing = featureImg->GetSpacing();
  ImagePointType origin = featureImg->GetOrigin();
  std::cout << "Feature image read..." << std::endl;

  InitializeLevelSet( spacing );

  CompImagePointer cumMask = CompImageType::New();
  cumMask->SetRegions( featureImg->GetLargestPossibleRegion() );
  cumMask->SetSpacing( spacing);
  cumMask->SetOrigin( origin );
  cumMask->Allocate();
  cumMask->FillBuffer(0);
  std::cout << "Allocated output image..." << std::endl;

  m_ROIfilter->SetInput(featureImg);

  int NumOfCells = 0;
  typename std::map< float, ImageIndexType >::iterator indexIt;
  ImageSizeType inputSize = featureImg->GetLargestPossibleRegion().GetSize();

  ImageIndexType cellImageExtent;
  for( unsigned int j = 0; j < ImageDimension; j++ )
  {
    cellImageExtent[j] =
      static_cast<ImageIndexValueType>( 1.2 * m_LargestCellRadius/spacing[j] );
  }

  indexIt = seeds.begin();
  while( indexIt != seeds.end() )
  {
    ImageIndexType idx = indexIt->second;
    CompImagePixelType pOut = cumMask->GetPixel( idx );

    if ( pOut == 0 )
    {
      ImageIndexType start,end, levelsetStart, levelsetEnd;
      ImageSizeType size, levelsetSize;

      for( unsigned int j=0;j<ImageDimension;j++ )
      {

        if ( idx[j] >= cellImageExtent[j] )
        {
          start[j] = idx[j] - cellImageExtent[j];
          levelsetStart[j] = - cellImageExtent[j];
        }
        else
        {
          start[j] = 0;
          levelsetStart[j] = - idx[j];
        }
        if ( static_cast< ImageSizeValueType > ( idx[j] + cellImageExtent[j] )
          <= inputSize[j]-1 )
        {
          end[j] = idx[j] + cellImageExtent[j];
          levelsetEnd[j] = cellImageExtent[j];
        }
        else
        {
          end[j] = inputSize[j]-1;
          levelsetEnd[j] = inputSize[j]- idx[j] - 1;
        }
        size[j] = end[j] - start[j];
        levelsetSize[j] = levelsetEnd[j] - levelsetStart[j];
      }

      ImageRegionType region;
      region.SetSize( size );
      region.SetIndex( start );

      m_ROIfilter->SetRegionOfInterest( region );
      m_ROIfilter->Update();

      CompImagePointer subMask = GACShape( m_ROIfilter->GetOutput(), NumOfCells,
        levelsetStart, levelsetSize);
      CompIteratorType It(cumMask,region);
      CompIteratorType subIt(subMask,subMask->GetLargestPossibleRegion());
      for ( It.GoToBegin(),subIt.GoToBegin(); !It.IsAtEnd();++It,++subIt )
        if ( subIt.Get()>0 )
          It.Set(subIt.Get());

      ++NumOfCells;
    }
    ++indexIt;
  }
  std::cout << "Finished level-set segmentation..." << std::endl;

  m_CastOutput->SetInput( cumMask );
  m_CastOutput->GraftOutput( this->GetOutput() );
  m_CastOutput->Update();

  std::cout << "Finished processing..." << std::endl;
  this->GraftOutput( m_CastOutput->GetOutput() );
}


template < class TFeatureImage, class TOutputImage >
void
ShapeLevelSetBasedCellSegmentation< TFeatureImage,TOutputImage >::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Largest cell radius:" << this->m_LargestCellRadius
    << std::endl;
  os << indent << "seedValue:" << this->m_SeedValue << std::endl;
  os << indent << "Max Iterations:" << this->m_Iterations << std::endl;
  os << indent << "propagationScaling:" << this->m_PropagationScaling
    << std::endl;
  os << indent << "curvatureScaling:" << this->m_CurvatureScaling << std::endl;
  os << indent << "advectionScaling:" << this->m_AdvectionScaling << std::endl;
  os << indent << "maxRMSChange:" << this->m_MaxRMSChange << std::endl;
}

} /* end namespace itk */

#endif
