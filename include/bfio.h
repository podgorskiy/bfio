#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace bfio
{
	class OStream
	{
	public:
		virtual ~OStream() {};
		virtual bool Write(const char* src, size_t size) = 0;
	};

	class IStream
	{
	public:
		virtual ~IStream() {};
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
		bfio::Writer writer(stream);
		writer & const_cast<T&>(object);
	}

	template<typename T>
	inline void operator >> (IStream& stream, T& object)
	{
		bfio::Reader reader(stream);
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

	class CFileStream : public IOStream
	{
	public:
		CFileStream(FILE* f) : file(f)
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

	class MemoryStream : public IOStream
	{
		MemoryStream(const MemoryStream& other) = delete; // non construction-copyable
		MemoryStream& operator=(const MemoryStream&) = delete; // non copyable
	public:
		MemoryStream(char* data, size_t size) : m_size(size), m_offset(0), m_data(data)
		{};

		size_t GetSize() const
		{
			return m_size;
		}

		char* Data()
		{
			return m_data;
		}

		const char* DataConst() const
		{
			return m_data;
		}

		void Seek(size_t position)
		{
			m_offset = position;
		};

		size_t Tell() const
		{
			return m_offset;
		};

	protected:
		char* m_data;
		size_t m_size;
		size_t m_offset;
	};


	class StaticMemoryStream : public MemoryStream
	{
	public:
		StaticMemoryStream(char* data, size_t size): MemoryStream(data, size)
		{}

		~StaticMemoryStream()
		{}

	private:
		virtual bool Write(const char* src, size_t size) override
		{
			if (m_offset + size > m_size)
			{
				memcpy(m_data + m_offset, src, m_size - m_offset);
				m_offset = m_size;
				return false;
			}
			else
			{
				memcpy(m_data + m_offset, src, size);
				m_offset += size;
				return true;
			}
		}

		virtual bool Read(char* dst, size_t size) override
		{
			if (m_offset + size > m_size)
			{
				memcpy(dst, m_data + m_offset, m_size - m_offset);
				m_offset = m_size;
				return false;
			}
			else
			{
				memcpy(dst, m_data + m_offset, size);
				m_offset += size;
				return true;
			}
		}
	};


	class DynamicMemoryStream : public MemoryStream
	{
		enum
		{
			InitialReservedSize = 16
		};
	public:
		DynamicMemoryStream() : MemoryStream(nullptr, 0), m_reserved(0)
		{
			Resize(InitialReservedSize);
		}

		~DynamicMemoryStream()
		{
			free(m_data);
		}

		bool Resize(size_t newSize)
		{
			if (newSize > m_reserved)
			{
				size_t newReserved = PowerOf2RoundUp(newSize);
				char* newData = static_cast<char*>(realloc(m_data, newReserved));
				if (newData != nullptr)
				{
					m_reserved = newReserved;
					m_data = newData;
					m_size = newSize;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				m_size = newSize;
				return true;
			}
		}

	private:
		virtual bool Write(const char* src, size_t size) override
		{
			if (Resize(size + m_offset))
			{
				memcpy(m_data + m_offset, src, size);
				m_offset += size;
				return true;
			}
			else
			{
				return false;
			}
		}

		virtual bool Read(char* dst, size_t size) override
		{
			if (m_offset + size > m_size)
			{
				memcpy(dst, m_data + m_offset, m_size - m_offset);
				m_offset = m_size;
				return false;
			}
			else
			{
				memcpy(dst, m_data + m_offset, size);
				m_offset += size;
				return true;
			}
		}

		// Round up to the next highest power of 2. https://graphics.stanford.edu/~seander/bithacks.html
		size_t PowerOf2RoundUp(uint64_t v)
		{
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v |= v >> 32;
			v++;
			return static_cast<size_t>(v);
		}

		size_t m_reserved;
	};
}
