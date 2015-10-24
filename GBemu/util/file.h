#ifndef _FILE_H
#define _FILE_H

#include <cstdio>
#include <cstdint>
#include <string>
#include <memory>
#include <exception>
#include <cstdarg>
#include "Endian.h"

namespace robojan
{
	class FileException : public std::exception
	{
	public:
		FileException(int error, const char *format, ...);
		virtual ~FileException() { }
		const char *what() const { return _errorMsg; }
		int why() const { return _errno; }
	private:
		errno_t _errno;
		char _errorMsg[256];
	};

	class EndOfFileException : public std::exception
	{
	public:
		const char *what() const { return "End of file was reached"; }
	};

	class File
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

		File(const char *filename, fileOpenMode mode);
		File(const char *filename);
		File();
		virtual ~File();

		bool Seek(long int offset, seekOrigins origin = seekCurrent);
		long int GetPosition();
		long int GetSize();
		bool IsEOF() const;

		bool Open();
		bool Open(fileOpenMode mode);
		bool IsOpen() const;
		bool Close();
		
		errno_t GetLastErrno() const { return _lastErrno; }
		void ClearErrno() { _lastErrno = 0; }

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
		std::shared_ptr<FILE> _file;
		std::string _filename;
		fileOpenMode _mode;
		errno_t _lastErrno;
	};

	class RawFile : public File
	{
	public:

		RawFile(const char *filepath, fileOpenMode mode, bool littleEndian = true);
		RawFile(const char *filepath, bool littleEndian = true);
		RawFile(bool littleEndian = true);
		virtual ~RawFile();

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
		const EndianFuncs *_conv;
	};
}

#endif