# bfio - Binary formats I/O. 

[![Build Status](https://travis-ci.org/podgorskiy/bfio.svg?branch=master)](https://travis-ci.org/podgorskiy/bfio)

**bfio** is a small, one header only library for serialization/deserialization of C/C++ objects.

Goals:

* Serialize/Deserialize C/C++ objects into/from existing binary formats. For example, you can define readers/writers for zip-archive headers, PE headers, ELF headers, etc..
* Provide API that defines readers/writers simultaneously, so that you do not need to write code for serialization and deserialization separately, but allowing to do so if needed.
* Provide API similar to [boost serialization library](https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/)
* File I/O API agnostic and allowing reading/writing to memory buffers.

*Why not [boost serialization library](https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/) or [Cereal](https://uscilab.github.io/cereal/) or others?*

\- The goal is not just to serialize/deserialize objects, but write and read specific (existing) binary formats.

Below presented a very simple show case for reading/writing TGA headers, you can find full example [here](https://github.com/podgorskiy/bfio/blob/master/examples/example_3.cpp).

For more complex examples see [streams example](https://github.com/podgorskiy/bfio/blob/master/examples/example_1.cpp) and  [reading zip archive](https://github.com/podgorskiy/bfio/blob/master/examples/example_2.cpp) example.

# Showcase. Truevision TGA header

The detailed description of the format can be found at [wiki](https://en.wikipedia.org/wiki/Truevision_TGA)

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
(Syntax is heavily inspired by [boost serialization library](https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/).)

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

```

# Features.

* Provides default serialization functions for some standard containers. 
To enable containers, you need to define one of those:
  * *BFIO_INCLUDE_VECTOR* for *std::vector*
  * *BFIO_INCLUDE_STRING* for *std::string*
  * *BFIO_INCLUDE_MAP* for *std::map*
  * *BFIO_INCLUDE_SET* for *std::set*
  * *BFIO_INCLUDE_LIST* for *std::list*
* Has support for glm. To enable define *BFIO_INCLUDE_GLM*
* Has *SizeOf* function that can return size of the object to be serialized, with optimizations on it is optimized to ilined constant.
* Has predefined streams:
  * *CFileStream* for working with C files
  * *StaticMemoryStream* for working with preallocated memory buffer
  * *DynamicMemoryStream* for working with dynamically growing memory buffer
* Allows defining custom streams for working with any I/O API.

# Installation.

It is a one-header library, no installation is needed. Add the path to the *bfio/include* to your search paths for headers. CMakeLists.txt is mainly for building examples are tests.
However, you can install the library on the system by running:

```
mkdir build
cd build
cmake ../
make install
```

# Reference

## How to write serialization functions?

There are two ways:

 * Write a generic serialization function for both, writing and reading.
 * Write distinct functions for writing and for reading
 
Typically, the first option is preferred. The second option is used when you need to do some additional manipulations that are different for reading and writing.

To write a generic function, create in **bfio** namespace a template function *Serialize* of the form:

```cpp
namespace bfio
{
    template<typename Stream, AccessType direction>
    inline void Serialize(Accessor<Stream, direction>& w, MyData& x)
}
```

Where *MyData* - is the type for which the serialization function is written. 

The function should take a reference to the template *bfio::Accessor<typename Stream, enum Direction>* object and a non-const reference to your type.

Using operator *&* of the *bfio::Accessor*, you can specify which members and in which order should be serialized/deserialized.

You can write generic functions, which are templates with arguments *Stream* and *Direction* or you can create a specialization for particular direction (Reading or Writing) or specific stream types.

### Example:

Structure:

```cpp
struct MyData
{
    std::map<int, std::string> m;
    std::string str;
    std::vector<std::pair<std::string, float> > v;

    int a[10];
};
```

Serialization function:

```cpp
namespace bfio
{
    template<typename Stream, AccessType direction>
    inline void Serialize(Accessor<Stream, direction>& w, MyData& x)
    {
        w & x.m;
        w & x.str;
        w & x.v;
        w & x.a;
    }
}
```

Or in shortened form:

```cpp
namespace bfio
{
    template<typename A>
    inline void Serialize(A& w, MyData& x)
    {
        w & x.m;
        w & x.str;
        w & x.v;
        w & x.a;
    }
}
```


The second way is to specify functions for reading/writing separately. To do so, you need to specify *bfio::Accessor* with template arguments of:

 * Stream type. Intended to be a template argument. However, you can create a specialization for particular stream types.
 * Direction. *bfio::AccessType::Reading* or *bfio::AccessType::Writing*

Example:

```cpp
namespace bfio
{
    template<typename Stream>
    inline void Serialize(Accessor<Stream, Writing>& w, MyData& x)
    {
        printf("Hello from writer!");
        w & x.m;
        w & x.str;
        w & x.v;
        w & x.a;
    }

    template<typename Stream>
    inline void Serialize(Accessor<Stream, Reading>& r, MyData& x)
    {
        printf("Hello from reader!");
        r & x.m;
        r & x.str;
        r & x.v;
        r & x.a;
    }
}
```

## How to use streams?

Streams are classes derived from *bfio::Stream* class and implement the following functions:

```cpp
    bool Write(const void* src, size_t size);
    bool Read(void* dst, size_t size);
```

Class *bfio::Stream* defines operators *<<* and *>>* for serialization and deserialization. 
There are three default streams:

* *CFileStream* - for access to fread and fwrite from standard c library.
* *StaticMemoryStream* - for reading/writing to the preallocated memory buffer.
* *DynamicMemoryStream* - for reading/writing to dynamic, growing memory buffer.

You can create your streams by subclassing from *bfio::Stream*.

*CFileStream*  as an example:

```cpp
    class CFileStream : public Stream<CFileStream>
    {
    public:
        CFileStream(FILE* f) : file(f)
        {};

        bool Write(const void* src, size_t size)
        {
            return fwrite(src, size, 1, file) == size;
        }
        bool Read(void* dst, size_t size)
        {
            return fread(dst, size, 1, file) == size;
        }

    private:
        FILE* file;
    };
```

### *CFileStream*

*CFileStream* is a small wrapper for reading/writing using fread and fwrite functions from C standard library.

Useage:

```cpp
    FILE* f = fopen("test.bin", "wb");
    CFileStream(f) << mydata;
    fcolse(f);
```

### *StaticMemoryStream*

This stream type takes a pointer to preallocated memory buffer and size of the buffer in its constructor. This stream does not copy data from the buffer and does not delete it. You should use this stream with caution and make sure the stream is not used after the memory is freed. 

Usage:

```cpp
    char* data = new char[512];
    bfio::StaticMemoryStream stearm(data, 512);
    stearm << mydata;
```

It won't read/write beyond the buffer boundaries, the access operators (<<,  >>) will return false for this case.

Provides additional member functions:

* *size_t StaticMemoryStream::GetSize() const* Returns size of the buffer associated with the stream.
* *char* StaticMemoryStream::Data()* Returns pointer to the buffer.
* *const char* StaticMemoryStream::DataConst() const* Return const pointer to the buffer.
* *void StaticMemoryStream::Seek(size_t position)* Sets internal offset pointer to the given position.
* *size_t StaticMemoryStream::Tell() const* Returns internal offset pointer.
    

### *DynamicMemoryStream*

This stream creates internal memory buffer that can grow. If in constructor specified a pointer to a memory block and size, this memory buffer will be copied to the internal buffer and thus can be freed after the construction of the *DynamicMemoryStream*.

Internal buffer grows when the data to be written goes beyond the internal buffer size.

Usage:

```cpp
    char* data = new char[512];
    bfio::StaticMemoryStream stearm(data, 512);
    stearm << mydata;
```

Provides all member functions of *StaticMemoryStream* plus additional member function:

* *bool StaticMemoryStream::Resize(size_t newSize)* Chanes the size of internal buffer.

For more references see [examples](https://github.com/podgorskiy/bfio/tree/master/examples)
