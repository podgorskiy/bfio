#include <bfio.h>
#include <cassert>
#include <inttypes.h>

struct ColorMapSpecificatuion
{
	uint16_t firstEntryIndex;
	uint16_t colorMapLength;
	uint8_t colorMapEntrySize;
};

struct ImageSpecificatuion
{
	uint16_t xOrigin;
	uint16_t yOrigin;
	uint16_t width;
	uint16_t height;
	uint8_t pixelDepth;
	uint8_t imageDescriptor;
};

struct MainTGAHeader
{
	uint8_t IDLength;
	uint8_t colorMapType;
	uint8_t imageType;
	ColorMapSpecificatuion colormap;
	ImageSpecificatuion imageSpec;
};

namespace bfio
{
	template<class RW>
	inline void Serialize(RW& io, ColorMapSpecificatuion& x)
	{
		io & x.firstEntryIndex;
		io & x.colorMapLength;
		io & x.colorMapEntrySize;
	}
	template<class RW>
	inline void Serialize(RW& io, ImageSpecificatuion& x)
	{
		io & x.xOrigin;
		io & x.yOrigin;
		io & x.width;
		io & x.height;
		io & x.pixelDepth;
		io & x.imageDescriptor;
	}
	template<class RW>
	inline void Serialize(RW& io, MainTGAHeader& x)
	{
		io & x.IDLength;
		io & x.colorMapType;
		io & x.imageType;
		io & x.colormap;
		io & x.imageSpec;
	}
}

int main()
{
	// Reading image header
	FILE* f = fopen("test.tga", "rb");
	bfio::CFileStream stream(f);

	printf("Tga header size: %d\n", (int)bfio::SizeOf<MainTGAHeader>());

	MainTGAHeader tgaHeader;

	stream >> tgaHeader;

	printf("Image info:\nColormap: %s\nImage type: %d\nImage width %d\nImage height: %d\nPixel depth: %d\n"
		,tgaHeader.colorMapType == 0 ? "No colormap" : "Has colormap"
		,tgaHeader.imageType
		,tgaHeader.imageSpec.width
		,tgaHeader.imageSpec.height
		,tgaHeader.imageSpec.pixelDepth);

	fclose(f);

	// Writing image header
	FILE* fout = fopen("header.bin", "wb");

	bfio::CFileStream outstream(fout);
	outstream << tgaHeader;
	fclose(fout);

	return 0;
}
