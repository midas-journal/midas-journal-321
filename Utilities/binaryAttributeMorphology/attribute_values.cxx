#include "itkImageFileReader.h"
#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkBinaryImageToShapeLabelMapFilter.h"

int main(int argc, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, dim >    ImageType;
  
  if( argc != 3)
    {
    std::cerr << "usage: " << argv[0] << " input foreground" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  // read the input image
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  // define the object type. Here the ShapeLabelObject type
  // is chosen in order to read some attribute related to the shape
  // of the objects (by opposition to the content of the object, with
  // the StatisticsLabelObejct).
  typedef unsigned long LabelType;
  typedef itk::ShapeLabelObject< LabelType, dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;

  // convert the image in a collection of objects
  typedef itk::BinaryImageToShapeLabelMapFilter< ImageType, LabelMapType > ConverterType;
  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( reader->GetOutput() );
  converter->SetForegroundValue( atoi(argv[2]) );

  // update the shape filter, so its output will be up to date
  converter->Update();

  // then we can read the attribute values we're interested in. The BinaryImageToShapeLabelMapFilter
  // produce consecutive labels, so we can use a for loop and GetLabelObject() method to retrieve
  // the label objects. If the labels are not consecutive, the GetNthLabelObject() method must be
  // use instead of GetLabelObject(), or an iterator on the label object container of the label map.
  LabelMapType::Pointer labelMap = converter->GetOutput();
  for( unsigned int label=1; label<=labelMap->GetNumberOfLabelObjects(); label++ )
    {
    // we don't need a SmartPointer of the label object here, because the reference is kept in
    // in the label map.
    const LabelObjectType * labelObject = labelMap->GetLabelObject( label );
    std::cout << label << "\t" << labelObject->GetPhysicalSize() << "\t" << labelObject->GetCentroid() << std::endl;
    }
  
  return 0;
}

