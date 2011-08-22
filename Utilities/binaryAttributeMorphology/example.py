
import itk, sys
itk.auto_progress()

Dimension = 2
PixelType = itk.UC
ImageType = itk.Image[ PixelType, Dimension ]

DistancePixelType = itk.F
DistanceImageType = itk.Image[ DistancePixelType, Dimension ]

RGBPixelType = itk.RGBPixel[PixelType]
RGBImageType = itk.Image[ RGBPixelType, Dimension ]

LabelObjectType = itk.StatisticsLabelObject[itk.UL, Dimension]
LabelMapType = itk.LabelMap[LabelObjectType]

# read the image of the nucleus
nuclei = itk.ImageFileReader[ImageType].New(FileName="images/noyaux.png")
# perform a simple binarization. Note that the Otsu filter does not use the same convention as usual : the white part is outside.
otsu = itk.OtsuThresholdImageFilter[ImageType, ImageType].New(nuclei, OutsideValue=255, InsideValue=0)
# split the nuclei
maurer = itk.SignedMaurerDistanceMapImageFilter[ImageType, DistanceImageType].New(otsu)
watershed = itk.MorphologicalWatershedImageFilter[DistanceImageType, ImageType].New(maurer, Level=60, MarkWatershedLine=False)
mask = itk.MaskImageFilter[ImageType, ImageType, ImageType].New(watershed, otsu)
# now switch to the label map representation and compute the attribute values, including the perimeter and the Feret diameter
stats = itk.LabelImageToStatisticsLabelMapFilter[ImageType, ImageType, LabelMapType].New(mask, nuclei)
# drop the objects too small to be a nucleus, and the ones on the border
size = itk.ShapeOpeningLabelMapFilter[LabelMapType].New(stats, Attribute='Size', Lambda=100)
border = itk.ShapeOpeningLabelMapFilter[LabelMapType].New(size, Attribute='SizeOnBorder', Lambda=10, ReverseOrdering=True)
# reoder the labels. The objects with the highest mean are the first ones.
relabel = itk.StatisticsRelabelLabelMapFilter[LabelMapType].New(border, Attribute='Mean')
# for visual validation
overlay = itk.LabelMapOverlayImageFilter[LabelMapType, ImageType, RGBImageType].New(relabel, nuclei)
itk.write(overlay, "nuclei-overlay.png")


spots = itk.ImageFileReader[ImageType].New(FileName="images/spots.png")
# mask the spot image to keep only the nucleus zone. The rest of the image is cropped, excepted a border of 2 pixels
maskSpots = itk.LabelMapMaskImageFilter[LabelMapType, ImageType].New(relabel, spots, Label=1, Crop=True, CropBorder=2)
th = itk.BinaryThresholdImageFilter[ImageType, ImageType].New(maskSpots, LowerThreshold=110)
sstats = itk.BinaryImageToStatisticsLabelMapFilter[ImageType, ImageType, LabelMapType].New(th, nuclei)
# we know there are 4 spots in the nubleus, so keep the 4 biggest spots
skeep = itk.ShapeKeepNObjectsLabelMapFilter[LabelMapType].New(sstats, Attribute='Size', NumberOfObjects=4)
# reoder the labels. The bigger objects first.
srelabel = itk.StatisticsRelabelLabelMapFilter[LabelMapType].New(skeep, Attribute='Size')



# display the values we are interested in:
# - the nucleus number
# - the spot position
# - the mean value in the nucleus in the spot zone
print "nuclei", "x", "y", "mean"
for nl in range(1, relabel.GetOutput().GetNumberOfLabelObjects()+1):
	maskSpots.SetLabel(nl)
	srelabel.UpdateLargestPossibleRegion()
	labeCollection = srelabel.GetOutput()
	for l in range(1, labeCollection.GetNumberOfLabelObjects()+1):
		lo = labeCollection.GetLabelObject(l)
		print nl, lo.GetCentroid()[0], lo.GetCentroid()[1], lo.GetMean()


