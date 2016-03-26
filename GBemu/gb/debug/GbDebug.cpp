#include "GbDebug.h"
#include "../../../gui/IdList.h"
#include "GbCpuDbg.h"
#include "../Gameboy.h"
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

BEGIN_EVENT_TABLE(GbDebug, wxMenu)
	EVT_MENU(ID_Main_Debug_GB_mem, GbDebug::onMemory)
	EVT_MENU(ID_Main_Debug_GB_cpu, GbDebug::onCPU)
	EVT_MENU(ID_Main_Debug_GB_gpu, GbDebug::onGPU)
	EVT_MENU(ID_Main_Debug_GB_sound, GbDebug::onSound)
END_EVENT_TABLE()

GbDebug::GbDebug(wxFrame *parent) : wxMenu()
{
	Append(ID_Main_Debug_GB_mem, _("&Memory"));
	Append(ID_Main_Debug_GB_cpu, _("&CPU"));
	Append(ID_Main_Debug_GB_gpu, _("&GPU"));
	Append(ID_Main_Debug_GB_sound, _("&Sound"));
	this->parent = parent;
	this->cpuDbg = new GbCpuDbg(parent, &Gameboy::gbInfo);
	this->memDbg = new GbMemDbg(parent, &Gameboy::gbInfo);
	this->gpuDbg = new GbGpuDbg(parent, &Gameboy::gbInfo);
}

void GbDebug::onMemory(wxCommandEvent &evt)
{
	memDbg->updateContent();
	memDbg->Refresh();
	memDbg->Show(true);
	memDbg->SetFocus();
}

void GbDebug::onCPU(wxCommandEvent &evt)
{
	cpuDbg->updateContent();
	cpuDbg->Show(true);
	cpuDbg->SetFocus();
}

void GbDebug::onGPU(wxCommandEvent &evt)
{
	gpuDbg->updateContent();
	gpuDbg->Show(true);
	gpuDbg->SetFocus();
}

void GbDebug::onSound(wxCommandEvent &evt)
{

}