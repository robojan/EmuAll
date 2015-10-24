#define _CRT_SECURE_NO_WARNINGS
#include "file.h"
#include <cerrno>


namespace robojan
{
	FileException::FileException(int error, const char *format, ...) : 
		_errno(error) 
	{ 
		va_list args; 
		va_start(args, format);	
		vsnprintf(_errorMsg, 
		sizeof(_errorMsg), format, args);
		va_end(args); 
	}

	File::File(const char *filename, fileOpenMode mode) :
		_filename(filename), _mode(mode), _lastErrno(0)
	{
		Open();
	}

	File::File(const char *filename) :
		_filename(filename), _mode(), _lastErrno(0)
	{
		
	}

	File::File() :
		_filename(), _mode(), _lastErrno(0)
	{

	}

	File::~File()
	{
		Close();
	}

	bool File::Seek(long int offset, File::seekOrigins origin)
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			return false;
		}
		long int pos = ftell(_file.get());
		if ((origin == File::seekBeginning && offset == pos) || (origin == File::seekCurrent && offset == 0))
			return true;
		if (fseek(_file.get(), offset, origin) != 0)
		{
			_lastErrno = errno;
			throw FileException(_lastErrno, "File seek failed: %s", strerror(_lastErrno));
		}
		return true;
	}

	long int File::GetPosition()
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
			return -1L;
		}
		return ftell(_file.get());
	}

	long int File::GetSize()
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		long int currentPos = ftell(_file.get());
		bool success = Seek(0, seekEnd);
		long int endPos = ftell(_file.get());
		success &= Seek(currentPos, seekBeginning);
		if (success)
		{
			return endPos;
		}
		_lastErrno = errno;
		throw FileException(_lastErrno, "Getting file size failed: %s", strerror(_lastErrno));
	}

	bool File::IsEOF() const
	{
		return _file.get() == NULL || feof(_file.get()) != 0;
	}

	bool File::Open()
	{
		if (_filename.empty() || _mode == 0)
		{
			_lastErrno = errno = EINVAL;
			throw FileException(_lastErrno, "No filename or mode was given");
		}
		if (_file.get() != NULL)
		{
			FILE *tmp = freopen(_filename.c_str(), GetOpenModeString(_mode), _file.get());
			_file = std::shared_ptr<FILE>(tmp, std::fclose);
		}
		else {
			FILE *tmp = fopen(_filename.c_str(), GetOpenModeString(_mode));
			_file = std::shared_ptr<FILE>(tmp, std::fclose);
		}
		if (_file.get() == NULL)
		{
			_lastErrno = errno;
			throw FileException(_lastErrno, "Could not open file: %s", strerror(_lastErrno));
		}
		return true;
	}

	bool File::Open(fileOpenMode mode)
	{
		_mode = mode;
		if (_file.get() != NULL)
		{
			if (freopen(_filename.c_str(), GetOpenModeString(_mode), _file.get()) == NULL)
			{
				_lastErrno = errno;
				throw FileException(_lastErrno, "Could not open file: %s", strerror(_lastErrno));
			}
			return true;
		}
		else {
			return Open();
		}
	}

	bool File::IsOpen() const
	{
		return _file.get() != NULL;
	}

	bool File::Close()
	{
		_file.reset();
		return true;
	}

	size_t File::Read(void *ptr, size_t size)
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		int result = fread(ptr, 1, size, _file.get());
		if (result != size)
		{
			if (IsEOF())
			{
				throw EndOfFileException();
			}
			_lastErrno = errno;
			throw FileException(_lastErrno, "File reading failed: %s", strerror(_lastErrno));
		}
		return result;
	}

	size_t File::Write(const void *ptr, size_t size)
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		int result = fwrite(ptr, 1, size, _file.get());
		if (result != size)
		{
			_lastErrno = errno;
			throw FileException(_lastErrno, "File writing failed: %s", strerror(_lastErrno));
		}
		return result;
	}

	int File::GetC()
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		int result = fgetc(_file.get());
		if (IsEOF())
		{
			throw EndOfFileException();
		}
		return result;
	}

	bool File::GetC(unsigned char *c)
	{
		int result;
		if (c == NULL)
		{
			_lastErrno = errno = EINVAL;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		if ((result = GetC()) == EOF)
		{
			if (IsEOF())
			{
				throw EndOfFileException();
			}
			_lastErrno = errno;
			throw FileException(_lastErrno, "File reading failed: %s", strerror(_lastErrno));
		}
		*c = (unsigned char)result;
		return true;
	}

	bool File::GetC(char *c)
	{
		int result;
		if (c == NULL)
		{
			_lastErrno = errno = EINVAL;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		if ((result = GetC()) == EOF)
		{
			if (IsEOF())
			{
				throw EndOfFileException();
			}
			_lastErrno = errno;
			throw FileException(_lastErrno, "File reading failed: %s", strerror(_lastErrno));
		}
		*c = (char) result;
		return true;
	}

	bool File::PutC(unsigned char c)
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		if (fputc(c, _file.get()) != c)
		{
			_lastErrno = errno;
			throw FileException(_lastErrno, "File writing failed: %s", strerror(_lastErrno));
		}
		return true;
	}

	bool File::PutC(char c)
	{
		if (_file.get() == NULL)
		{
			_lastErrno = errno = ENOSR;
			throw FileException(_lastErrno, "File not opened: %s", strerror(_lastErrno));
		}
		if (fputc(c, _file.get()) != c)
		{
			_lastErrno = errno;
			throw FileException(_lastErrno, "File writing failed: %s", strerror(_lastErrno));
		}
		return true;
	}


	const char *File::GetOpenModeString(fileOpenMode mode)
	{
		switch (mode)
		{
		case openReadOnly: return "r";
		case openWriteOnly: return "w";
		case openAppend: return "a";
		case openRW: return "r+";
		case openRWCreate: return "w+";
		case openAppendRead: return "a+";
		case openWriteOnlyFail: return "wx";
		case openRWCreateFail: return "w+x";
		// Keep raw and text mode seperated
		case openRawReadOnly: return "rb";
		case openRawWriteOnly: return "wb";
		case openRawAppend: return "ab";
		case openRawRW: return "rb+";
		case openRawRWCreate: return "wb+";
		case openRawAppendRead: return "ab+";
		case openRawWriteOnlyFail: return "wbx";
		case openRawRWCreateFail: return "wb+x";
		default: return "rb";
		}
	}

	RawFile::RawFile(const char *filepath, fileOpenMode mode, bool littleEndian) : 
		File(filepath, mode), _conv(NULL)
	{
		_conv = getEndianFuncs(littleEndian ? 0 : 1);
	}

	RawFile::RawFile(const char *filepath, bool littleEndian) : 
		File(filepath), _conv(NULL)
	{
		_conv = getEndianFuncs(littleEndian ? 0 : 1);
	}

	RawFile::RawFile(bool littleEndian) :
		File(), _conv(NULL)
	{
		_conv = getEndianFuncs(littleEndian ? 0 : 1);
	}

	RawFile::~RawFile()
	{
	}

	uint8_t RawFile::GetU8()
	{
		uint8_t x;
		Read(&x, sizeof(x));
		return x;
	}

	int8_t RawFile::GetI8()
	{
		int8_t x;
		Read(&x, sizeof(x));
		return x;
	}

	uint16_t RawFile::GetU16()
	{
		uint16_t x;
		Read(&x, sizeof(x));
		return _conv->convu16(x);
	}

	int16_t RawFile::GetI16()
	{
		int16_t x;
		Read(&x, sizeof(x));
		return _conv->convi16(x);
	}

	uint32_t RawFile::GetU24()
	{
		uint32_t x = 0;
		GetC(((uint8_t *) &x) + 0);
		GetC(((uint8_t *) &x) + 1);
		GetC(((uint8_t *) &x) + 2);
		return _conv->convu32(x);
	}

	int32_t RawFile::GetI24()
	{
		int32_t x;
		GetC(((uint8_t *) &x) + 0);
		GetC(((uint8_t *) &x) + 1);
		GetC(((uint8_t *) &x) + 2);
		return _conv->convi32(x);
	}

	uint32_t RawFile::GetU32()
	{
		uint32_t x;
		Read(&x, sizeof(x));
		return _conv->convu32(x);
	}

	int32_t RawFile::GetI32()
	{
		int32_t x;
		Read(&x, sizeof(x));
		return _conv->convi32(x);
	}

	uint64_t RawFile::GetU64()
	{
		uint64_t x;
		Read(&x, sizeof(x));
		return _conv->convu64(x);
	}

	int64_t RawFile::GetI64()
	{
		int64_t x;
		Read(&x, sizeof(x));
		return _conv->convi64(x);
	}

	float RawFile::GetFloat()
	{
		float x;
		Read(&x, sizeof(x));
		return _conv->convf(x);
	}

	double RawFile::GetDouble()
	{
		double x;
		Read(&x, sizeof(x));
		return _conv->convd(x);
	}

	void RawFile::PutU8(uint8_t x)
	{
		Write(&x, sizeof(x));
	}

	void RawFile::PutI8(int8_t x)
	{
		Write(&x, sizeof(x));
	}

	void RawFile::PutU16(uint16_t x)
	{
		uint16_t t = _conv->convu16(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutI16(int16_t x)
	{
		int16_t t = _conv->convi16(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutU24(uint32_t x)
	{
		uint32_t t = _conv->convu32(x);
		Write(&t, 3);
	}

	void RawFile::PutI24(int32_t x)
	{
		int32_t t = _conv->convi32(x);
		Write(&t, 3);
	}

	void RawFile::PutU32(uint32_t x)
	{
		uint32_t t = _conv->convu32(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutI32(int32_t x)
	{
		int32_t t = _conv->convi32(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutU64(uint64_t x)
	{
		uint64_t t = _conv->convu64(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutI64(int64_t x)
	{
		int64_t t = _conv->convi64(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutFloat(float x)
	{
		float t = _conv->convf(x);
		Write(&t, sizeof(t));
	}

	void RawFile::PutDouble(double x)
	{
		double t = _conv->convd(x);
		Write(&t, sizeof(t));
	}

}