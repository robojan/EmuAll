#ifndef _IDLIST_H
#define _IDLIST_H

#include <wx/wx.h>

enum
{
	ID_MainFrame = wxID_HIGHEST + 1,
	ID_Timer,
	ID_Main_File_open,
	ID_Main_File_quit,
	ID_Main_File_reset,
	ID_Main_File_run,
	ID_Main_File_SaveState,
	ID_Main_File_LoadState = ID_Main_File_SaveState + 10,
	ID_Main_File_RecentFile = ID_Main_File_LoadState + 10,
	ID_Main_Debug_window = ID_Main_File_RecentFile + 5,
	ID_Main_Debug_level_fat,
	ID_Main_Debug_level_err,
	ID_Main_Debug_level_war,
	ID_Main_Debug_level_mes,
	ID_Main_Debug_level_deb,
	ID_Main_Debug_cpu,
	ID_Main_Debug_mem,
	ID_Main_Debug_gpu,
	ID_Debug_Cpu_stepOver,
	ID_Debug_Cpu_stepOut,
	ID_Debug_Cpu_step,
	ID_Debug_Cpu_run,
	ID_Debug_Mem_picker,
	ID_Debug_Mem_goto,
	ID_Debug_Gpu_timer,
	ID_Main_Options_KeepAspect,
	ID_Main_Options_Filter,
	ID_Main_Options_input = ID_Main_Options_Filter + 6,
	ID_Main_display
};

#endif