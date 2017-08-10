#include <bfio.h>

class MyStream : public bfio::IOStream
{
public:
	MyStream(FILE* f) : file(f)
	{};

private:
	virtual bool Write(const char* src, size_t size) override
	{
		return fwrite(src, size, 1, file) == size;
	}
	virtual bool Read(char* dst, size_t size) override
	{
		return fread(dst, size, 1, file) == size;
	}

	FILE* file;
};

struct MyData
{
	std::map<int, std::string> m;
	std::string str;
	std::vector<std::pair<std::string, float> > v;

	int a[10];
};


namespace bfio
{
	template<class RW>
	inline void Serialize(RW& io, MyData& x)
	{
		io & x.m;
		io & x.str;
		io & x.v;
		io & x.a;
	}
}

void main()
{
	{
		FILE* f = fopen("test.bin", "wb");

		MyData md;
		md.str = "asdasd";
		md.m[1] = "asd";
		md.m[3] = "test";
		md.v.push_back(std::make_pair("asd", 7.8f));
		md.v.push_back(std::make_pair("asd", 7.8f));
		md.a[0] = -3;
		md.a[1] = 1;

		MyStream(f) << md;

		MyStream(f) << std::string("asdasd");

		fclose(f);
	}

	{
		FILE* f = fopen("test.bin", "rb");

		MyData md;

		MyStream(f) >> md;

		std::string str;

		MyStream(f) >> str;

		fclose(f);
	}
	{
		bfio::DynamicMemoryStream dms;
		{
			MyData md;
			md.str = "asdasd";
			md.m[1] = "asd";
			md.m[3] = "test";
			md.v.push_back(std::make_pair("asd", 7.8f));
			md.v.push_back(std::make_pair("asd", 7.8f));
			md.a[0] = -3;
			md.a[1] = 1;

			dms << md;

			dms << std::string("asdasd");
		}

		dms.Seek(0);

		{
			MyData md;
			dms >> md;

			std::string str;
			dms >> str;
		}
	}
	{
		char* data = new char[512];
		bfio::StaticMemoryStream sms(data, 512);
		{
			MyData md;
			md.str = "asdasd";
			md.m[1] = "asd";
			md.m[3] = "test";
			md.v.push_back(std::make_pair("asd", 7.8f));
			md.v.push_back(std::make_pair("asd", 7.8f));
			md.a[0] = -3;
			md.a[1] = 1;

			sms << md;

			sms << std::string("asdasd");
		}

		sms.Seek(0);

		{
			MyData md;
			sms >> md;

			std::string str;
			sms >> str;
		}
		delete[] data;
	}
}