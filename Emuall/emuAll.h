#ifndef _EMUALL_H
#define _EMUALL_H 0

#include <wx/wx.h>
#include "gui/MainFrame.h"

class EmuAll: public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
private:
	MainFrame *_mainFrame;
};

#endif