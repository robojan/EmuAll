
#include <emuall/util/file.h>
#include <cerrno>
#include <vector>

class File::Prvt {
public:
	std::shared_ptr<FILE> _file;
	std::string _filename;
	fileOpenMode _mode;
	errno_t _lastErrno;
};

File::File(const char *filename, fileOpenMode mode)
{
	_prvt = new Prvt;
	_prvt->_filename = filename;
	_prvt->_mode = mode;
	_prvt->_lastErrno = 0;
	_prvt->_file = nullptr;
	Open();
}

File::File() 
{
	_prvt = new Prvt;
	_prvt->_mode = openReadOnly;
	_prvt->_lastErrno = 0;
	_prvt->_file = nullptr;
}

File::File(File &other)
{
	_prvt = other._prvt;
	other._prvt = new Prvt;
	_prvt->_mode = openReadOnly;
	_prvt->_lastErrno = 0;
	_prvt->_file = nullptr;
}

File::~File()
{
	Close();
	delete _prvt;
}

bool File::Seek(long offset, File::seekOrigins origin)
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		return false;
	}
	size_t pos = ftell(_prvt->_file.get());
	if ((origin == File::seekBeginning && offset == pos) || (origin == File::seekCurrent && offset == 0))
		return true;
	if (fseek(_prvt->_file.get(), offset, origin) != 0)
	{
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File seek failed: %s", strerror(_prvt->_lastErrno));
	}
	return true;
}

long int File::GetPosition()
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
		return -1L;
	}
	return ftell(_prvt->_file.get());
}

long int File::GetSize()
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	long int currentPos = ftell(_prvt->_file.get());
	bool success = Seek(0, seekEnd);
	long int endPos = ftell(_prvt->_file.get());
	success &= Seek(currentPos, seekBeginning);
	if (success)
	{
		return endPos;
	}
	_prvt->_lastErrno = errno;
	throw FileException(_prvt->_lastErrno, "Getting file size failed: %s", strerror(_prvt->_lastErrno));
}

bool File::IsEOF() const
{
	return _prvt->_file.get() == NULL || feof(_prvt->_file.get()) != 0;
}

bool File::Open()
{
	if (_prvt->_filename.empty() || _prvt->_mode == 0)
	{
		_prvt->_lastErrno = errno = EINVAL;
		throw FileException(_prvt->_lastErrno, "No filename or mode was given");
	}
	if (_prvt->_file.get() != NULL)
	{
		FILE *tmp = freopen(_prvt->_filename.c_str(), GetOpenModeString(_prvt->_mode), _prvt->_file.get());
		_prvt->_file = std::shared_ptr<FILE>(tmp, std::fclose);
	}
	else {
		FILE *tmp = fopen(_prvt->_filename.c_str(), GetOpenModeString(_prvt->_mode));
		_prvt->_file = std::shared_ptr<FILE>(tmp, std::fclose);
	}
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "Could not open file: %s", strerror(_prvt->_lastErrno));
	}
	return true;
}

bool File::Open(fileOpenMode mode)
{
	_prvt->_mode = mode;
	if (_prvt->_file.get() != NULL)
	{
		if (freopen(_prvt->_filename.c_str(), GetOpenModeString(_prvt->_mode), _prvt->_file.get()) == NULL)
		{
			_prvt->_lastErrno = errno;
			throw FileException(_prvt->_lastErrno, "Could not open file: %s", strerror(_prvt->_lastErrno));
		}
		return true;
	}
	else {
		return Open();
	}
}

bool File::IsOpen() const
{
	return _prvt->_file.get() != NULL;
}

bool File::Close()
{
	_prvt->_file.reset();
	return true;
}

size_t File::Read(void *ptr, size_t size)
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	size_t result = fread(ptr, 1, size, _prvt->_file.get());
	if (result != size)
	{
		if (IsEOF())
		{
			throw EndOfFileException();
		}
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File reading failed: %s", strerror(_prvt->_lastErrno));
	}
	return result;
}

size_t File::Write(const void *ptr, size_t size)
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	size_t result = fwrite(ptr, 1, size, _prvt->_file.get());
	if (result != size)
	{
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File writing failed: %s", strerror(_prvt->_lastErrno));
	}
	return result;
}

int File::GetC()
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	int result = fgetc(_prvt->_file.get());
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
		_prvt->_lastErrno = errno = EINVAL;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	if ((result = GetC()) == EOF)
	{
		if (IsEOF())
		{
			throw EndOfFileException();
		}
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File reading failed: %s", strerror(_prvt->_lastErrno));
	}
	*c = (unsigned char)result;
	return true;
}

bool File::GetC(char *c)
{
	int result;
	if (c == NULL)
	{
		_prvt->_lastErrno = errno = EINVAL;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	if ((result = GetC()) == EOF)
	{
		if (IsEOF())
		{
			throw EndOfFileException();
		}
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File reading failed: %s", strerror(_prvt->_lastErrno));
	}
	*c = (char) result;
	return true;
}

bool File::PutC(unsigned char c)
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	if (fputc(c, _prvt->_file.get()) != c)
	{
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File writing failed: %s", strerror(_prvt->_lastErrno));
	}
	return true;
}

bool File::PutC(char c)
{
	if (_prvt->_file.get() == NULL)
	{
		_prvt->_lastErrno = errno = ENOSR;
		throw FileException(_prvt->_lastErrno, "File not opened: %s", strerror(_prvt->_lastErrno));
	}
	if (fputc(c, _prvt->_file.get()) != c)
	{
		_prvt->_lastErrno = errno;
		throw FileException(_prvt->_lastErrno, "File writing failed: %s", strerror(_prvt->_lastErrno));
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

errno_t File::GetLastErrno() const
{
	return _prvt->_lastErrno;
}

void File::ClearErrno()
{
	_prvt->_lastErrno = 0;
}

File & File::operator=(File &other)
{
	_prvt = other._prvt;
	other._prvt = new Prvt;
	return *this;
}

RawFile::RawFile(const char *filepath, fileOpenMode mode, bool littleEndian) : 
	File(filepath, mode), _conv(!littleEndian)
{
}

RawFile::RawFile(bool littleEndian) :
	File(), _conv(!littleEndian)
{
}

RawFile::RawFile(RawFile &other) :
	File(other), _conv(other._conv)
{

}

RawFile::~RawFile()
{
}

RawFile & RawFile::operator=(RawFile &other)
{
	File::operator=(other);
	_conv = other._conv;
	return *this;
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
	return _conv.convu16(x);
}

int16_t RawFile::GetI16()
{
	int16_t x;
	Read(&x, sizeof(x));
	return _conv.convi16(x);
}

uint32_t RawFile::GetU24()
{
	uint32_t x = 0;
	GetC(((uint8_t *) &x) + 0);
	GetC(((uint8_t *) &x) + 1);
	GetC(((uint8_t *) &x) + 2);
	return _conv.convu32(x);
}

int32_t RawFile::GetI24()
{
	int32_t x;
	GetC(((uint8_t *) &x) + 0);
	GetC(((uint8_t *) &x) + 1);
	GetC(((uint8_t *) &x) + 2);
	return _conv.convi32(x);
}

uint32_t RawFile::GetU32()
{
	uint32_t x;
	Read(&x, sizeof(x));
	return _conv.convu32(x);
}

int32_t RawFile::GetI32()
{
	int32_t x;
	Read(&x, sizeof(x));
	return _conv.convi32(x);
}

uint64_t RawFile::GetU64()
{
	uint64_t x;
	Read(&x, sizeof(x));
	return _conv.convu64(x);
}

int64_t RawFile::GetI64()
{
	int64_t x;
	Read(&x, sizeof(x));
	return _conv.convi64(x);
}

float RawFile::GetFloat()
{
	float x;
	Read(&x, sizeof(x));
	return _conv.convf(x);
}

double RawFile::GetDouble()
{
	double x;
	Read(&x, sizeof(x));
	return _conv.convd(x);
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
	uint16_t t = _conv.convu16(x);
	Write(&t, sizeof(t));
}

void RawFile::PutI16(int16_t x)
{
	int16_t t = _conv.convi16(x);
	Write(&t, sizeof(t));
}

void RawFile::PutU24(uint32_t x)
{
	uint32_t t = _conv.convu32(x);
	Write(&t, 3);
}

void RawFile::PutI24(int32_t x)
{
	int32_t t = _conv.convi32(x);
	Write(&t, 3);
}

void RawFile::PutU32(uint32_t x)
{
	uint32_t t = _conv.convu32(x);
	Write(&t, sizeof(t));
}

void RawFile::PutI32(int32_t x)
{
	int32_t t = _conv.convi32(x);
	Write(&t, sizeof(t));
}

void RawFile::PutU64(uint64_t x)
{
	uint64_t t = _conv.convu64(x);
	Write(&t, sizeof(t));
}

void RawFile::PutI64(int64_t x)
{
	int64_t t = _conv.convi64(x);
	Write(&t, sizeof(t));
}

void RawFile::PutFloat(float x)
{
	float t = _conv.convf(x);
	Write(&t, sizeof(t));
}

void RawFile::PutDouble(double x)
{
	double t = _conv.convd(x);
	Write(&t, sizeof(t));
}