#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkAttributeLabelObject.h"
#include "itkLabelMap.h"

#include "itkLabelImageToLabelMapFilter.h"

#include "itkAttributeKeepNObjectsLabelMapFilter.h"
#include "itkAttributeOpeningLabelMapFilter.h"
#include "itkAttributeRelabelLabelMapFilter.h"

#include "itkLabelMapToAttributeImageFilter.h"
#include "itkLabelMapToLabelImageFilter.h"


int main(int argc, char * argv[])
{

  if( argc != 10 )
    {
    std::cerr << "usage: " << argv[0] << " label input attr keep open relabel bg lambda nb" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  // declare the dimension used, and the type of the input image
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;
  
  // The AttributeLabelObject class take 3 template parameters: the 2 ones
  // from the LabelObject class, and the type of the attribute associated
  // with each node. Here we have chosen a double. We then declares the
  // type of the LabelMap with the type of the label object.
  typedef itk::AttributeLabelObject< unsigned long, dim, double > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;

  // We read the input image.
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  
  // And convert it to a LabelMap
  typedef itk::LabelImageToLabelMapFilter< IType, LabelMapType > I2LType;
  I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput( reader->GetOutput() );
  i2l->SetBackgroundValue( atoi(argv[7]) );
  // The next step is made outside the pipeline model, so we call Update() now.
  i2l->Update();
  reader2->Update();
  
  // Now we will valuate the attributes. The attribute will be the mean of the pixels
  // values in the 2nd image. The StatisticsLabelObject can give us that value, without
  // having to code that by hand - that's an example.
  
  LabelMapType::Pointer labelMap = i2l->GetOutput();
  
  // Lets begin by declaring the iterator for the objects in the image.
  LabelMapType::LabelObjectContainerType::const_iterator it;
  // And get the object container to reuse it later
  const LabelMapType::LabelObjectContainerType & labelObjectContainer = labelMap->GetLabelObjectContainer();
  for( it = labelObjectContainer.begin(); it != labelObjectContainer.end(); it++ )
    {
    // the label is there if we need it, but it can also be found at labelObject->GetLabel().
    // const PType & label = it->first;
    
    // the label object
    LabelObjectType * labelObject = it->second;
  
    // init the vars
    double mean = 0;
    unsigned long size = 0;
    
    // the iterator for the lines
    LabelObjectType::LineContainerType::const_iterator lit;
    LabelObjectType::LineContainerType & lineContainer = labelObject->GetLineContainer();
  
    // iterate over all the lines
    for( lit = lineContainer.begin(); lit != lineContainer.end(); lit++ )
      {
      const LabelMapType::IndexType & firstIdx = lit->GetIndex();
      const unsigned long & length = lit->GetLength();
      
      size += length;
  
      long endIdx0 = firstIdx[0] + length;
      for( LabelMapType::IndexType idx = firstIdx; idx[0]<endIdx0; idx[0]++)
        {
        mean += reader2->GetOutput()->GetPixel( idx );
        }
      }
    
    mean /= size;
    
    labelObject->SetAttribute( mean );
    labelObject->Print( std::cout );
    
    }
    
  // now that the objects have their attribute, we are free to manipulate them with
  // the prebuilt filters, or by hand.
  // The default accessor (AttributeLabelObject) is the right one when using AttributeLabelObject
  // so we don't have to specify it. A different one can be used if needed though.
  
  typedef itk::AttributeKeepNObjectsLabelMapFilter< LabelMapType > KeepType;
  KeepType::Pointer keep = KeepType::New();
  keep->SetInput( labelMap );
  keep->SetReverseOrdering( true );
  keep->SetNumberOfObjects( atoi(argv[9]) );
  // prevent the filter to run in place, and modify the input image
  keep->SetInPlace( false );

  typedef itk::AttributeOpeningLabelMapFilter< LabelMapType > OpeningType;
  OpeningType::Pointer opening = OpeningType::New();
  opening->SetInput( labelMap );
  opening->SetLambda( atof(argv[8]) );
  opening->SetInPlace( false );

  typedef itk::AttributeRelabelLabelMapFilter< LabelMapType > RelabelType;
  RelabelType::Pointer relabel = RelabelType::New();
  relabel->SetInput( labelMap );
  relabel->SetInPlace( false );
  
  // The attribute values can be put directly in a classic image
  
  typedef itk::LabelMapToAttributeImageFilter< LabelMapType, IType > A2IType;
  A2IType::Pointer a2i = A2IType::New();
  a2i->SetInput( labelMap );
  
  // or the label collection can be converted back to an label image, or to a binary image
  // (not shown here)
  
  typedef itk::LabelMapToLabelImageFilter< LabelMapType, IType > L2IType;
  L2IType::Pointer l2i = L2IType::New();
  
  // finally, we write the results
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  
  writer->SetInput( a2i->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->Update();

  writer->SetInput( l2i->GetOutput() );
  
  l2i->SetInput( keep->GetOutput() );
  writer->SetFileName( argv[4] );
  writer->Update();

  l2i->SetInput( opening->GetOutput() );
  writer->SetFileName( argv[5] );
  writer->Update();

  l2i->SetInput( relabel->GetOutput() );
  writer->SetFileName( argv[6] );
  writer->Update();
  
  return 0;
}

