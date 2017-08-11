#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace bfio
{
	enum AccessType
	{
		Reading,
		Writing
	};


	template<typename StreamType>
	class Stream
	{
	public:
		template<typename T>
		inline void operator << (const T& object)
		{
			StreamType& stream_ = static_cast<StreamType&>(*this);
			Accessor<StreamType, Writing> accessor(stream_);
			accessor & const_cast<T&>(object);
		}

		template<typename T>
		inline void operator >> (const T& object)
		{
			StreamType& stream_ = static_cast<StreamType&>(*this);
			Accessor<StreamType, Reading> accessor(stream_);
			accessor & const_cast<T&>(object);
		}
	};


	template<typename Stream, typename D>
	class AccessorBase
	{
	public:
		AccessorBase(Stream& stream) :stream(stream)
		{}

		template<typename T>
		void operator & (T& x)
		{
			Serialize(static_cast<D&>(*this), x);
		}

		template<typename T, size_t N>
		void operator & (T(&x)[N])
		{
			for (size_t i = 0; i < N; ++i)
			{
				*this & x[i];
			}
		}
		
	protected:
		Stream& stream;
	};


	template<typename Stream, AccessType accessType>
	class Accessor;
	

	template<typename Stream>
	class Accessor<Stream, Reading> : public AccessorBase<Stream, Accessor<Stream, Reading> >
	{
	public:
		Accessor(Stream& stream) :AccessorBase<Stream, Accessor<Stream, Reading> >(stream)
		{}
		template<typename T>
		bool Access(T& x)
		{
			return stream.Read(reinterpret_cast<char*>(&x), sizeof(T));
		}
	};

	
	template<typename Stream>
	class Accessor<Stream, Writing> : public AccessorBase<Stream, Accessor<Stream, Writing> >
	{
	public:
		Accessor(Stream& stream) : AccessorBase<Stream, Accessor<Stream, Writing> >(stream)
		{}
		template<typename T>
		bool Access(T& x)
		{
			return stream.Write(reinterpret_cast<const char*>(&x), sizeof(T));
		}
	};


	template<class A>
	inline void Serialize(A& io, char& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, short& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, int& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, long& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, long long& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, unsigned char& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, unsigned short& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, unsigned int& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, unsigned long& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, unsigned long long& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, float& v)
	{
		io.Access(v);
	}

	template<class A>
	inline void Serialize(A& io, double& v)
	{
		io.Access(v);
	}

	template<class A, typename T1, typename T2>
	inline void Serialize(A& io, std::pair<T1, T2>& v)
	{
		io & v.first;
		io & v.second;
	}

	template<typename T, typename Stream>
	inline void Serialize(Accessor<Stream, Writing>& w, std::vector<T>& v)
	{
		size_t size = v.size();
		w & size;
		for (size_t i = 0, l = v.size(); i < l; ++i)
		{
			w & v[i];
		}
	}

	template<typename T, typename Stream>
	inline void Serialize(Accessor<Stream, Reading>& r, std::vector<T>& v)
	{
		size_t size;
		r & size;
		v.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			r & v[i];
		}
	}

	template<typename Stream, typename Key, typename Val>
	inline void Serialize(Accessor<Stream, Reading>& r, std::map<Key, Val>& x)
	{
		size_t size;
		r & size;
		Key k;
		Val val;
		for (size_t i = 0; i < size; ++i)
		{
			r & k;
			r & val;
			x[k] = val;
		}
	}

	template<typename Stream, typename Key, typename Val>
	inline void Serialize(Accessor<Stream, Writing>& w, std::map<Key, Val>& x)
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

	template<typename Stream, typename Key>
	inline void Serialize(Accessor<Stream, Reading>& w, std::set<Key>& x)
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

	template<typename Stream, typename Key>
	inline void Serialize(Accessor<Stream, Writing>& w, std::set<Key>& x)
	{
		size_t size = x.size();
		w & size;
		for (std::set<Key>::iterator it = x.begin(); it != x.end(); ++it)
		{
			w & *it;
		}
	}

	template<typename Stream>
	inline void Serialize(Accessor<Stream, Writing>& w, std::string& x)
	{
		size_t size = x.size();
		w & size;
		for (size_t i = 0; i < size; ++i)
		{
			w & x[i];
		}
	}

	template<typename Stream>
	inline void Serialize(Accessor<Stream, Reading>& w, std::string& v)
	{
		size_t size;
		w & size;
		v.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			w & v[i];
		}
	}


	class SizeCalculator : public Stream<SizeCalculator>
	{
	public:
		SizeCalculator(): m_size(0)
		{}
		bool Write(const char* dst, size_t size)
		{
			m_size += size;
			return true;
		}
		size_t GetSize() const
		{
			return m_size;
		}
	private:
		size_t m_size;
	};


	template <typename T>
	inline size_t SizeOf()
	{
		SizeCalculator calc;
		Accessor<SizeCalculator, Writing> accessor(calc);
		accessor & *reinterpret_cast<T*>(nullptr);
		return calc.GetSize();
	}


	class CFileStream : public Stream<CFileStream>
	{
	public:
		CFileStream(FILE* f) : file(f)
		{};

		bool Write(const char* src, size_t size)
		{
			return fwrite(src, size, 1, file) == size;
		}
		bool Read(char* dst, size_t size)
		{
			return fread(dst, size, 1, file) == size;
		}

	private:
		FILE* file;
	};


	class MemoryStream
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


	class StaticMemoryStream : public MemoryStream, public Stream<StaticMemoryStream>
	{
	public:
		StaticMemoryStream(char* data, size_t size): MemoryStream(data, size)
		{}

		~StaticMemoryStream()
		{}

		bool Write(const char* src, size_t size)
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

		bool Read(char* dst, size_t size)
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


	class DynamicMemoryStream : public MemoryStream, public Stream<DynamicMemoryStream>
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
		DynamicMemoryStream(const char* data, size_t size) : MemoryStream(nullptr, 0), m_reserved(0)
		{
			Resize(InitialReservedSize < size ? size : InitialReservedSize);
			memcpy(m_data, data, size);
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

		bool Write(const char* src, size_t size)
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

		bool Read(char* dst, size_t size)
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

	private:
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
