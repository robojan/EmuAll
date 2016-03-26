
#include "SaveFile.h"

#include <wx/filename.h>
#include "../util/log.h"
#include "../util/Endian.h"

void SaveFile::ReadSaveFile(wxString fileName, SaveData_t &data)
{
	data.ramData = NULL;
	data.ramDataLen = 0;
	data.miscData = NULL;
	data.miscDataLen = 0;
	if (!wxFileName::FileExists(fileName))
		return;
	
	wxFFile file(fileName, "rb");
	if (!file.IsOpened()) {
		Log(Warn, "Could not open save data file '%s': %s", fileName.c_str(), strerror(errno));
		return;
	}
	uint32_t id;
	if (file.Read(&id, sizeof(uint32_t)) != sizeof(uint32_t)) {
		Log(Warn, "Error reading save file");
		return;
	}
	id = getEndianFuncs(0)->convu32(id);
	if (id == SaveFile::saveID) {
		ReadSection(file, &data.ramDataLen, &data.ramData);
		ReadSection(file, &data.miscDataLen, &data.miscData);
	}
	else {
		Log(Warn, "Save file not in Emuall format, interpreting it as raw save data.");
		file.Seek(0);
		data.ramDataLen = file.Length();
		data.ramData = new uint8_t[data.ramDataLen];
		int read = file.Read(data.ramData, data.ramDataLen);
		if (read != data.ramDataLen) {
			if (file.Error() || file.Eof()) {
				Log(Warn, "Error occurred while reading RAM data");
				delete[] data.ramData;
				data.ramData = NULL;
				data.ramDataLen = 0;
				return;
			}
		}
	}
}

void SaveFile::WriteSaveFile(wxString fileName, const SaveData_t &data)
{
	if (data.miscDataLen == 0 && data.ramDataLen == 0) {
		// Don't create save file when there is nothing to save
		return;
	}

	wxFFile file(fileName, "wb");
	if (!file.IsOpened()) {
		Log(Error, "Could not open save data file '%s': %s", fileName.c_str(), strerror(errno));
		return;
	}
	if (file.Write(&SaveFile::saveID, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (file.Write(&data.ramDataLen, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (data.ramDataLen > 0) {
		if (file.Write(data.ramData, data.ramDataLen) != data.ramDataLen) goto writeError;
	}
	if (file.Write(&data.miscDataLen, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (data.miscDataLen > 0) {
		if (file.Write(data.miscData, data.miscDataLen) != data.miscDataLen) goto writeError;
	}
	return;

writeError:
	Log(Error, "Error writing file '%s': %s", fileName.c_str(), strerror(errno));
	file.Close();
	wxRemoveFile(fileName);
}

void SaveFile::ReadStateFile(wxString fileName, SaveData_t &data)
{
	data.romData = NULL;
	data.romDataLen = 0;
	data.ramData = NULL;
	data.ramDataLen = 0;
	data.miscData = NULL;
	data.miscDataLen = 0;
	if (!wxFileName::FileExists(fileName))
		return;

	wxFFile file(fileName, "rb");
	if (!file.IsOpened()) {
		Log(Error, "Could not open state data file '%s': %s", fileName.c_str(), strerror(errno));
		return;
	}
	uint32_t id;
	if (file.Read(&id, sizeof(uint32_t)) != sizeof(uint32_t)) {
		Log(Error, "Error reading state file");
		return;
	}
	id = getEndianFuncs(0)->convu32(id);
	if (id != SaveFile::stateID) {
		Log(Error, "Corrupted state data");
		return;
	}
	//ReadSection(file, &data.romDataLen, &data.romData);
	ReadSection(file, &data.ramDataLen, &data.ramData);
	ReadSection(file, &data.miscDataLen, &data.miscData);
}

void SaveFile::WriteStateFile(wxString fileName, const SaveData_t &data)
{
	if (data.miscDataLen == 0 && data.ramDataLen == 0) {
		// Don't create save file when there is nothing to save
		return;
	}

	wxFFile file(fileName, "wb");
	if (!file.IsOpened()) {
		Log(Error, "Could not open save data file '%s': %s", fileName.c_str(), strerror(errno));
		return;
	}
	if (file.Write(&SaveFile::stateID, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (file.Write(&data.ramDataLen, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (data.ramDataLen > 0) {
		if (file.Write(data.ramData, data.ramDataLen) != data.ramDataLen) goto writeError;
	}
	if (file.Write(&data.miscDataLen, sizeof(uint32_t)) != sizeof(uint32_t)) goto writeError;
	if (data.miscDataLen > 0) {
		if (file.Write(data.miscData, data.miscDataLen) != data.miscDataLen) goto writeError;
	}
	return;

writeError:
	Log(Error, "Error writing file '%s': %s", fileName.c_str(), strerror(errno));
	file.Close();
	wxRemoveFile(fileName);
}

void SaveFile::ReadSection(wxFFile &file, size_t *len, void **data)
{
	uint32_t sectionLen;
	if (file.Read(&sectionLen, sizeof(uint32_t)) != sizeof(uint32_t)) {
		return;
	}
	sectionLen = getEndianFuncs(0)->convu32(sectionLen);
	if (sectionLen > 0) {
		*data = new uint8_t[sectionLen];
		if ((*len = file.Read(*data, sectionLen)) != sectionLen) {
			Log(Warn, "Could not read all the data");
			if (file.Eof() || file.Error()) {
				if (*len == 0) {
					delete[] *data;
					*data = NULL;
					*len = 0;
				}
			}
		}
	}
}
