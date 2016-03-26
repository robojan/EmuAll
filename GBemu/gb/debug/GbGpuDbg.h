#ifndef _GBGPUDBG_H
#define _GBGPUDBG_H

#include <wx/wx.h>
#include <wx/bookctrl.h>
#include <wx/grid.h>
#include "../mem/GbMem.h"
#include "../gpu/GbGpu.h"
#include "../gbInfo.h"
#include "../../../gui/GlBufferPane.h"

class GbGpuDbg : public wxFrame
{
	DECLARE_CLASS(GbGpuDbg)
	DECLARE_EVENT_TABLE()
public:
	GbGpuDbg(wxFrame *parent, GbInfo_t *info);
	~GbGpuDbg();

	void updateContent();
private:
	GbInfo_t			*m_info;
	wxBookCtrl			*m_book;
	wxPanel				*m_panel;
	wxPanel				*m_book_background;
	wxPanel				*m_book_tiles;
	wxPanel				*m_book_palettes;
	wxPanel				*m_book_oam;
	wxStaticBoxSizer	*m_background_sizer;
	wxStaticBoxSizer	*m_background_grid;
	wxStaticBoxSizer	*m_background_tile;
	wxStaticBoxSizer	*m_gpuinfo_sizer;
	wxStaticBoxSizer	*m_tiles_sizer;
	wxStaticBoxSizer	*m_tiles_info;
	wxStaticBoxSizer	*m_palettes_BgBox;
	wxStaticBoxSizer	*m_palettes_ObjBox;
	wxStaticBoxSizer	*m_palettes_monoBox;
	wxStaticBoxSizer	*m_oam_box;
	wxStaticBoxSizer	*m_oam_infoBox;
	wxRadioBox			*m_background_map;
	wxRadioBox			*m_background_data;
	wxCheckBox			*m_background_hflip;
	wxCheckBox			*m_background_vflip;
	wxCheckBox			*m_background_prio;
	wxCheckBox			*m_gpu_enabled;
	wxCheckBox			*m_background_grid_chkbx;
	wxCheckBox			*m_oam_prio;
	wxCheckBox			*m_oam_xflip;
	wxCheckBox			*m_oam_yflip;
	wxCheckBox			*m_oam_vram;
	wxStaticText		*m_LCDC;
	wxStaticText		*m_STAT;
	wxStaticText		*m_SCX;
	wxStaticText		*m_SCY;
	wxStaticText		*m_LY;
	wxStaticText		*m_LYC;
	wxStaticText		*m_WY;
	wxStaticText		*m_WX;
	wxStaticText		*m_VBK;
	wxStaticText		*m_background_tilenr;
	wxStaticText		*m_background_palette;
	wxStaticText		*m_tiles_address;
	wxStaticText		*m_tiles_number;
	wxStaticText		*m_oam_x;
	wxStaticText		*m_oam_y;
	wxStaticText		*m_oam_nr;
	wxStaticText		*m_oam_palette;
	GlBufferPane		*m_background_image;
	GlBufferPane		*m_tile_image;
	GlBufferPane		*m_oam_image;
	wxGrid				*m_palette_bg;
	wxGrid				*m_palette_ob;
	wxGrid				*m_palette_mono;
	wxPoint				m_mouse;
	wxBoxSizer			*m_sizerH1;
	wxBoxSizer			*m_sizerH2;
	wxBoxSizer			*m_sizerH3;
	wxBoxSizer			*m_sizerH4;
	wxBoxSizer			*m_sizerV1;
	wxBoxSizer			*m_sizerV2;
	wxBoxSizer			*m_sizerV3;
	wxBoxSizer			*m_sizerV4;

	void drawBackground(void);
	void drawTiles(void);

	void onClose(wxCloseEvent &evt);
	void onMouse(wxMouseEvent &evt);
};

#endif