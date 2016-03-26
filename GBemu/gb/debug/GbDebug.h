#ifndef _GBDEBUG_H
#define _GBDEBUG_H

#include <wx/wx.h>
#include "GbCpuDbg.h"
#include "GbMemDbg.h"
#include "GbGpuDbg.h"

class GbDebug : public wxMenu
{
	friend class Gameboy;
private:
	wxFrame		*parent;
	GbCpuDbg	*cpuDbg;
	GbMemDbg	*memDbg;
	GbGpuDbg	*gpuDbg;

public:
	GbDebug(wxFrame *parent);

	void onMemory(wxCommandEvent &evt);
	void onCPU(wxCommandEvent &evt);
	void onGPU(wxCommandEvent &evt);
	void onSound(wxCommandEvent &evt);
	

	DECLARE_EVENT_TABLE()
};

#endif