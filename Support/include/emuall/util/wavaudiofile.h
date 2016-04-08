#ifndef _WAVAUDIOFILE_H
#define _WAVAUDIOFILE_H

#include <emuall/util/file.h>

#include <list>
#include <vector>
#include <exception>
#include <cstdarg>
#include <emuall/common.h>

class DLLEXPORT WavChunk
{
public:
	WavChunk(RawFile &file);
	WavChunk(const char *id);
	virtual ~WavChunk();

	virtual size_t Size() const { return 8 + _size; }
	virtual size_t ContentSize() const{ return _size; }

	long int GetLocation() { return _location; }
	bool IsID(const char *) const;
	void GotoPosition(RawFile &file) { file.Seek(_location, File::seekBeginning); }
	void Skip(RawFile &file) { file.Seek(_location+_size+8, File::seekBeginning); }
	virtual void WriteToFile(RawFile &file) const;
	virtual void ReadFromFile(RawFile &file);
protected:
	char _id[4];
	uint32_t _size;
	long int _location;
};

class DLLEXPORT WavUnkChunk : public WavChunk
{
public:
	WavUnkChunk(RawFile &file);
	~WavUnkChunk();

	virtual void WriteToFile(RawFile &file) const;
	virtual void ReadFromFile(RawFile &file);
private:
	void *_payload;
};

class DLLEXPORT WavFactChunk : public WavChunk
{
public:
	WavFactChunk(RawFile &file);
	~WavFactChunk();

	virtual size_t Size() { return 0; }

	virtual void WriteToFile(RawFile &file) const;
	virtual void ReadFromFile(RawFile &file);
private:
	uint32_t _samplesPerChannel;
};

class DLLEXPORT WavFmtChunk : public WavChunk
{
public:
	enum CompressionCode
	{
		Unknown = 0,
		PCM,
		ADPCM,
		IEEE_FLOAT
	};

	WavFmtChunk(RawFile &file);
	WavFmtChunk();

	CompressionCode GetCompression() const { return _compression; }
	uint16_t GetChannels() const { return _channels; }
	uint32_t GetSampleRate() const { return _samplerate; }
	uint32_t GetBytesPerSecond() const { return _bytesPerSecond; }
	uint16_t GetBlockAlign() const { return _blockAlign; }
	uint16_t GetBitsPerSample() const { return _bitsPerSample; }

	void SetCompression(CompressionCode comp) { _compression = comp; }
	void SetChannels(uint16_t channels) { _channels = channels; }
	void SetSampleRate(uint32_t sr) { _samplerate = sr; }
	void SetBytesPerSecond(uint32_t bps) { _bytesPerSecond = bps; }
	void SetBlockAlign(uint16_t ba) { _blockAlign = ba; }
	void SetBitsPerSample(uint16_t bps) { _bitsPerSample = bps; }

	virtual void WriteToFile(RawFile &file) const;
	virtual void ReadFromFile(RawFile &file);

private:
	CompressionCode _compression;
	uint16_t _channels;
	uint32_t _samplerate;
	uint32_t _bytesPerSecond;
	uint16_t _blockAlign;
	uint16_t _bitsPerSample;
};
	
class DLLEXPORT WavDataChunk : public WavChunk
{
public:
	WavDataChunk(RawFile &file, WavFmtChunk *format = NULL);
	WavDataChunk(WavFmtChunk *format = NULL);
	virtual ~WavDataChunk();

	void SetWavFmt(const WavFmtChunk *format) {_format = format;}
	const WavFmtChunk* GetWavFmt() const { return _format; }
	void LoadData(RawFile &file);
	void ClearSamples();
	unsigned int Count() const { return _count; }
	unsigned int ReadSamples(int16_t *buffer, unsigned int pos, unsigned int samples) const;
	unsigned int WriteSamples(int16_t *buffer, unsigned int samples);
	WavFmtChunk GetOutputFormat() const;

	virtual size_t Size() { return 8 + _count * GetOutputFormat().GetBlockAlign(); }

	virtual void WriteToFile(RawFile &file) const;
	virtual void ReadFromFile(RawFile &file);
private: 
	unsigned int _count;
	const WavFmtChunk *_format;
	std::vector<int16_t *> *_audioSamples;
};
	
class DLLEXPORT WavException : public std::exception
{
public:
	WavException(const char *format, ...);
	virtual ~WavException() { }
	const char *what() const { return errorMsg;}
private: 
	char errorMsg[256];
};

class DLLEXPORT WavFile: private WavChunk
{
public:

	WavFile();
	WavFile(const char *path, bool create);
	virtual ~WavFile();

	void OpenWavFile(const char *path, bool create);
		
	unsigned int ReadPCM16(int16_t *buffer, unsigned int samples);
	unsigned int GetSampleRate() const;
	unsigned int GetChannels() const;
	unsigned int Count() const;

	void SetFormat(unsigned int sampleRate, unsigned short channels);
	unsigned int WritePCM(int16_t *buffer, unsigned int samples);
	void WriteToFile(const char *path = NULL);

private:
	virtual void Release();
	void ReadChunks();

	unsigned int _position;
	WavFmtChunk *_waveFormatInfo;
	WavDataChunk *_waveData;
	std::list<WavChunk *> *_chunks;
	RawFile _file;
};

class DLLEXPORT WavFileInStream : private WavChunk
{
public:
	WavFileInStream(const char *path);
	virtual ~WavFileInStream();

	unsigned int ReadFloat(float *buffer, unsigned int samples);
	unsigned int ReadPCM16(int16_t *buffer, unsigned int samples);
	unsigned int GetBitsPerSample() const;
	unsigned int GetSampleRate() const;
	unsigned int GetChannels() const;
	unsigned int Count() const;

private:
	unsigned int Read(void* buffer, unsigned int samples);
	virtual void Release();
	void OpenWavFile(const char *path);
	void ReadChunks();

	size_t _position;
	size_t _dataOffset;
	size_t _dataSize;
	WavFmtChunk *_waveFormatInfo;
	RawFile _file;
};

class DLLEXPORT WavFileOutStream
{
public:
	enum OutFormat{
		PCM8,
		PCM16,
		PCM32,
		IEEE_FLOAT,
		IEEE_DOUBLE
	};

	WavFileOutStream(const char *path, OutFormat compression, unsigned int sampleRate, unsigned int channels);
	virtual ~WavFileOutStream();


	unsigned int WriteFloat(float *buffer, unsigned int samples);
	unsigned int WritePCM8(uint8_t *buffer, unsigned int samples);
	unsigned int WritePCM16(int16_t *buffer, unsigned int samples);
	unsigned int GetBitsPerSample() const;
	unsigned int GetSampleRate() const;
	unsigned int GetChannels() const;
	unsigned int Count() const;

private:
	unsigned int Write(void* buffer, unsigned int samples);
	virtual void Release();
	void CreateWavFile(const char *path);
	void SetFormat(WavFmtChunk::CompressionCode compression, unsigned int bitsPerSample, unsigned int sampleRate, unsigned int channels);

	size_t _dataOffset;
	unsigned int _dataSize;
	WavFmtChunk *_waveFormatInfo;
	size_t _factOffset;
	std::list<WavChunk *> *_chunks;
	RawFile _file;
};


#endif