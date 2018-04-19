# bfio - Binary formats I/O. 

**bfio** is a small, one header only library for serialization/deserialization of C/C++ objects.

Goals:

* Serialize/Descirialize C/C++ objects into/from existing binary formats. For example you can define readers/writers for zip-archive headers, PE headers, ELF headers, etc..
* Provide API that define readers/writers simultaneously, so that you do not need to write code for serialization and deserialization separetly, but alloing to do so if needed.
* Provide API similar to [boost serialization library](https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/)
* File I/O api agnostic and allowing reading/writing to memory buffers.

*Why not [boost serialization library](https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/) or [Cereal](https://uscilab.github.io/cereal/) or others?*

\- The goal is not just to serialize/deserialize objects, but write and read specific (existing) binary formats.

## Showcase. Truevision TGA header

Detailed description of the format can be found at [wiki](https://en.wikipedia.org/wiki/Truevision_TGA)

The main header has the following structure:

| Field no. | Length | Field name | Description|
| --------- | ------ | ---------- | ---------- |
| 1         | 1 byte | ID length  | Length of the image ID field |
| 2         | 1 byte | Color map type | Whether a color map is included |
| 3         | 1 byte | Image type | Compression and color types|
| 4         | 5 bytes| Color map specification | Describes the color map |
| 5         |10 bytes| Image specification |Image dimensions and format |

Color map has the following structure:

| Length | Field name | Description|
| ------ | ---------- | ---------- |
| 2 byte | First entry index  | Index of first color map entry that is included in the file |
| 2 byte | Color map length | Number of entries of the color map that are included in the file |
| 1 byte | Color map entry size | Number of bits per pixel|

Image specification has the following structure:

| Length | Field name | Description|
| ------ | ---------- | ---------- |
| 2 byte | X-origin  | absolute coordinate of lower-left corner for displays where origin is at the lower left |
| 2 byte | Y-origin | as for X-origin |
| 2 byte | Image width | width in pixels |
| 2 byte | Image height | height in pixels |
| 1 byte | Pixel depth | bits per pixel |
| 1 byte | Image descriptor | bits 3-0 give the alpha channel depth, bits 5-4 give direction|

Let's say, we have defined structures for main header, color-map, and image specification: 

```cpp
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
```

Then, we should define serialization functions for each type:

```cpp
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
```
In the case shown above, the template functions define serialization process for both, reading and writing.

Now we can use them as follows:

```cpp
int main()
{
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
	return 0;
}
```
