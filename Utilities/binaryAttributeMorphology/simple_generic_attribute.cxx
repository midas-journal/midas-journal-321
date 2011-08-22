#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkAttributeOpeningLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"


// This example shows how to use the LabelMap classes to remove all the
// object with a bounding size smaller (or greater) than a given value
// on the z axis.
// The attribute we are interested is not in the ones already implemented,
// but can be easily computed from the one in the ShapeLabelObject.
// We just need to have an accessor that the generic opening filter
// can use to access them. That's the role of LastDimesionRegionSizeObjectAccessor.
template< class TLabelObject >
class ITK_EXPORT LastDimesionRegionSizeObjectAccessor
{
public:
  // The declaration of AttributeValueType is mandatory. It is used internally
  // in the opening filter to define the type of the lambda attribute.
  // AttributeValueType should be the same as the type returned by the operator()
  // method.
  typedef unsigned long AttributeValueType;

  // operator() is the core of the accessor. It takes a label object as parameter,
  // and return the attribute of interest.
  inline const AttributeValueType operator()( const TLabelObject * labelObject )
    {
    return labelObject->GetRegion().GetSize()[TLabelObject::ImageDimension-1];
    }
};



int main(int argc, char * argv[])
{

  if( argc != 6 )
    {
    std::cerr << "usage: " << argv[0] << " input output bg lambda reverse" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  // declare the dimension used, and the type of the input image
  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;
  
  // We read the input image.
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  // And convert it to a LabelMap, with the shape attribute computed.
  // We use the default label object type.
  typedef itk::LabelImageToShapeLabelMapFilter< IType > I2LType;
  I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput( reader->GetOutput() );
  i2l->SetBackgroundValue( atoi(argv[3]) );
  
  // The opening filter is declared with our custom accessor type as second
  // template argument, so it will be able to user our custom attribute to make
  // the opening.
  typedef LastDimesionRegionSizeObjectAccessor< I2LType::LabelObjectType > AccessorType;
  typedef itk::AttributeOpeningLabelMapFilter< I2LType::OutputImageType, AccessorType > OpeningType;
  OpeningType::Pointer opening = OpeningType::New();
  opening->SetInput( i2l->GetOutput() );
  opening->SetLambda( atof(argv[4]) );
  opening->SetReverseOrdering( atof(argv[5]) );
  itk::SimpleFilterWatcher watcher(opening, "filter");

  // the label map is then converted back to an label image.
  typedef itk::LabelMapToLabelImageFilter< I2LType::OutputImageType, IType > L2IType;
  L2IType::Pointer l2i = L2IType::New();
  l2i->SetInput( opening->GetOutput() );
  
  // write the result
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( l2i->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  // finally, print all the label objects after the opening, to check everything has been done right.
  opening->GetOutput()->PrintLabelObjects();
  std::cout << "Number of objects after the opening: " << opening->GetOutput()->GetNumberOfLabelObjects() << std::endl;
  
  return 0;
}

