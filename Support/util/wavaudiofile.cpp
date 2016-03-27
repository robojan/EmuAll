
// Needs to be a multiple of 1, 2, 3, 4 and 8
#define WAVSAMPLEBUFFER 0xF000 

#include <emuall/util/wavaudiofile.h>

#include <cassert>


WavException::WavException(const char *format, ...)
{
	va_list args; 
	va_start(args, format); 
	vsnprintf(errorMsg, sizeof(errorMsg), format, args); 
	va_end(args);
}

WavChunk::WavChunk(RawFile &file)
{
	ReadFromFile(file);
}

WavChunk::WavChunk(const char *id)
{
	assert(id != NULL);
	int i = 0;
	while (*id != NULL && i < 4)
	{
		_id[i] = *id;
		id++;
		i++;
	}
	while (i < 4)
	{
		_id[i] = ' ';
		i++;
	}
	_location = -1;
	_size = 0;
}

WavChunk::~WavChunk()
{

}
	
void WavChunk::WriteToFile(RawFile &file) const
{
	file.PutI8(_id[0]);
	file.PutI8(_id[1]);
	file.PutI8(_id[2]);
	file.PutI8(_id[3]);
	file.PutU32(_size);
}

void WavChunk::ReadFromFile(RawFile &file)
{
	_location = file.GetPosition();
	_id[0] = file.GetI8();
	_id[1] = file.GetI8();
	_id[2] = file.GetI8();
	_id[3] = file.GetI8();
	_size = file.GetU32();
}

bool WavChunk::IsID(const char *id) const
{
	if (id == NULL || strlen(id) < 4)
		return false;
	return _id[0] == id[0] && _id[1] == id[1] && _id[2] == id[2] && _id[3] == id[3];
}

WavUnkChunk::WavUnkChunk(RawFile &file) :
	WavChunk(file), _payload(NULL)
{
	_payload = new char[WavChunk::_size];
	ReadFromFile(file);
}

WavUnkChunk::~WavUnkChunk()
{
	if (_payload != NULL)
		delete[] _payload;
}

void WavUnkChunk::WriteToFile(RawFile &file) const
{
	WavChunk::WriteToFile(file);
	if (_payload != NULL)
	{
		file.Write(_payload, WavChunk::_size);
	}
	else {
		size_t i = WavChunk::_size;
		while (i--)
		{
			file.PutC((char)0);
		}
	}
}

void WavUnkChunk::ReadFromFile(RawFile &file)
{
	//WavChunk::ReadFromFile(file);
	if (_payload != NULL)
		file.Read(_payload, WavChunk::_size);
	file.Seek(WavChunk::_location + WavChunk::_size + 8, File::seekBeginning);
}
	
WavFmtChunk::WavFmtChunk(RawFile &file) :
	WavChunk(file)
{
	ReadFromFile(file);
	WavChunk::_size = 16;
}

WavFmtChunk::WavFmtChunk() :
	WavChunk("fmt ")
{
	WavChunk::_size = 16;
}

void WavFmtChunk::ReadFromFile(RawFile &file)
{
	//WavChunk::ReadFromFile(file);
	_compression = (CompressionCode)file.GetU16();
	_channels = file.GetU16();
	_samplerate = file.GetU32();
	_bytesPerSecond = file.GetU32();
	_blockAlign = file.GetU16();
	_bitsPerSample = file.GetU16();
	// Skip extra format bytes
	file.Seek(WavChunk::_location + WavChunk::_size + 8, File::seekBeginning);

	if (_compression != PCM && _compression != IEEE_FLOAT)
	{
		throw WavException("Only uncompressed wave files are supported");
	}
}

void WavFmtChunk::WriteToFile(RawFile &file) const
{
	WavChunk::WriteToFile(file);
	file.PutU16(_compression);
	file.PutU16(_channels);
	file.PutU32(_samplerate);
	file.PutU32(_bytesPerSecond);
	file.PutU16(_blockAlign);
	file.PutU16(_bitsPerSample);
}

WavDataChunk::WavDataChunk(WavFmtChunk *format) :
	_format(format), _count(0), WavChunk("data")
{
}

WavDataChunk::WavDataChunk(RawFile &file, WavFmtChunk *format) :
	_format(format), _count(0), WavChunk(file)
{
	_audioSamples = new std::vector<int16_t *>;
	ReadFromFile(file);
}

WavDataChunk::~WavDataChunk()
{
	ClearSamples();
	delete _audioSamples;
}

void WavDataChunk::ReadFromFile(RawFile &file)
{
	file.Seek(WavChunk::_location + 8, File::seekBeginning);
	//WavChunk::ReadFromFile(file);
	if (_format != NULL)
	{
		// read data
		LoadData(file);
	}
	// Goto next chunk
	file.Seek(WavChunk::_location + WavChunk::_size + 8, File::seekBeginning);
}

void WavDataChunk::WriteToFile(RawFile &file) const
{
	WavFmtChunk format = GetOutputFormat();
	file.PutC('d'); file.PutC('a'); file.PutC('t'); file.PutC('a');
	file.PutU32(format.GetBlockAlign()*_count);
		
	for (unsigned int i = 0; i < _count; ++i)
	{
		for (int j = 0; j < format.GetChannels(); ++j)
		{
			uint32_t offset = i * format.GetChannels()*sizeof(int16_t) +j * sizeof(int16_t);
			uint32_t bufferIndex = offset / WAVSAMPLEBUFFER;
			uint32_t sampleIndex = offset % WAVSAMPLEBUFFER;
			if (_audioSamples->size() <= bufferIndex)
			{
				throw WavException("Audiosample size is not equal to the sample count");
			}
			int16_t *ptr = (*_audioSamples)[bufferIndex] + (sampleIndex / sizeof(int16_t));
			file.PutI16(*ptr);
		}
	}
}

void WavDataChunk::LoadData(RawFile &file)
{
	WavChunk::GotoPosition(file);
	file.Seek(8); // Goto data

	if (_format == NULL)
		throw WavException("Format chunk is not loaded");

	// Remove the old samples
	ClearSamples();

	unsigned int bytespersample = (_format->GetBitsPerSample()+7)/8;
	if (bytespersample != 4 && _format->GetCompression() == WavFmtChunk::IEEE_FLOAT)
	{
		throw WavException("Samples are not valid floating points");
	}

	// Load the new samples
	_count = WavChunk::_size / _format->GetBlockAlign();
	for (uint32_t i = 0; i < _count; ++i)
	{
		for (int j = 0; j < _format->GetChannels(); ++j)
		{
			uint32_t offset = i * _format->GetChannels()*sizeof(int16_t) + j * sizeof(int16_t);
			uint32_t bufferIndex = offset / WAVSAMPLEBUFFER;
			uint32_t sampleIndex = offset % WAVSAMPLEBUFFER;
			if (_audioSamples->size() <= bufferIndex)
			{
				_audioSamples->push_back(new int16_t[WAVSAMPLEBUFFER / sizeof(int16_t)]);
			}
			int16_t *ptr = (*_audioSamples)[bufferIndex] + (sampleIndex / sizeof(int16_t));
			switch (_format->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (bytespersample)
				{
				case 1:
					*ptr = (int16_t) (file.GetI8() * 256);
					break;
				case 2:
					*ptr = file.GetI16();
					break;
				case 3:
					*ptr = (int16_t) (file.GetI24() / 256);
					break;
				case 4:
					*ptr = (int16_t) (file.GetI32() / (256*256));
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _format->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				*ptr = (int16_t)(file.GetFloat()*INT16_MAX);
				break;
			default:
				throw WavException("Unsupported compression %d used", _format->GetCompression());
			}
		}
	}

}

void WavDataChunk::ClearSamples()
{
	std::vector<int16_t *>::iterator it;
	for (it = _audioSamples->begin(); it != _audioSamples->end(); ++it)
	{
		delete[] * it;
	}
	_audioSamples->clear();
}

unsigned int WavDataChunk::ReadSamples(int16_t *buffer, unsigned int pos, unsigned int samples) const
{
	if (_format == NULL)
		throw WavException("Format chunk is not loaded");

	uint32_t start = pos * _format->GetBlockAlign();
	uint32_t size = samples * sizeof(int16_t) * _format->GetChannels();
	uint32_t offset = start;

	if (pos + size / (sizeof(int16_t) *_format->GetChannels()) > _count)
		size = (_count - pos)*(sizeof(int16_t) *_format->GetChannels());

	while (size > 0)
	{
		uint32_t bufferIndex = offset / WAVSAMPLEBUFFER;
		uint32_t sampleIndex = offset % WAVSAMPLEBUFFER;
		if (size > WAVSAMPLEBUFFER - sampleIndex)
		{
			memcpy(buffer, (*_audioSamples)[bufferIndex] + sampleIndex, WAVSAMPLEBUFFER - sampleIndex);
			size -= WAVSAMPLEBUFFER - sampleIndex;
			offset += WAVSAMPLEBUFFER - sampleIndex;
			buffer += (WAVSAMPLEBUFFER - sampleIndex) / sizeof(int16_t);
		}
		else {
			memcpy(buffer, (*_audioSamples)[bufferIndex] + sampleIndex, size);
			size -= size;
			offset += size;
			buffer += size / sizeof(int16_t);
		}
	}
	return offset-start;
}

unsigned int WavDataChunk::WriteSamples(int16_t *buffer, unsigned int samples)
{
	if (_format == NULL)
		throw WavException("Format chunk is not loaded");
		
	uint32_t start = _count * sizeof(int16_t) * _format->GetChannels();

	for (unsigned int i = 0; i < samples; i++)
	{
		for (int j = 0; j < _format->GetChannels(); ++j)
		{
			uint32_t offset = start + i * _format->GetChannels()*sizeof(int16_t) +j * sizeof(int16_t);
			uint32_t bufferIndex = offset / WAVSAMPLEBUFFER;
			uint32_t sampleIndex = offset % WAVSAMPLEBUFFER;
			if (_audioSamples->size() <= bufferIndex)
			{
				_audioSamples->push_back(new int16_t[WAVSAMPLEBUFFER/sizeof(int16_t)]);
			}
			int16_t *ptr = (*_audioSamples)[bufferIndex] + (sampleIndex / sizeof(int16_t));
			*ptr = *buffer;
			buffer += 1;
		}
	}
	_count += samples;
	return samples;
}

WavFmtChunk WavDataChunk::GetOutputFormat() const
{
	if (_format == NULL)
		throw WavException("No format loaded");
	WavFmtChunk out = *_format;
	out.SetCompression(WavFmtChunk::PCM);
	out.SetBitsPerSample(16);
	out.SetBlockAlign((out.GetBitsPerSample()+7)/8 * out.GetChannels());
	out.SetBytesPerSecond(out.GetBlockAlign()*out.GetSampleRate());
	return out;
}
	
WavFactChunk::WavFactChunk(RawFile &file)
	: WavChunk(file), _samplesPerChannel(0)
{
	ReadFromFile(file);
}

WavFactChunk::~WavFactChunk()
{

}

void WavFactChunk::WriteToFile(RawFile &file) const
{

}

void WavFactChunk::ReadFromFile(RawFile &file)
{
	//WavChunk::ReadFromFile(file);
	_samplesPerChannel = file.GetU32();
}
	
WavFile::WavFile() :
	_file(true), _waveFormatInfo(NULL), _position(0), WavChunk("RIFF"), _waveData(NULL)
{

}

WavFile::WavFile(const char *path, bool create) :
	_file(true), _waveFormatInfo(NULL), _position(0), WavChunk("RIFF"), _waveData(NULL)
{
	_chunks = new std::list<WavChunk *>;
	try
	{
		OpenWavFile(path, create);
	}
	catch (...)
	{
		Release();
		throw;
	}
}

WavFile::~WavFile()
{
	Release();
	delete _chunks;
}

void WavFile::Release()
{
	std::list<WavChunk *>::iterator it;
	for (it = _chunks->begin(); it != _chunks->end(); ++it)
	{
		delete *it;
	}
	_chunks->clear();
	if (_waveData != NULL)
		delete _waveData;
	if (_waveFormatInfo != NULL)
		delete _waveFormatInfo;
}

void WavFile::OpenWavFile(const char *path, bool create)
{
	try {
		if (create)
		{
			// File was not found so we create one
			_file = RawFile(path, File::openRawRWCreate);
			WavChunk::_id[0] = 'R'; WavChunk::_id[1] = 'I'; WavChunk::_id[2] = 'F'; WavChunk::_id[3] = 'F';
			WavChunk::_location = 0;
			WavChunk::_size = 0;
			_waveFormatInfo = new WavFmtChunk();
			_waveFormatInfo->SetBitsPerSample(16);
			_waveFormatInfo->SetCompression(WavFmtChunk::PCM);
			_waveData = new WavDataChunk(_waveFormatInfo);
		}
		else {
			_file = RawFile(path, File::openRawRW, true);
			// Get first chunk
			WavChunk::ReadFromFile(_file);
			char riffType[4];
			riffType[0] = _file.GetI8();
			riffType[1] = _file.GetI8();
			riffType[2] = _file.GetI8();
			riffType[3] = _file.GetI8();
			if (!IsID("RIFF") || riffType[0] != 'W' || riffType[1] != 'A' || riffType[2] != 'V' || riffType[3] != 'E')
			{
				throw WavException("File is not a wave file");
			}
			ReadChunks();
			if (_waveData == NULL)
			{
				_waveData = new WavDataChunk();
			}
			if (_waveData->GetWavFmt() == NULL)
			{
				_waveData->SetWavFmt(_waveFormatInfo);
				_waveData->LoadData(_file);
			}
		}
	}
	catch (EndOfFileException)
	{
		throw WavException("EOF reached before filetype could be determined");
	}

}

void WavFile::ReadChunks()
{
	// Read chunks
	while (!_file.IsEOF())
	{
		try {
			WavChunk chunkHeader(_file);
			WavChunk *chunk;
			chunkHeader.GotoPosition(_file);
			if (chunkHeader.IsID("fmt "))
			{
				if (_waveFormatInfo != NULL)
				{
					throw WavException("Multiple format chunks found");
				}
				_waveFormatInfo = new WavFmtChunk(_file);
				chunk = _waveFormatInfo;
				//_chunks.push_back(chunk); 
			}
			else if (chunkHeader.IsID("data"))
			{
				if (_waveData != NULL)
					throw WavException("Multiple data segments not supported");
				_waveData = new WavDataChunk(_file, _waveFormatInfo);
				chunk = _waveData;
				//_chunks.push_back(chunk);
			}
			else if (chunkHeader.IsID("fact"))
			{
				chunk = new WavFactChunk(_file);
				_chunks->push_back(chunk);
			}
			else {
				chunk = new WavUnkChunk(_file);
				_chunks->push_back(chunk);
			}
		}
		catch (EndOfFileException)
		{
			break;
		}
	}
}

unsigned int WavFile::ReadPCM16(int16_t *buffer, unsigned int samples)
{
	if (_waveData == NULL  || _waveFormatInfo == NULL)
	{
		throw WavException("Tried reading from uninitialized wav file");
	}

	unsigned int samplesRead = _waveData->ReadSamples(buffer, _position, samples);
	_position += samplesRead;

	return samplesRead;
}

unsigned int WavFile::GetSampleRate() const
{
	if (_waveFormatInfo == NULL)
	{
		return 0;
	}
	return _waveFormatInfo->GetSampleRate();
}

unsigned int WavFile::GetChannels() const 
{
	if (_waveFormatInfo == NULL)
	{
		return 0;
	}
	return _waveFormatInfo->GetChannels();
}

unsigned int WavFile::Count() const
{
	if (_waveData == NULL)
	{
		return 0;
	}
	return _waveData->Count();
}


void WavFile::SetFormat(unsigned int sampleRate, unsigned short channels)
{
	if (_waveFormatInfo == NULL)
		throw WavException("Wave format information not loaded");
	_waveFormatInfo->SetSampleRate(sampleRate);
	_waveFormatInfo->SetChannels(channels);
	_waveFormatInfo->SetBlockAlign((_waveFormatInfo->GetBitsPerSample()+7)/8*_waveFormatInfo->GetChannels());
	_waveFormatInfo->SetBytesPerSecond(_waveFormatInfo->GetBlockAlign()*_waveFormatInfo->GetSampleRate());
}

unsigned int WavFile::WritePCM(int16_t *buffer, unsigned int samples)
{
	if (_waveData == NULL || _waveFormatInfo == NULL)
	{
		throw WavException("Tried writing files to uninitialized wav file");
	}

	unsigned int samplesWritten = _waveData->WriteSamples(buffer, samples);

	return samplesWritten;
}

void WavFile::WriteToFile(const char *path) const
{
	RawFile file;
	assert(_waveData != NULL);
		
	if (path == NULL)
	{
		file = _file;
	}
	else {
		file = RawFile(path,File::openRawWriteOnly,true);
	}
	WavFmtChunk format = _waveData->GetOutputFormat();
	std::list<WavChunk *>::const_iterator it;

	size_t size = format.Size();
	size += _waveData->Size();

	for (it = _chunks->cbegin(); it != _chunks->cend(); ++it)
	{
		size += (*it)->Size();
	}

	file.PutC('R');
	file.PutC('I');
	file.PutC('F');
	file.PutC('F');
	file.PutU32(size);
	file.PutC('W');
	file.PutC('A');
	file.PutC('V');
	file.PutC('E');
	format.WriteToFile(file);
	_waveData->WriteToFile(file);

	for (it = _chunks->cbegin(); it != _chunks->cend(); ++it)
	{
		(*it)->WriteToFile(file);
	}
}

WavFileInStream::WavFileInStream(const char *path) : 
	_file(true), _position(0), _dataOffset(0), _dataSize(0),
	_waveFormatInfo(NULL), WavChunk("RIFF")
{
	try
	{
		OpenWavFile(path);
	}
	catch (...)
	{
		Release();
		throw;
	}
}

WavFileInStream::~WavFileInStream()
{
	Release();
}

unsigned int WavFileInStream::Read(void* buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL || _dataSize == 0)
		throw WavException("Tried reading from uninitialized wav file");
		
	if (_position + samples > _dataSize / _waveFormatInfo->GetBlockAlign())
	{
		samples = _dataSize / _waveFormatInfo->GetBlockAlign() - _position;
	}

	_file.Seek(_dataOffset + _position*_waveFormatInfo->GetBlockAlign(), File::seekBeginning); // Goto data
	size_t read =  _file.Read(buffer, samples * _waveFormatInfo->GetBlockAlign());
	_position += read / _waveFormatInfo->GetBlockAlign();
	return read;
}

unsigned int WavFileInStream::ReadFloat(float *buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL || _dataSize == 0)
		throw WavException("Tried reading from uninitialized wav file");

	if (_waveFormatInfo->GetCompression() == WavFmtChunk::IEEE_FLOAT && _waveFormatInfo->GetBitsPerSample() == 32 && Endian::isHostLittleEndian())
		return Read(buffer, samples); // use native
		
	if (_position + samples > _dataSize / _waveFormatInfo->GetBlockAlign())
	{
		samples = _dataSize / _waveFormatInfo->GetBlockAlign() - _position;
	}

	if (_waveFormatInfo->GetCompression() != WavFmtChunk::PCM && _waveFormatInfo->GetCompression() != WavFmtChunk::IEEE_FLOAT)
		throw WavException("Unsupported audio format");

	_file.Seek(_position*_waveFormatInfo->GetBlockAlign() + _dataOffset, File::seekBeginning);

	unsigned int read;
	float *ptr = buffer;
	for (read = 0; read < samples; read++)
	{
		for (int i = 0; i < _waveFormatInfo->GetChannels(); i++)
		{
			switch (_waveFormatInfo->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 8:
					*ptr = (float) (((int16_t) _file.GetU8() - 128) / (float) INT8_MAX);
					break;
				case 16:
					*ptr = (float) (_file.GetI16() / (float) INT16_MAX);
					break;
				case 24:
					*ptr = (float) (_file.GetI24() / 8388607.0);
					break;
				case 32:
					*ptr = (float) (_file.GetI32() / (float) INT32_MAX);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 32:
					*ptr = _file.GetFloat();
					break;
				case 64:
					*ptr = (float)_file.GetDouble();
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			default:
				throw WavException("Unsupported compression %d used", _waveFormatInfo->GetCompression());
			}
			ptr++;
		}
		_position += _waveFormatInfo->GetBlockAlign();
	}
	return read;
}

unsigned int WavFileInStream::ReadPCM16(int16_t *buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL || _dataSize == 0)
		throw WavException("Tried reading from uninitialized wav file");

	if (_waveFormatInfo->GetCompression() != WavFmtChunk::PCM && _waveFormatInfo->GetCompression() != WavFmtChunk::IEEE_FLOAT)
		throw WavException("Unsupported audio format");

	if (_waveFormatInfo->GetCompression() == WavFmtChunk::PCM && _waveFormatInfo->GetBitsPerSample() == 16 && Endian::isHostLittleEndian())
		return Read(buffer, samples); // use native

	if (_position + samples > _dataSize / _waveFormatInfo->GetBlockAlign())
	{
		samples = _dataSize / _waveFormatInfo->GetBlockAlign() - _position;
	}

	_file.Seek(_position*_waveFormatInfo->GetBlockAlign()+ _dataOffset, File::seekBeginning);

	unsigned int read;
	int16_t *ptr = buffer;
	for (read = 0; read < samples; read++)
	{
		for (int i = 0; i < _waveFormatInfo->GetChannels(); i++)
		{
			switch (_waveFormatInfo->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 8:
					*ptr = (int16_t) (((int16_t)_file.GetU8()-128) * 256);
					break;
				case 16:
					*ptr = _file.GetI16();
					break;
				case 24:
					*ptr = (int16_t) (_file.GetI24() / 256);
					break;
				case 32:
					*ptr = (int16_t) (_file.GetI32() / (256 * 256));
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 32:
					*ptr = (int16_t) (_file.GetFloat()*INT16_MAX);
					break;
				case 64:
					*ptr = (int16_t) (_file.GetDouble() * INT16_MAX);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			default:
				throw WavException("Unsupported compression %d used", _waveFormatInfo->GetCompression());
			}
			ptr++;
		}
		_position += _waveFormatInfo->GetBlockAlign();
	}
	return read;
}

unsigned int WavFileInStream::GetBitsPerSample() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetBitsPerSample();
}

unsigned int WavFileInStream::GetSampleRate() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetSampleRate();
}

unsigned int WavFileInStream::GetChannels() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetChannels();
}

unsigned int WavFileInStream::Count() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _dataSize/_waveFormatInfo->GetBlockAlign();
}

void WavFileInStream::Release()
{
	if (_waveFormatInfo != NULL)
		delete _waveFormatInfo;
}
	
void WavFileInStream::OpenWavFile(const char *path)
{
	try {
		_file = RawFile(path, File::openRawReadOnly, true);
		// Get first chunk
		WavChunk::ReadFromFile(_file);
		char riffType[4];
		riffType[0] = _file.GetI8();
		riffType[1] = _file.GetI8();
		riffType[2] = _file.GetI8();
		riffType[3] = _file.GetI8();
		if (!IsID("RIFF") || riffType[0] != 'W' || riffType[1] != 'A' || riffType[2] != 'V' || riffType[3] != 'E')
		{
			throw WavException("File is not a wave file");
		}
		ReadChunks();
	}
	catch (EndOfFileException)
	{
		throw WavException("EOF reached before filetype could be determined");
	}
}

void WavFileInStream::ReadChunks()
{
	// Read chunks
	while (!_file.IsEOF())
	{
		try {
			WavChunk chunkHeader(_file);
			chunkHeader.GotoPosition(_file);
			if (chunkHeader.IsID("fmt "))
			{
				if (_waveFormatInfo != NULL)
				{
					throw WavException("Multiple format chunks found");
				}
				_waveFormatInfo = new WavFmtChunk(_file);
			}
			else if (chunkHeader.IsID("data"))
			{
				if (_dataSize != 0)
					throw WavException("Multiple data segments not supported");
				_dataOffset = chunkHeader.GetLocation()+8;
				_dataSize = chunkHeader.ContentSize();
				_file.Seek(_dataSize+8+_dataOffset, File::seekBeginning);
			}
			else {
				chunkHeader.Skip(_file);
			}
		}
		catch (EndOfFileException)
		{
			break;
		}
	}
}

WavFileOutStream::WavFileOutStream(const char *path, OutFormat compression, unsigned int sampleRate, unsigned int channels) :
	_waveFormatInfo(NULL), _file(true), _dataOffset(0), _dataSize(0), _factOffset(0)
{
	_chunks = new std::list<WavChunk *>;
	try
	{
		WavFmtChunk::CompressionCode code;
		int bps;
		switch (compression)
		{
		case PCM8:
			code = WavFmtChunk::PCM;
			bps = 8;
			break;
		case PCM16:
			code = WavFmtChunk::PCM;
			bps = 16;
			break;
		case PCM32:
			code = WavFmtChunk::PCM;
			bps = 32;
			break;
		case IEEE_FLOAT:
			code = WavFmtChunk::IEEE_FLOAT;
			bps = 32;
			break;
		case IEEE_DOUBLE:
			code = WavFmtChunk::IEEE_FLOAT;
			bps = 64;
			break;
		default:
			throw WavException("Unknown output format %d", compression);
		}
		SetFormat(code, bps, sampleRate, channels);
		CreateWavFile(path);
	}
	catch (...)
	{
		Release();
		throw;
	}
}

WavFileOutStream::~WavFileOutStream()
{
	Release();
	delete _chunks;
}

unsigned int WavFileOutStream::Write(void* buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL)
		return 0;

	/*
	// Goto end of datastream
	if (_file.GetPosition() != _dataOffset + _dataSize)
		_file.Seek(_dataOffset + _dataSize, File::seekBeginning);
	*/
	// Possible big endian error
	unsigned int written = _file.Write(buffer, samples * _waveFormatInfo->GetBlockAlign());
	_dataSize += written;
	return written;
}

unsigned int WavFileOutStream::WriteFloat(float *buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL)
		return 0;
		
	if (_waveFormatInfo->GetCompression() == WavFmtChunk::IEEE_FLOAT && _waveFormatInfo->GetBitsPerSample() == 32 && Endian::isHostLittleEndian())
	{
		return Write(buffer, samples);
	}
	/*
	// Goto end of datastream
	if (_file.GetPosition() != _dataOffset + _dataSize)
		_file.Seek(_dataOffset + _dataSize, File::seekBeginning);
	*/
	unsigned int written;
	float *ptr = buffer;
	for (written = 0; written < samples; written++)
	{
		for (int i = 0; i < _waveFormatInfo->GetChannels(); i++)
		{
			float val = *ptr;
			if (val > 1.0f) val = 1.0f;
			else if (val < -1.0f) val = -1.0f;
			switch (_waveFormatInfo->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 8:
					_file.PutU8((uint8_t)(val * INT8_MAX + 128));
					break;
				case 16:
					_file.PutI16((int16_t)(val * INT16_MAX));
					break;
				case 24:
					_file.PutI24((int32_t)(val * 8388607));
					break;
				case 32:
					_file.PutI32((int32_t)(val * INT32_MAX));
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 32:
					_file.PutFloat(val);
					break;
				case 64:
					_file.PutDouble(val);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			default:
				throw WavException("Unsupported compression %d used", _waveFormatInfo->GetCompression());
			}
			ptr++;
			_dataSize += (_waveFormatInfo->GetBitsPerSample() + 7) / 8;
		}
	}
	return written;
}

unsigned int WavFileOutStream::WritePCM16(int16_t *buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL)
		throw WavException("Tried writing to uninitialized wav file");
		
	if (_waveFormatInfo->GetCompression() == WavFmtChunk::PCM && _waveFormatInfo->GetBitsPerSample() == 16 && Endian::isHostLittleEndian())
	{
		return Write(buffer, samples);
	}
	/*
	// Goto end of datastream
	if (_file.GetPosition() != _dataOffset + _dataSize)
		_file.Seek(_dataOffset + _dataSize, File::seekBeginning);
		*/
	unsigned int written;
	int16_t *ptr = buffer;
	for (written = 0; written < samples; written++)
	{
		for (int i = 0; i < _waveFormatInfo->GetChannels(); i++)
		{
			switch (_waveFormatInfo->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 8:
					_file.PutU8((*ptr >> 8) + 128);
					break;
				case 16:
					_file.PutI16(*ptr);
					break;
				case 24:
					_file.PutI24(*ptr << 8);
					break;
				case 32:
					_file.PutI32(*ptr << 16);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 32:
					_file.PutFloat(*ptr/(float)INT16_MAX);
					break;
				case 64:
					_file.PutDouble(*ptr / (double) INT16_MAX);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			default:
				throw WavException("Unsupported compression %d used", _waveFormatInfo->GetCompression());
			}
			ptr++;
			_dataSize += (_waveFormatInfo->GetBitsPerSample() + 7)/8;
		}
	}

	return written;
}

unsigned int WavFileOutStream::WritePCM8(uint8_t *buffer, unsigned int samples)
{
	if (_waveFormatInfo == NULL)
		throw WavException("Tried writing to uninitialized wav file");

	if (_waveFormatInfo->GetCompression() == WavFmtChunk::PCM && _waveFormatInfo->GetBitsPerSample() == 8 && Endian::isHostLittleEndian())
	{
		return Write(buffer, samples);
	}
	/*
	// Goto end of datastream
	if (_file.GetPosition() != _dataOffset + _dataSize)
	_file.Seek(_dataOffset + _dataSize, File::seekBeginning);
	*/
	unsigned int written;
	uint8_t *ptr = buffer;
	for (written = 0; written < samples; written++)
	{
		for (int i = 0; i < _waveFormatInfo->GetChannels(); i++)
		{
			switch (_waveFormatInfo->GetCompression())
			{
			case WavFmtChunk::PCM:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 8:
					_file.PutU8(*ptr);
					break;
				case 16:
					_file.PutI16(*ptr << 8);
					break;
				case 24:
					_file.PutI24(*ptr << 16);
					break;
				case 32:
					_file.PutI32(*ptr << 24);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			case WavFmtChunk::IEEE_FLOAT:
				switch (_waveFormatInfo->GetBitsPerSample())
				{
				case 32:
					_file.PutFloat(*ptr / (float) INT8_MAX);
					break;
				case 64:
					_file.PutDouble(*ptr / (double) INT8_MAX);
					break;
				default:
					throw WavException("Unsupported bits per sample: %d", _waveFormatInfo->GetBitsPerSample());
				}
				break;
			default:
				throw WavException("Unsupported compression %d used", _waveFormatInfo->GetCompression());
			}
			ptr++;
			_dataSize += (_waveFormatInfo->GetBitsPerSample() + 7) / 8;
		}
	}

	return written;
}

unsigned int WavFileOutStream::GetBitsPerSample() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetBitsPerSample();
}

unsigned int WavFileOutStream::GetSampleRate() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetSampleRate();
}

unsigned int WavFileOutStream::GetChannels() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _waveFormatInfo->GetChannels();
}

unsigned int WavFileOutStream::Count() const
{
	if (_waveFormatInfo == NULL)
		return 0;
	return _dataSize / _waveFormatInfo->GetBlockAlign();
}

void WavFileOutStream::Release()
{
	if (_file.IsOpen())
	{
		if ((_dataSize & 1) != 0)
		{
			// Need to word align
			_file.Seek(_dataOffset+_dataSize, File::seekBeginning);
			_file.PutU8(0);
		}
		// File is open so finish writing
		// Write all the chunks
		std::list<WavChunk *>::iterator it;
		for (it = _chunks->begin(); it != _chunks->end(); ++it)
		{
			(*it)->WriteToFile(_file);
			delete *it;
		}
		// Write datasize
		_file.Seek(_dataOffset-4, File::seekBeginning);
		_file.PutU32(_dataSize);

		// Write number of samples per channel if there is a fact chunk
		if (_factOffset != 0 && _waveFormatInfo != NULL)
		{
			_file.Seek(_factOffset, File::seekBeginning);
			_file.PutU32(_dataSize/_waveFormatInfo->GetBlockAlign());
		}

		// Write container size
		_file.Seek(4, File::seekBeginning);
		_file.PutU32(_file.GetSize()-8);
	}
	if (_waveFormatInfo != NULL)
		delete _waveFormatInfo;
}

void WavFileOutStream::CreateWavFile(const char *path)
{
	assert(_waveFormatInfo != NULL);
	_file = RawFile(path, File::openRawRWCreate);
	_file.PutC('R'); _file.PutC('I'); _file.PutC('F'); _file.PutC('F');
	_file.PutU32(0); // Empty size for now
	_file.PutC('W'); _file.PutC('A'); _file.PutC('V'); _file.PutC('E');

	// Write format table
	_waveFormatInfo->WriteToFile(_file);

	// Todo other sections
	if (_waveFormatInfo->GetCompression() != WavFmtChunk::PCM)
	{
		// Add fact chunk
		_file.PutC('f'); _file.PutC('a'); _file.PutC('c'); _file.PutC('t');
		_file.PutU32(4);
		_factOffset = _file.GetPosition();
		_file.PutU32(0);
	}


	// Initialize data section
	_file.PutC('d'); _file.PutC('a'); _file.PutC('t'); _file.PutC('a');
	_file.PutU32(0);
	_dataOffset = _file.GetPosition();

}

void WavFileOutStream::SetFormat(WavFmtChunk::CompressionCode compression, unsigned int bitsPerSample, unsigned int sampleRate, unsigned int channels)
{
	if (_waveFormatInfo != NULL)
	{
		delete _waveFormatInfo;
		_waveFormatInfo = NULL;
	}
	_waveFormatInfo = new WavFmtChunk();
	_waveFormatInfo->SetBitsPerSample(bitsPerSample);
	_waveFormatInfo->SetCompression(compression);
	_waveFormatInfo->SetChannels(channels);
	_waveFormatInfo->SetSampleRate(sampleRate);
	_waveFormatInfo->SetBlockAlign(channels * ((bitsPerSample+7)/8));
	_waveFormatInfo->SetBytesPerSecond(_waveFormatInfo->GetBlockAlign() * sampleRate);
}
