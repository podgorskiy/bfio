#include <bfio.h>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

struct POD_dataStruct
{
	uint32_t a;
	uint64_t b;
	uint16_t c;
	uint8_t d;
	float e;
	double f;
	uint16_t h[3];
};

namespace bfio
{
	template<class A>
	inline void Serialize(A& io, POD_dataStruct& x)
	{
		io & x.a;
		io & x.b;
		io & x.c;
		io & x.d;
		io & x.e;
		io & x.f;
		io & x.h;
	}
}

TEST_CASE("Size calculation test", "[sizeof][POD struct]")
{
	SECTION("Check for POD types")
	{
		REQUIRE(bfio::SizeOf<int32_t>() == sizeof(int32_t));
		REQUIRE(bfio::SizeOf<int64_t>() == sizeof(int64_t));
	}
	SECTION("Check for a struct composed out of POD types")
	{
		REQUIRE(bfio::SizeOf<POD_dataStruct>() == (sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(float) + sizeof(double) + sizeof(uint16_t[3])));
	}
}

struct TestStream : bfio::Stream<TestStream>
{
public:
	bool Write(const char* src, size_t size)
	{
		this->src = src;
		this->size = size;
		return true;
	}
	bool Read(char* dst, size_t size)
	{
		this->src = dst;
		this->size = size;
		return true;
	}

	const char* src;
	size_t size;
};

TEST_CASE("Stream test", "[stream]")
{
	SECTION("Writing data to stream. Checking if approriate call to Write was made")
	{
		TestStream testStream;
		int a = 5;
		testStream << a;

		REQUIRE(testStream.size == sizeof(a));
		REQUIRE(testStream.src == (char*)&a);
	}

	SECTION("Reading data to stream. Checking if approriate call to read was made")
	{
		TestStream testStream;
		char b;
		testStream >> b;

		REQUIRE(testStream.size == sizeof(char));
		REQUIRE(testStream.src == &b);
	}
}

TEST_CASE("CFile stream test", "[cfile]")
{
	FILE* f = fopen("test.bin", "wb");
	bfio::CFileStream(f) << 5;
	fclose(f);
	f = fopen("test.bin", "rb");
	int a = 0;
	bfio::CFileStream(f) >> a;
	fclose(f);
	REQUIRE(a == 5);
}

TEST_CASE("Dynamic memory stream test", "[dynamic]")
{
	SECTION("Grow test")
	{
		bfio::DynamicMemoryStream dms;
		char buff[33];
		bool result = dms.Write(buff, 33);
		REQUIRE(result);
		REQUIRE(dms.GetSize() == 33);
	}
	SECTION("Write / read back test")
	{
		bfio::DynamicMemoryStream dms;
		int a = 5;
		dms << a;
		dms.Seek(0);
		int b = 0;
		dms >> b;
		REQUIRE(a == b);
		REQUIRE(b == 5);
	}
}

TEST_CASE("Static memory stream test", "[static]")
{
	SECTION("Grow test")
	{
		char buff[33];
		bfio::StaticMemoryStream sms(buff, 33);
		bool result = sms.Write(buff, 33);
		REQUIRE(result);
		REQUIRE(sms.GetSize() == 33);
	}
	SECTION("Write overflow test")
	{
		char buff[33];
		bfio::StaticMemoryStream sms(buff, 33);
		bool result = sms.Write(buff, 1024);
		REQUIRE(!result);
		REQUIRE(sms.GetSize() == 33);
	}
	SECTION("Write / read back test")
	{
		char buff[33];
		bfio::StaticMemoryStream sms(buff, 33);
		int a = 5;
		sms << a;
		sms.Seek(0);
		int b = 0;
		sms >> b;
		REQUIRE(a == b);
		REQUIRE(b == 5);
	}
}

TEST_CASE("std::vector test", "[vector][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::vector<int> vWrite;
	vWrite.push_back(1);
	vWrite.push_back(2);
	vWrite.push_back(3);
	sms << vWrite;
	sms.Seek(0);
	std::vector<int> vRead;
	sms >> vRead;
	REQUIRE(vRead == vWrite);
	REQUIRE(vRead[0] == 1);
	REQUIRE(vRead[1] == 2);
	REQUIRE(vRead[2] == 3);
}

TEST_CASE("std::list test", "[list][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::list<int> vWrite;
	vWrite.push_back(1);
	vWrite.push_back(2);
	vWrite.push_back(3);
	sms << vWrite;
	sms.Seek(0);
	std::list<int> vRead;
	sms >> vRead;
	REQUIRE(vRead == vWrite);
}

TEST_CASE("std::map test", "[map][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::map<int, int> vWrite;
	vWrite[1] = 10;
	vWrite[2] = 20;
	vWrite[3] = 30;
	sms << vWrite;
	sms.Seek(0);
	std::map<int, int> vRead;
	sms >> vRead;
	REQUIRE(vRead == vWrite);
	REQUIRE(vRead.size() == 3);
	REQUIRE(vRead[1] == 10);
	REQUIRE(vRead[2] == 20);
	REQUIRE(vRead[3] == 30);
}

TEST_CASE("std::set test", "[set][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::set<int> vWrite;
	vWrite.insert(10);
	vWrite.insert(20);
	vWrite.insert(30);
	sms << vWrite;
	sms.Seek(0);
	std::set<int> vRead;
	sms >> vRead;
	REQUIRE(vRead == vWrite);
	REQUIRE(vRead.size() == 3);
	REQUIRE(vRead.find(10) != vRead.end());
	REQUIRE(vRead.find(20) != vRead.end());
	REQUIRE(vRead.find(30) != vRead.end());
}

TEST_CASE("std::string test", "[string][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::string vWrite("abc");
	sms << vWrite;
	sms.Seek(0);
	std::string vRead;
	sms >> vRead;
	REQUIRE(vRead == vWrite);
	REQUIRE(vRead == "abc");
}

TEST_CASE("std::pair test", "[pair][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::pair<int, float> pWrite(3, 3.14f);
	sms << pWrite;
	sms.Seek(0);
	std::pair<int, float> pRead;
	sms >> pRead;
	REQUIRE(pRead == pWrite);
	REQUIRE(pRead.first == 3);
	REQUIRE(pRead.second == 3.14f);
}

TEST_CASE("c array of simple pod type", "[carray][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	int vWrite[3] = {2, 3, 4};
	sms << vWrite;
	sms.Seek(0);
	int vRead[3];
	sms >> vRead;
	REQUIRE(memcmp(vRead, vWrite, sizeof(vRead)) == 0);
	REQUIRE(vRead[0] == 2);
	REQUIRE(vRead[1] == 3);
	REQUIRE(vRead[2] == 4);
}

TEST_CASE("c array of non pod type", "[carray][static]")
{
	char buff[33];
	bfio::StaticMemoryStream sms(buff, 33);
	std::string vWrite[3] = { "a", "b", "c" };
	sms << vWrite;
	sms.Seek(0);
	std::string vRead[3];
	sms >> vRead;
	REQUIRE(vRead[0] == "a");
	REQUIRE(vRead[1] == "b");
	REQUIRE(vRead[2] == "c");
}

#if BFIO_INCLUDE_GLM
TEST_CASE("GLM", "[GLM][static]")
{
	char buff[128];
	bfio::StaticMemoryStream sms(buff, 33);
	SECTION("vec2")
	{
		sms.Seek(0);
		glm::vec2 vWrite(1.f, 2.f);
		sms << vWrite;
		sms.Seek(0);
		glm::vec2 vRead;
		sms >> vRead;
		REQUIRE(vRead == vWrite);
		REQUIRE(vRead.x == 1.f);
		REQUIRE(vRead.y == 2.0f);
	}
	SECTION("vec3")
	{
		sms.Seek(0);
		glm::vec3 vWrite(1.f, 2.f, 3.f);
		sms << vWrite;
		sms.Seek(0);
		glm::vec3 vRead;
		sms >> vRead;
		REQUIRE(vRead == vWrite);
		REQUIRE(vRead.x == 1.f);
		REQUIRE(vRead.y == 2.0f);
		REQUIRE(vRead.z == 3.0f);
	}
	SECTION("vec4")
	{
		sms.Seek(0);
		glm::vec4 vWrite(1.f, 2.f, 3.f, 4.f);
		sms << vWrite;
		sms.Seek(0);
		glm::vec4 vRead;
		sms >> vRead;
		REQUIRE(vRead == vWrite);
		REQUIRE(vRead.x == 1.f);
		REQUIRE(vRead.y == 2.0f);
		REQUIRE(vRead.z == 3.0f);
		REQUIRE(vRead.w == 4.0f);
	}
	SECTION("ivec4")
	{
		sms.Seek(0);
		glm::ivec4 vWrite(1, 2, 3, 4);
		sms << vWrite;
		sms.Seek(0);
		glm::ivec4 vRead;
		sms >> vRead;
		REQUIRE(vRead == vWrite);
		REQUIRE(vRead.x == 1);
		REQUIRE(vRead.y == 2);
		REQUIRE(vRead.z == 3);
		REQUIRE(vRead.w == 4);
	}
	SECTION("mat2")
	{
		sms.Seek(0);
		glm::mat2 vWrite(1.f, 2.f, 3.f, 4.f);
		sms << vWrite;
		sms.Seek(0);
		glm::mat2 vRead;
		sms >> vRead;
		REQUIRE(vRead == vWrite);
		REQUIRE(vRead[0] == vWrite[0]);
		REQUIRE(vRead[1] == vWrite[1]);
		REQUIRE(vRead[0][0] == vWrite[0][0]);
		REQUIRE(vRead[0][1] == vWrite[0][1]);
		REQUIRE(vRead[1][0] == vWrite[1][0]);
		REQUIRE(vRead[1][1] == vWrite[1][1]);
	}
}

#endif

struct CountStream : public bfio::Stream<CountStream>, public bfio::StaticMemoryStream
{
public:
	CountStream(char* data, size_t size) : StaticMemoryStream(data, size), readCount(0), writeCount(0)
	{}

	bool Write(const char* src, size_t size)
	{
		++writeCount;
		return StaticMemoryStream::Write(src, size);
	}
	bool Read(char* dst, size_t size)
	{
		++readCount;
		return StaticMemoryStream::Read(dst, size);
	}

	size_t writeCount;
	size_t readCount;
};

TEST_CASE("IO access counter test", "[access count]")
{
	char buff[128];
	SECTION("Access count for c array of non pod")
	{
		CountStream stream(buff, 128);
		std::string a[3];

		(bfio::Stream<CountStream>&)stream << a;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> a;

		REQUIRE(stream.readCount != 1);
		REQUIRE(stream.writeCount != 1);
	}

	SECTION("Access count for c array")
	{
		CountStream stream(buff, 128);
		int a[10] = {0};

		(bfio::Stream<CountStream>&)stream << a;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> a;

		REQUIRE(stream.readCount == 1);
		REQUIRE(stream.writeCount == 1);
	}

#if BFIO_INCLUDE_GLM
	SECTION("Access count for GLM vec")
	{
		CountStream stream(buff, 128);
		glm::vec4 v;

		(bfio::Stream<CountStream>&)stream << v;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> v;

		REQUIRE(stream.readCount == 1);
		REQUIRE(stream.writeCount == 1);
	}
	SECTION("Access count for GLM mat")
	{
		CountStream stream(buff, 128);
		glm::mat4 m;

		(bfio::Stream<CountStream>&)stream << m;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> m;

		REQUIRE(stream.readCount == 1);
		REQUIRE(stream.writeCount == 1);
	}
#endif
	SECTION("Access count for string")
	{
		CountStream stream(buff, 128);
		std::string str("asdasd");

		(bfio::Stream<CountStream>&)stream << str;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> str;

		REQUIRE(stream.readCount == 2);
		REQUIRE(stream.writeCount == 2);
	}
	SECTION("Access count for vector of simple pod")
	{
		CountStream stream(buff, 128);
		std::vector<int> v = {1, 2, 3, 4, 1, 2, 3, 4 };

		(bfio::Stream<CountStream>&)stream << v;
		stream.Seek(0);
		(bfio::Stream<CountStream>&)stream >> v;

		REQUIRE(stream.readCount == 2);
		REQUIRE(stream.writeCount == 2);
	}
}


struct MyData
{
	std::map<int, std::string> m;
	std::string str;
	std::vector<std::pair<std::string, float> > v;
	int a[3];

	bool operator ==(const MyData& other)
	{
		return m == other.m && str == other.str && v == v && memcmp(a, other.a, sizeof(a)) == 0;
	}
};

namespace bfio
{
	template<class A>
	inline void Serialize(A& io, MyData& x)
	{
		io & x.m;
		io & x.str;
		io & x.v;
		io & x.a;
	}
}

TEST_CASE("Writing struct to file and reading it back", "[cfile][POD struct][carray][map][pair][vector][string]")
{
	FILE* f = fopen("test.bin", "wb");
	
	MyData dataWritten;
	dataWritten.str = "test_string";
	dataWritten.m[1] = "one";
	dataWritten.m[3] = "three";
	dataWritten.v.push_back(std::make_pair("seven_point_eight", 7.8f));
	dataWritten.v.push_back(std::make_pair("two point three", 2.3f));
	dataWritten.a[0] = 3;
	dataWritten.a[1] = 4;
	dataWritten.a[2] = 6;

	bfio::CFileStream(f) << dataWritten;
	
	fclose(f);

	f = fopen("test.bin", "rb");
	bfio::CFileStream inStream(f);

	MyData dataRead;

	bfio::CFileStream(f) >> dataRead;

	fclose(f);

	REQUIRE(dataWritten == dataRead);

	REQUIRE(dataRead.a[0] == 3);
	REQUIRE(dataRead.a[1] == 4);
	REQUIRE(dataRead.a[2] == 6);
	REQUIRE(dataRead.m[1] == "one");
	REQUIRE(dataRead.m[3] == "three");
	REQUIRE(dataRead.m.size() == 2);
	REQUIRE(dataRead.v[0].first == "seven_point_eight");
	REQUIRE(dataRead.v[1].first == "two point three");
	REQUIRE(dataRead.v[0].second == 7.8f);
	REQUIRE(dataRead.v[1].second == 2.3f);
	REQUIRE(dataRead.v.size() == 2);
	REQUIRE(dataRead.str == "test_string");
}
