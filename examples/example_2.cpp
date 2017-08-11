#include <bfio.h>
#include <cassert>

namespace ZIP_SIGNATURES
{
	enum
	{
		CENTRAL_DIRECTORY_FILE_HEADER = 0x02014b50,
		END_OF_CENTRAL_DIRECTORY_SIGN = 0x06054b50,
		LOCAL_HEADER                  = 0x04034b50,
	};
}

struct DataDescriptor
{
	int32_t CRC32;
	int32_t compressedSize;
	int32_t uncompressedSize;
};

struct CentralDirectoryHeader
{
	int32_t centralFileHeaderSignature;
	int16_t versionMadeBy;
	int16_t	versionNeededToExtract;
	int16_t	generalPurposBbitFlag;
	int16_t	compressionMethod;
	int16_t	lastModFileTime;
	int16_t	lastModFileDate;
	DataDescriptor dataDescriptor;
	int16_t	fileNameLength;
	int16_t	extraFieldLength;
	int16_t	fileCommentLength;
	int16_t	diskNumberStart;
	int16_t	internalFileAttributes;
	int32_t	externalFileAttributes;
	int32_t	relativeOffsetOfLocalHeader;
};

struct EndOfCentralDirectoryRecord
{
	int32_t endOfCentralDirSignature;
	int16_t	numberOfThisDisk;
	int16_t	numberOfTheDiskWithTheStartOfTheCentralDirectory;
	int16_t	totalNumberOfEntriesInTheCentralDirectoryOnThisDisk;
	int16_t	totalNumberOfEntriesInTheCentralDirectory;
	int32_t	sizeOfTheCentralDirectory;
	int32_t	offsetOfStartOfCentralDirectory;
	int16_t ZIPFileCommentLength;
};

struct LocalFileHeader
{
	int32_t localFileHeaderSignature;
	int16_t	versionNeededToExtract;
	int16_t	generalPurposeBitFlag;
	int16_t	compressionMethod;
	int16_t	lastModFileTime;
	int16_t	lastModFileDate;
	DataDescriptor dataDescriptor;
	int16_t	fileNameLength;
	int16_t	extraFieldLength;
};

namespace bfio
{
	template<class RW>
	inline void Serialize(RW& io, DataDescriptor& x)
	{
		io & x.CRC32;
		io & x.compressedSize;
		io & x.uncompressedSize;
	}
	template<class RW>
	inline void Serialize(RW& io, CentralDirectoryHeader& x)
	{
		io & x.centralFileHeaderSignature;
		io & x.versionMadeBy;
		io & x.versionNeededToExtract;
		io & x.generalPurposBbitFlag;
		io & x.compressionMethod;
		io & x.lastModFileTime;
		io & x.lastModFileDate;
		io & x.dataDescriptor;
		io & x.fileNameLength;
		io & x.extraFieldLength;
		io & x.fileCommentLength;
		io & x.diskNumberStart;
		io & x.internalFileAttributes;
		io & x.externalFileAttributes;
		io & x.relativeOffsetOfLocalHeader;
	}
	template<class RW>
	inline void Serialize(RW& io, EndOfCentralDirectoryRecord& x)
	{
		io & x.endOfCentralDirSignature;
		io & x.numberOfThisDisk;
		io & x.numberOfTheDiskWithTheStartOfTheCentralDirectory;
		io & x.totalNumberOfEntriesInTheCentralDirectoryOnThisDisk;
		io & x.totalNumberOfEntriesInTheCentralDirectory;
		io & x.sizeOfTheCentralDirectory;
		io & x.offsetOfStartOfCentralDirectory;
		io & x.ZIPFileCommentLength;
	}
	template<class RW>
	inline void Serialize(RW& io, LocalFileHeader& x)
	{
		io & x.localFileHeaderSignature;
		io & x.versionNeededToExtract;
		io & x.generalPurposeBitFlag;
		io & x.compressionMethod;
		io & x.lastModFileTime;
		io & x.lastModFileDate;
		io & x.dataDescriptor;
		io & x.fileNameLength;
		io & x.extraFieldLength;
	}
}

int main()
{
	FILE* f = fopen("archive.zip", "rb");
	bfio::CFileStream stream(f);

	size_t sizeOfCDEND = bfio::SizeOf<EndOfCentralDirectoryRecord>();

	fseek(f, -sizeOfCDEND, SEEK_END);

	EndOfCentralDirectoryRecord eocd;

	stream >> eocd;

	assert(eocd.endOfCentralDirSignature == ZIP_SIGNATURES::END_OF_CENTRAL_DIRECTORY_SIGN);

	fseek(f, eocd.offsetOfStartOfCentralDirectory, SEEK_SET);

	std::string filename;

	for (int i = 0; i < eocd.totalNumberOfEntriesInTheCentralDirectory; ++i)
	{
		CentralDirectoryHeader header;
		stream >> header;

		assert(header.centralFileHeaderSignature == ZIP_SIGNATURES::CENTRAL_DIRECTORY_FILE_HEADER);

		size_t currPos = ftell(f);

		fseek(f, header.relativeOffsetOfLocalHeader, SEEK_SET);

		LocalFileHeader fileHeader;

		stream >> fileHeader;

		assert(fileHeader.localFileHeaderSignature == ZIP_SIGNATURES::LOCAL_HEADER);

		filename.resize(fileHeader.fileNameLength);
		fread(&filename[0], fileHeader.fileNameLength, 1, f);

		printf("%s\n", filename.data());

		if (fileHeader.dataDescriptor.uncompressedSize != 0 && fileHeader.compressionMethod == 0 && (fileHeader.generalPurposeBitFlag & 1) == 0)
		{
			printf("\tFile content:\n");
			char* data = new char[fileHeader.dataDescriptor.uncompressedSize];
			fread(data, fileHeader.dataDescriptor.uncompressedSize, 1, f);
			fwrite(data, sizeof(char), fileHeader.dataDescriptor.uncompressedSize, stdout);
			delete[] data;
		}

		fseek(f, currPos + header.fileNameLength + header.extraFieldLength + header.fileCommentLength, SEEK_SET);
	}
	fclose(f);
	return 0;
}
