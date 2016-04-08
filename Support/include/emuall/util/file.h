#ifndef _FILE_H
#define _FILE_H

#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>
#include <exception>
#include <cstdarg>
#include <emuall/common.h>
#include <emuall/util/endian.h>

class DLLEXPORT std::exception;

class DLLEXPORT FileException : public std::exception
{
public:
	FileException(int error, const char *format, ...) :
		_errno(error)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(_errorMsg, sizeof(_errorMsg), format, args);
		va_end(args);
	}
	virtual ~FileException() { }
	inline const char *what() const { return _errorMsg; }
	inline int why() const { return _errno; }
private:
	errno_t _errno;
	char _errorMsg[256];
};

class DLLEXPORT EndOfFileException : public std::exception
{
public:
	inline const char *what() const { return "End of file was reached"; }
};

class DLLEXPORT File
{
public:

	enum seekOrigins
	{
		seekBeginning = SEEK_SET,
		seekCurrent = SEEK_CUR,
		seekEnd = SEEK_END
	};

	enum fileOpenMode
	{
		openReadOnly = 1,
		openWriteOnly,
		openAppend,
		openRW,
		openRWCreate,
		openAppendRead,
		openWriteOnlyFail,
		openRWCreateFail,
		// Keep raw and text mode seperated
		openRawReadOnly, 
		openRawWriteOnly,
		openRawAppend,
		openRawRW,
		openRawRWCreate,
		openRawAppendRead,
		openRawWriteOnlyFail,
		openRawRWCreateFail,
	};

	File(const char *filename, fileOpenMode mode = openReadOnly);
	File(File &other);
	File();
	virtual ~File();

	File &operator=(File &other);

	bool Seek(long offset, seekOrigins origin = seekCurrent);
	long int GetPosition();
	long int GetSize();
	bool IsEOF() const;

	bool Open();
	bool Open(fileOpenMode mode);
	bool IsOpen() const;
	bool Close();
		
	errno_t GetLastErrno() const; 
	void ClearErrno();

	size_t Read(void *ptr, size_t size);
	size_t Write(const void *ptr, size_t size);
	int GetC();
	bool GetC(unsigned char *c);
	bool GetC(char *c);
	bool PutC(unsigned char c);
	bool PutC(char c);
protected:
	static const char *GetOpenModeString(fileOpenMode mode);
private:
	class Prvt;
	Prvt *_prvt;
};

class DLLEXPORT RawFile : public File
{
public:

	RawFile(const char *filepath, fileOpenMode mode = File::openReadOnly, bool littleEndian = true);
	RawFile(bool littleEndian = true);
	RawFile(RawFile &other);
	virtual ~RawFile();

	RawFile &operator=(RawFile &other);

	uint8_t GetU8();
	int8_t GetI8();
	uint16_t GetU16();
	int16_t GetI16();
	uint32_t GetU24();
	int32_t GetI24();
	uint32_t GetU32();
	int32_t GetI32();
	uint64_t GetU64();
	int64_t GetI64();
	float GetFloat();
	double GetDouble();
	void PutU8(uint8_t x);
	void PutI8(int8_t x);
	void PutU16(uint16_t x);
	void PutI16(int16_t x);
	void PutU24(uint32_t x);
	void PutI24(int32_t x);
	void PutU32(uint32_t x);
	void PutI32(int32_t x);
	void PutU64(uint64_t x);
	void PutI64(int64_t x);
	void PutFloat(float x);
	void PutDouble(double x);

private:
	Endian _conv;
};

#endif