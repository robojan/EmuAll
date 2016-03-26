#ifndef SAVEFILE_H_
#define SAVEFILE_H_

#include <emu.h>
#include <wx/string.h>
#include <wx/ffile.h>

namespace SaveFile {
	static const uint32_t saveID = 0x454D5541;
	static const uint32_t stateID = 0x454D5553;

	void ReadSaveFile(wxString fileName, SaveData_t &data);
	void WriteSaveFile(wxString fileName, const SaveData_t &data);
	void ReadStateFile(wxString fileName, SaveData_t &data);
	void WriteStateFile(wxString fileName, const SaveData_t &data);

	void ReadSection(wxFFile &file, size_t *len, void **data);
}

#endif
