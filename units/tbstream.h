#pragma once
#include "stdint.h"
#include "stdio.h"
#include "tbstringref.h"
#include "tballoc.h"

namespace tblib
{	
	
	// потому что нужные классы все - зависят от стрима
	// приходится руками ебашить это говнище			
	class _StrBuf
	{
		char* buf;
		_StrBuf (const _StrBuf&);
		_StrBuf& operator = (const _StrBuf&);
	public :
		_StrBuf (StringRef fileName)
		{
			if (fileName.Size()>0)
			{
				buf = reinterpret_cast<char*>(tbAlloc.Malloc(fileName.Size()+1));
				if (buf)
				{
					memcpy(buf, fileName.begin(), fileName.Size());
					buf [fileName.Size()] = 0;
				}
			}	else 
			{
				buf = NULL;
			}
		}

		~_StrBuf ()
		{
			if (buf)
				tbAlloc.Free(buf);
		}

		operator const char* ()
		{
			return buf;
		}
	};

	class IFStream
	{
		FILE* file;
		bool flushed;
		IFStream(const IFStream&);
		IFStream&operator=(const IFStream&);
		mutable int fileVersion;
		mutable int curVersion;
		mutable int processed;

	public :
		
		IFStream (StringRef fileName) : fileVersion(0), curVersion(0), processed(0)
		{
			file = fopen(_StrBuf(fileName), "rb");
		}

		~IFStream ()
		{
			if (file)
				fclose(file);
		}

		long Position ()
		{
			return ftell(file);
		}

		long Size ()
		{
			if (file)
			{
				long cur = ftell(file);
				fseek(file, 0, SEEK_END);
				long result = ftell(file);
				fseek(file, cur, SEEK_SET);
				return result;
			} else
			{
				return 0;
			}
		}

		bool Eof ()
		{
			return (feof(file)!=0);
		}

		bool IsOpen ()
		{
			return file != NULL;
		}

		void SetFileVersion (int version) const
		{
			fileVersion = version;
		}
		
		int GetFileVersion () const
		{
			return fileVersion;
		}

		inline const IFStream& ReadVersion () const;

		const IFStream& Version (int version) const
		{
			curVersion = version;
			return *this;
		}

		size_t Processed () const
		{
			return processed;
		}
		
		template <typename T>
		void BinaryRead (T& t, int count=1) const
		{
			assert (file);

			if (curVersion <= fileVersion) // формат не старый 
			{
				fread(reinterpret_cast<void*>(&t), sizeof(t), count, file);
				processed += sizeof(t);
			}
		}	
	};
	
	class OFStream
	{
		FILE* file;
		mutable int processed;

		OFStream(const OFStream&);
		OFStream&operator=(const OFStream&);
	public :
	
		OFStream (StringRef fileName, bool Append=false) : processed(0)
		{			
			if (Append)
			{
				file = fopen(_StrBuf(fileName), "a+b");
			}	else
			{
				file = fopen(_StrBuf(fileName), "wb");
			}
		}

		~OFStream ()
		{
			if (file)
				fclose(file);
		}	

		long Position ()
		{
			return ftell(file);
		}	

		bool Eof ()
		{
			return (feof(file)!=0);
		}

		bool IsOpen ()
		{
			return file != NULL;
		}		

		int Flush ()
		{
			assert(file);
			return fflush(file);
		}
		
		int Processed () const
		{
			return processed;
		}

		template <typename T>
		void BinaryWrite(const T& t, int count=1) const
		{			
			assert(file);
			fwrite(reinterpret_cast<const void*>(&t), sizeof(t), count, file);
		}
	};

	template <typename T>
	struct To
	{
		static const IFStream& From(const IFStream&s) 
		{ return s; }
	};

	template <typename T>
	struct From;
	// стандартные читатели из класса 
	// для них нельзя писать конструктор из потока!
	// для любого типа - либо FromStream, либо конструктор, но не и то и другое!!!
	
}

#define BINARY_SERIALIZATION(T)\
namespace tblib\
{\
	template <>\
	struct To<T>\
	{\
		static T From(const IFStream &s)\
		{ T t = T(); s.BinaryRead(t); return t; }\
	};\
	inline const OFStream& operator << (const OFStream& s, const T& t)\
	{ s.BinaryWrite(t); return s; }\
}

BINARY_SERIALIZATION(uint64_t);
BINARY_SERIALIZATION(int64_t);
BINARY_SERIALIZATION(uint32_t);
BINARY_SERIALIZATION(int32_t);
BINARY_SERIALIZATION(uint16_t);
BINARY_SERIALIZATION(int16_t);
BINARY_SERIALIZATION(char);
BINARY_SERIALIZATION(bool);



namespace tblib
{
	inline const IFStream& IFStream::ReadVersion () const
	{
		int fv = tblib::To<int>::From(*this);
		SetFileVersion(fv);
		return *this;
	}

	inline const OFStream& operator << (const OFStream&s, const char* c)
	{
		if (c)
		{
			while (*c)
			{
				s << *c;
				++c;
			}
		}
		return s;
	}
}