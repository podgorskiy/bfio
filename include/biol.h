#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace biol
{
	class OStream
	{
	public:
		virtual bool Write(const char* src, size_t size) = 0;
	};

	class IStream
	{
	public:
		virtual bool Read(char* dst, size_t size) = 0;
	};

	class IOStream : public IStream, public OStream
	{};

	class Writer
	{
	public:
		Writer(OStream& stream) :stream(stream)
		{
		}

		template<typename T>
		void operator & (T& x)
		{
			Serialize(*this, x);
		}

		template<typename T, size_t N>
		void operator & (T(&x)[N])
		{
			for (size_t i = 0; i < N; ++i)
			{
				*this & x[i];
			}
		}

	private:
		template<typename T>
		bool Write(T& x)
		{
			return stream.Write(reinterpret_cast<const char*>(&x), sizeof(T));
		}

		OStream& stream;
	};

	class Reader
	{
	public:
		Reader(IStream& stream) :stream(stream)
		{
		}

		template<typename T>
		void operator & (T& x)
		{
			Serialize(*this, x);
		}

		template<typename T, size_t N>
		void operator & (T(&x)[N])
		{
			for (size_t i = 0; i < N; ++i)
			{
				*this & x[i];
			}
		}

	private:
		template<typename T>
		bool Read(T& x)
		{
			return stream.Read(reinterpret_cast<char*>(&x), sizeof(T));
		}

		IStream& stream;
	};

	template<>
	inline void Writer::operator& (char& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (short& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (int& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (long long& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (long& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (unsigned char& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (unsigned short& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (unsigned int& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (unsigned long long& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (unsigned long& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (double& x)
	{
		Write(x);
	}

	template<>
	inline void Writer::operator& (float& x)
	{
		Write(x);
	}

	template<>
	inline void Reader::operator& (char& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (short& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (int& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (long long& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (long& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (unsigned char& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (unsigned short& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (unsigned int& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (unsigned long long& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (unsigned long& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (double& x)
	{
		Read(x);
	}

	template<>
	inline void Reader::operator& (float& x)
	{
		Read(x);
	}

	template<typename T>
	inline void operator << (OStream& stream, const T& object)
	{
		Serialization::Writer writer(stream);
		writer & const_cast<T&>(object);
	}

	template<typename T>
	inline void operator >> (IStream& stream, T& object)
	{
		Serialization::Reader reader(stream);
		reader & object;
	}

	///

	template<class RW, typename T1, typename T2>
	inline void Serialize(RW& io, std::pair<T1, T2>& v)
	{
		io & v.first;
		io & v.second;
	}

	template<class T>
	inline void Serialize(class Writer& w, std::vector<T>& v)
	{
		size_t size = v.size();
		w & size;
		for (size_t i = 0, l = v.size(); i < l; ++i)
		{
			w & v[i];
		}
	}

	template<class T>
	inline void Serialize(class Reader& w, std::vector<T>& v)
	{
		size_t size;
		w & size;
		v.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			w & v[i];
		}
	}

	template<class Key, class Val>
	inline void Serialize(class Reader& w, std::map<Key, Val>& x)
	{
		size_t size;
		w & size;
		Key k;
		Val val;
		for (size_t i = 0; i < size; ++i)
		{
			w & k;
			w & val;
			x[k] = val;
		}
	}

	template<class Key, class Val>
	inline void Serialize(class Writer& w, std::map<Key, Val>& x)
	{
		size_t size = x.size();
		w & size;
		for (std::map<Key, Val>::iterator it = x.begin(); it != x.end(); ++it)
		{
			Key k = it->first;
			w & k;
			w & it->second;
		}
	}

	template<class Key>
	inline void Serialize(class Reader& w, std::set<Key>& x)
	{
		size_t size;
		w & size;
		Key k;
		for (size_t i = 0; i < size; ++i)
		{
			w & k;
			w & val;
			x[k] = val;
		}
	}

	template<class Key>
	inline void Serialize(class Writer& w, std::set<Key>& x)
	{
		size_t size = x.size();
		w & size;
		for (std::set<Key>::iterator it = x.begin(); it != x.end(); ++it)
		{
			w & *it;
		}
	}

	inline void Serialize(class Writer& w, std::string& x)
	{
		size_t size = x.size();
		w & size;
		for (size_t i = 0; i < size; ++i)
		{
			w & x[i];
		}
	}

	inline void Serialize(class Reader& w, std::string& v)
	{
		size_t size;
		w & size;
		v.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			w & v[i];
		}
	}
}
