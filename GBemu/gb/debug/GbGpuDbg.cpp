
#include "GbGpuDbg.h"
#include "../../../gui/IdList.h"
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

IMPLEMENT_CLASS(GbGpuDbg, wxFrame)
BEGIN_EVENT_TABLE(GbGpuDbg, wxFrame)
	EVT_CLOSE(GbGpuDbg::onClose)
END_EVENT_TABLE()

GbGpuDbg::GbGpuDbg(wxFrame *parent, GbInfo_t *info) :
	wxFrame(parent, ID_Debug_Gpu_window, _("Gpu debugger"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
{
	m_info = info;

	m_panel = new wxPanel(this);
	m_sizerH1 = new wxBoxSizer(wxHORIZONTAL);

	// Tabs
	m_book = new wxBookCtrl(m_panel, ID_Debug_Gpu_book);
	
	// Background tab
	m_book_background = new wxPanel(m_book);
	m_sizerH2 = new wxBoxSizer(wxHORIZONTAL);
	m_sizerV1 = new wxBoxSizer(wxVERTICAL);

	// Background 
	m_background_sizer = new wxStaticBoxSizer(wxHORIZONTAL,m_book_background, _("Background"));
	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
	m_background_image = new GlBufferPane(m_book_background,256,256, ID_Debug_Gpu_Background_canvas, wxDefaultPosition, wxSize(256,256), wxFULL_REPAINT_ON_RESIZE, args);
	m_background_image->Connect(wxEVT_MOTION, wxMouseEventHandler(GbGpuDbg::onMouse),(wxObject *) NULL, this);
	int limit = m_background_image->getBufferHeight()*m_background_image->getBufferWidth();
	int color = m_book_background->GetBackgroundColour().GetRGBA();
	for(int i = 0; i < limit; i++)
	{
		m_background_image->m_screen[i] = color;
	}
	m_background_sizer->Add(m_background_image);
	m_background_grid = new wxStaticBoxSizer(wxVERTICAL, m_book_background, _("Grid"));
	m_background_grid_chkbx = new wxCheckBox(m_book_background, ID_Debug_Gpu_Background_grid, _("Enabled"));
	m_background_grid->Add(m_background_grid_chkbx);
	wxString radioMap[] = {_("9800-9BFF"), _("9C00-9FFF"), _("Auto")};
	m_background_map = new wxRadioBox(m_book_background, wxID_ANY,_("Background Map"), wxDefaultPosition, wxDefaultSize, 3, radioMap, 0,wxVERTICAL);
	m_background_map->SetSelection(2);
	wxString radioData[] = {_("8800-97FF"), _("8000-87FF"), _("Auto")};
	m_background_data = new wxRadioBox(m_book_background, wxID_ANY,_("Background Data"), wxDefaultPosition, wxDefaultSize, 3, radioData, 0,wxVERTICAL);
	m_background_data->SetSelection(2);
	m_background_tile = new wxStaticBoxSizer(wxVERTICAL, m_book_background,_("Info"));
	m_background_tilenr = new wxStaticText(m_book_background, wxID_ANY, _("Number: 0"));
	m_background_palette = new wxStaticText(m_book_background, wxID_ANY, _("Palette: 0"));
	m_background_hflip = new wxCheckBox(m_book_background, wxID_ANY, _("Horizontal Flip"));
	m_background_vflip = new wxCheckBox(m_book_background, wxID_ANY, _("Vertical Flip"));
	m_background_prio = new wxCheckBox(m_book_background, wxID_ANY, _("Priority"));
	m_background_hflip->Enable(false);
	m_background_vflip->Enable(false);
	m_background_prio->Enable(false);
	m_background_tile->Add(m_background_tilenr);
	m_background_tile->Add(m_background_palette);
	m_background_tile->Add(m_background_hflip);
	m_background_tile->Add(m_background_vflip);
	m_background_tile->Add(m_background_prio);
	m_sizerV1->Add(m_background_grid);
	m_sizerV1->Add(m_background_map);
	m_sizerV1->Add(m_background_data);
	m_sizerV1->Add(m_background_tile);
	m_sizerH2->Add(m_background_sizer);
	m_sizerH2->AddSpacer(5);
	m_sizerH2->Add(m_sizerV1);
	m_sizerH2->AddSpacer(5);

	m_book_background->SetSizerAndFit(m_sizerH2);
	m_book->AddPage(m_book_background, _("Background"), true);

	// Tiles tab
	m_book_tiles = new wxPanel(m_book);
	m_sizerV3 = new wxBoxSizer(wxVERTICAL);
	m_tiles_sizer = new wxStaticBoxSizer(wxHORIZONTAL, m_book_tiles, _("Tiles"));
	m_tile_image = new GlBufferPane(m_book_tiles, 289, 217, ID_Debug_Gpu_Tiles_canvas, wxDefaultPosition, wxSize(346,260), wxFULL_REPAINT_ON_RESIZE, args);
	m_tile_image->Connect(wxEVT_MOTION, wxMouseEventHandler(GbGpuDbg::onMouse),(wxObject *) NULL, this);
	m_tiles_info = new wxStaticBoxSizer(wxHORIZONTAL, m_book_tiles, _("Tile info"));
	m_tiles_address = new wxStaticText(m_book_tiles, wxID_ANY,_("Address: 0:0000   "));
	m_tiles_number = new wxStaticText(m_book_tiles, wxID_ANY, _("Number: 000"));
	m_tiles_info->Add(m_tiles_address);
	m_tiles_info->Add(m_tiles_number);

	limit = m_tile_image->getBufferHeight()*m_tile_image->getBufferWidth();
	color = m_book_tiles->GetBackgroundColour().GetRGBA();
	for(int i = 0; i < limit; i++)
	{
		m_tile_image->m_screen[i] = 0xFFFFFF;
	}
	for(int x = 0; x < 33; x++)
	{
		for(int y = 0; y < m_tile_image->getBufferHeight(); y++)
		{
			m_tile_image->m_screen[x*9+y*m_tile_image->getBufferWidth()] = 0xF0F0F0;
		}
	}
	for(int y = 0; y < 25; y++)
	{
		for(int x = 0; x < m_tile_image->getBufferWidth(); x++)
		{
			m_tile_image->m_screen[x+y*9*m_tile_image->getBufferWidth()] = 0xF0F0F0;
		}
	}
	m_tiles_sizer->Add(m_tile_image);
	m_sizerV3->Add(m_tiles_sizer);
	m_sizerV3->Add(m_tiles_info);
	m_book_tiles->SetSizerAndFit(m_sizerV3);
	m_book->AddPage(m_book_tiles, _("Tiles"));
	
	// OAM tab
	m_book_oam = new wxPanel(m_book);
	m_oam_box = new wxStaticBoxSizer(wxHORIZONTAL, m_book_oam, _("OAM objects:"));
	m_oam_image = new GlBufferPane(m_book_oam, 8*9+1, 5*17+1, ID_Debug_Gpu_OAM_canvas,wxDefaultPosition, wxSize(8*8*4+9, 5*16*4+6), 0, args);
	m_oam_image->Connect(wxEVT_MOTION, wxMouseEventHandler(GbGpuDbg::onMouse),(wxObject *) NULL, this);
	m_oam_box->Add(m_oam_image);
	m_oam_infoBox = new wxStaticBoxSizer(wxVERTICAL, m_book_oam, _("OAM info"));
	m_oam_y = new wxStaticText(m_book_oam, wxID_ANY, _("Y: 000"));
	m_oam_x = new wxStaticText(m_book_oam, wxID_ANY, _("X: 000"));
	m_oam_nr = new wxStaticText(m_book_oam, wxID_ANY, _("Tile: 0x00"));
	m_oam_palette = new wxStaticText(m_book_oam, wxID_ANY, _("Palette: OBJ0"));
	m_oam_prio = new wxCheckBox(m_book_oam, wxID_ANY, _("Priority"));
	m_oam_yflip = new wxCheckBox(m_book_oam, wxID_ANY, _("Y Flip"));
	m_oam_xflip = new wxCheckBox(m_book_oam, wxID_ANY, _("X Flip"));
	m_oam_vram = new wxCheckBox(m_book_oam, wxID_ANY, _("VRAM"));
	m_oam_infoBox->Add(m_oam_y);
	m_oam_infoBox->Add(m_oam_x);
	m_oam_infoBox->Add(m_oam_nr);
	m_oam_infoBox->Add(m_oam_palette);
	m_oam_infoBox->Add(m_oam_prio);
	m_oam_infoBox->Add(m_oam_xflip);
	m_oam_infoBox->Add(m_oam_yflip);
	m_oam_infoBox->Add(m_oam_vram);
	m_sizerH4 = new wxBoxSizer(wxHORIZONTAL);
	m_sizerH4->Add(m_oam_box);
	m_sizerH4->Add(m_oam_infoBox);
	m_book_oam->SetSizerAndFit(m_sizerH4);
	int width = m_oam_image->getBufferWidth();
	int bgcol = m_book_oam->GetBackgroundColour().GetRGB();
	int fgcol = wxLIGHT_GREY->GetRGB();
	for(int y = 0; y < m_oam_image->getBufferHeight(); y++)
	{
		for(int x = 0; x < width; x++)
		{
			if(x % 9 == 0 || y % 17 == 0)
			{
				m_oam_image->m_screen[x+y*width] = fgcol;
			} else {
				m_oam_image->m_screen[x+y*width] = bgcol;
			}
		}
	}
	m_book->AddPage(m_book_oam, _("OAM"));

	// palettes Tab
	m_book_palettes = new wxPanel(m_book);
	m_sizerV4 = new wxBoxSizer(wxVERTICAL);
	m_sizerH3 = new wxBoxSizer(wxHORIZONTAL);
	m_palettes_BgBox = new wxStaticBoxSizer(wxHORIZONTAL, m_book_palettes, _("Background palette:"));
	m_palettes_ObjBox = new wxStaticBoxSizer(wxHORIZONTAL, m_book_palettes, _("Object palette:"));
	m_palette_bg = new wxGrid(m_book_palettes, wxID_ANY);
	m_palette_ob = new wxGrid(m_book_palettes, wxID_ANY);
	m_palette_bg->CreateGrid(8, 4, wxGrid::wxGridSelectRows);
	m_palette_bg->SetDefaultColSize(20,true);
	m_palette_bg->SetDefaultRowSize(20,true);
	m_palette_ob->CreateGrid(8, 4, wxGrid::wxGridSelectRows);
	m_palette_ob->SetDefaultColSize(20,true);
	m_palette_ob->SetDefaultRowSize(20,true);
	m_palette_ob->DisableDragGridSize();
	m_palette_bg->DisableDragGridSize();
	for(int i = 0; i<8; i++)
	{
		m_palette_bg->SetRowLabelValue(i, wxString::Format(_("BG Pal %d"),i));
		m_palette_ob->SetRowLabelValue(i, wxString::Format(_("Obj Pal %d"),i));
	}
	m_palettes_BgBox->Add(m_palette_bg);
	m_palettes_ObjBox->Add(m_palette_ob);
	m_sizerH3->Add(m_palettes_BgBox, 0, wxALIGN_LEFT);
	m_sizerH3->Add(NULL, 0, wxEXPAND);
	m_sizerH3->Add(m_palettes_ObjBox, 0, wxALIGN_RIGHT);
	m_sizerV4->Add(m_sizerH3);

	m_palettes_monoBox = new wxStaticBoxSizer(wxVERTICAL, m_book_palettes, _("Monochrome Palettes"));
	m_palette_mono = new wxGrid(m_book_palettes, wxID_ANY);
	m_palette_mono->CreateGrid(3, 4, wxGrid::wxGridSelectRows);
	m_palette_mono->SetDefaultColSize(20,true);
	m_palette_mono->SetDefaultRowSize(20,true);
	m_palette_mono->SetRowLabelValue(0, _("BGP"));
	m_palette_mono->SetRowLabelValue(1, _("OBP 0"));
	m_palette_mono->SetRowLabelValue(2, _("OBP 1"));
	m_palette_mono->DisableDragGridSize();
	for(int i = 0; i<4; i++)
	{
		m_palette_bg->SetColLabelValue(i, wxString::Format(_("%d"), i));
		m_palette_ob->SetColLabelValue(i, wxString::Format(_("%d"), i));
		m_palette_mono->SetColLabelValue(i, wxString::Format(_("%d"), i));
	}
	m_palettes_monoBox->Add(m_palette_mono);

	m_sizerV3->Add(m_palettes_monoBox);

	m_book_palettes->SetSizer(m_sizerV4);
	m_book->AddPage(m_book_palettes, _("Palettes"));

	m_book->Fit();
	m_sizerV2 = new wxBoxSizer(wxVERTICAL);
	m_gpuinfo_sizer = new wxStaticBoxSizer(wxVERTICAL, m_panel, _("GPU info"));
	m_gpu_enabled = new wxCheckBox(m_panel, wxID_ANY, _("Enabled"));
	m_gpu_enabled->Enable(false);
	m_LCDC = new wxStaticText(m_panel, wxID_ANY, _("LCDC: 0x00"));
	m_STAT = new wxStaticText(m_panel, wxID_ANY, _("STAT: 0x00"));
	m_SCX = new wxStaticText(m_panel, wxID_ANY, _("SCX: 0x00"));
	m_SCY = new wxStaticText(m_panel, wxID_ANY, _("SCY: 0x00"));
	m_LY = new wxStaticText(m_panel, wxID_ANY, _("LY: 0x00"));
	m_LYC = new wxStaticText(m_panel, wxID_ANY, _("LYC: 0x00"));
	m_WY = new wxStaticText(m_panel, wxID_ANY, _("WY: 0x00"));
	m_WX = new wxStaticText(m_panel, wxID_ANY, _("WX: 0x00"));
	m_VBK = new wxStaticText(m_panel, wxID_ANY, _("VRAM: 0"));
	m_gpuinfo_sizer->Add(m_gpu_enabled);
	m_gpuinfo_sizer->Add(m_LCDC);
	m_gpuinfo_sizer->Add(m_STAT);
	m_gpuinfo_sizer->Add(m_SCX);
	m_gpuinfo_sizer->Add(m_SCY);
	m_gpuinfo_sizer->Add(m_LY);
	m_gpuinfo_sizer->Add(m_LYC);
	m_gpuinfo_sizer->Add(m_WY);
	m_gpuinfo_sizer->Add(m_WX);
	m_gpuinfo_sizer->Add(m_VBK);
	m_sizerV2->AddStretchSpacer();
	m_sizerV2->Add(m_gpuinfo_sizer);
	m_sizerH1->Add(m_book);
	m_sizerH1->Add(m_sizerV2);
	m_panel->SetSizerAndFit(m_sizerH1);
	Fit();
}

GbGpuDbg::~GbGpuDbg()
{

}

void GbGpuDbg::updateContent(void)
{
	static gbByte lcdc = 0;
	static gbByte stat = 0;
	static gbByte scx = 0;
	static gbByte scy = 0;
	static gbByte ly = 0;
	static gbByte lyc = 0;
	static gbByte wy = 0;
	static gbByte wx = 0;
	static gbByte vbk = 0;
	static gbByte tx = 0;
	static gbByte ty = 0;
	gbByte temp;
	if(!this->IsShown())
	{
		return;
	}
	if(m_info == NULL || m_info->gpu == NULL || m_info->mem == NULL)
	{
		return;
	}
	if(m_book_background->IsShown())
	{
		drawBackground();
		
		m_background_image->Refresh();
		if((tx != m_mouse.x/8 || ty != m_mouse.y/8)&&m_mouse.y < m_background_image->getHeight() && m_mouse.x < m_background_image->getWidth())
		{
			tx = m_mouse.x/8;
			ty = m_mouse.y/8;
			bool bgd = m_background_data->GetSelection() == 2 ? (m_info->mem->read(LCDC) & LCDC_BS) != 0 : m_background_data->GetSelection() == 1;
			bool bgm = m_background_map->GetSelection() == 2 ? (m_info->mem->read(LCDC) & LCDC_BTS) != 0 : m_background_map->GetSelection() == 1;
			gbBgMap tile = m_info->gpu->getTileData(bgd, bgm, tx+(ty*32));
			if(bgm)
			{
				m_background_tilenr->SetLabel(wxString::Format(_("Number: %d"), (unsigned int)m_info->mem->vram[0][BGMAP_VRAM_OFFSET1 + tx+(ty*32)]));
			} else {
				m_background_tilenr->SetLabel(wxString::Format(_("Number: %d"), (int)m_info->mem->vram[0][BGMAP_VRAM_OFFSET0 + tx+(ty*32)]));
			}
			m_background_palette->SetLabel(wxString::Format(_("Palette: %d"), tile.attributes & BGMAP_PALETTE));
			m_background_hflip->SetValue((tile.attributes & BGMAP_ATTR_HFLIP) != 0);
			m_background_vflip->SetValue((tile.attributes & BGMAP_ATTR_VFLIP) != 0);
			m_background_prio->SetValue((tile.attributes & BGMAP_ATTR_PRIO) != 0);
		}
	}
	if(m_book_tiles->IsShown())
	{
		drawTiles();
		m_tile_image->Refresh();
		if((tx != m_mouse.x/9 || ty != m_mouse.y/9)&&m_mouse.y < m_tile_image->getHeight() && m_mouse.x < m_tile_image->getWidth())
		{
			tx = m_mouse.x/9;
			int x = ((m_mouse.x*m_tile_image->getBufferWidth())/((double)m_tile_image->getWidth()))/9;
			ty = m_mouse.y/9;
			int y = ((m_mouse.y*m_tile_image->getBufferHeight())/((double)m_tile_image->getHeight()))/9;
			int vram = x < (16) ? 0x00 : 0x01;
			x %= 16;
			int address = (x+y*16)*0x10 + 0x8000;
			int number = y < 12 ? (x + y * 16) : (x + (y-12)*16 - 64);
			m_tiles_address->SetLabel(wxString::Format(_("Address: %d:%04X   "), vram, address));
			m_tiles_number->SetLabel(wxString::Format(_("Number: %03d"), number));
		}
	}
	if(m_book_oam->IsShown())
	{
		bool large = (lcdc & LCDC_OS) != 0;
		bool gbc = (m_info->mem->read(CGB) & 0x80) != 0;
		for(int i = 0; i < 40; i++)
		{
			int ox = (i % 8)*9+1;
			int oy = (i / 8)*17+1;
			gbSprite sprite;
			sprite.y = m_info->mem->mem[0xF][OAM_OFFSET + i * 4 + 0];
			sprite.x = m_info->mem->mem[0xF][OAM_OFFSET + i * 4 + 1];
			sprite.loc = m_info->mem->mem[0xF][OAM_OFFSET + i * 4 + 2];
			sprite.attr = m_info->mem->mem[0xF][OAM_OFFSET + i * 4 + 3];
			for(int y = 0; y < 16; y++)
			{
				gbByte yindex;
				if(large)
				{
					sprite.loc &= 0xFE;
					if(sprite.attr & OAM_YFLIP)
					{
						if(y < 8) sprite.loc |= 0x01;
						yindex = 7 - (y%8);
					} else {
						if(y > 7) sprite.loc |= 0x01;
						yindex = y % 8;
					}
				} else {
					yindex = (sprite.attr & OAM_YFLIP) ? 7-y : y;
					if(y > 7) 
					{
						break;
					}
				}
				
				gbByte *ptr = m_info->mem->vram[(sprite.attr & OAM_VRAM) ? 1 : 0]+SPRITE_OFFSET+sprite.loc*2*8;
				for(int x = 0; x < 8; x++)
				{
					gbByte xindex = (sprite.attr & OAM_XFLIP) ? 7-x : x;
					gbByte pixel = m_info->gpu->getTilePixel(ptr, xindex, yindex);gbColor col;
					if(gbc)
					{
						col = m_info->gpu->convertPaletteData(pixel, sprite.attr & OAM_PALETTE, m_info->gpu->m_oPaletteData);
					} else {
						col = m_info->gpu->convertPaletteData(pixel, m_info->mem->read((sprite.attr & OAM_PALETTENR) ? OBP1 : OBP0));
					}
					m_oam_image->m_screen[(oy+y)*m_oam_image->getBufferWidth()+(ox+x)] = col.color;
				}
			}

		}
		m_oam_image->Refresh();
		
		if(m_mouse.y < m_oam_image->getHeight() && m_mouse.x < m_oam_image->getWidth())
		{
			int x = ((m_mouse.x*m_oam_image->getBufferWidth())/((double)m_oam_image->getWidth()))/9;
			int y = ((m_mouse.y*m_oam_image->getBufferHeight())/((double)m_oam_image->getHeight()))/17;
			int objectnr = x + y * 8;
			gbSprite sprite;
			sprite.y = m_info->mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 0];
			sprite.x = m_info->mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 1];
			sprite.loc = m_info->mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 2];
			sprite.attr = m_info->mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 3];
			m_oam_y->SetLabelText(wxString::Format("Y: %03d", sprite.y));
			m_oam_x->SetLabelText(wxString::Format("X: %03d", sprite.x));
			m_oam_nr->SetLabelText(wxString::Format("Tile: 0x%02X", sprite.loc));
			if(gbc)
			{
				m_oam_palette->SetLabelText(wxString::Format("Palette: PAL%d", sprite.attr & 0x7));
			} else {
				m_oam_palette->SetLabelText(wxString::Format("Palette: OBJ%d", (sprite.attr & OAM_PALETTENR) ? 1 : 0));
			}
			m_oam_prio->SetValue((sprite.attr & OAM_PRIO) != 0);
			m_oam_xflip->SetValue((sprite.attr & OAM_XFLIP) != 0);
			m_oam_yflip->SetValue((sprite.attr & OAM_YFLIP) != 0);
			m_oam_vram->SetValue((sprite.attr & OAM_VRAM) != 0);
		}
	}
	if(m_book_palettes->IsShown())
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j< 4; j++)
			{
				gbColor col = m_info->gpu->convertPaletteData(j, i, m_info->gpu->m_bgPaletteData);
				m_palette_bg->SetCellBackgroundColour(wxColour(col.r, col.g, col.b),i,j);
				col = m_info->gpu->convertPaletteData(j, i, m_info->gpu->m_oPaletteData);
				m_palette_ob->SetCellBackgroundColour(wxColour(col.r, col.g, col.b),i,j);
			}
		}
		gbByte bgp = m_info->mem->read(BGP);
		gbByte obp0 = m_info->mem->read(OBP0);
		gbByte obp1 = m_info->mem->read(OBP1);
		for(int i = 0; i < 4; i++)
		{
			gbColor col = m_info->gpu->convertPaletteData(i, bgp);
			m_palette_mono->SetCellBackgroundColour(wxColour(col.r, col.g, col.b),0, i);
			col = m_info->gpu->convertPaletteData(i, obp0);
			m_palette_mono->SetCellBackgroundColour(wxColour(col.r, col.g, col.b),1, i);
			col = m_info->gpu->convertPaletteData(i, obp1);
			m_palette_mono->SetCellBackgroundColour(wxColour(col.r, col.g, col.b),2, i);
		}
		m_palette_mono->Refresh();
		m_palette_bg->Refresh();
		m_palette_ob->Refresh();
	}
	
	m_gpu_enabled->SetValue(m_info->gpu->m_enabled);
	if(lcdc != (temp = m_info->mem->read(LCDC)))
	{
		lcdc = temp;
		m_LCDC->SetLabel(wxString::Format("LCDC: 0x%02X",lcdc));
	}
	if(stat != (temp = m_info->mem->read(LCDS)))
	{
		stat = temp;
		m_STAT->SetLabel(wxString::Format("STAT: 0x%02X", stat));
	}
	if(scx != (temp = m_info->mem->read(SCX)))
	{
		scx = temp;
		m_SCX->SetLabel(wxString::Format("SCX: 0x%02X", scx));
	}
	if(scy != (temp = m_info->mem->read(SCY)))
	{
		scy = temp;
		m_SCY->SetLabel(wxString::Format("SCY: 0x%02X", scy));
	}
	if(ly != (temp = m_info->mem->read(LY)))
	{
		ly = temp;
		m_LY->SetLabel(wxString::Format("LY: 0x%02X", ly));
	}
	if(lyc != (temp = m_info->mem->read(LYC)))
	{
		lyc = temp;
		m_LYC->SetLabel(wxString::Format("LYC: 0x%02X", lyc));
	}
	if(wy != (temp = m_info->mem->read(WY)))
	{
		wy = temp;
		m_WY->SetLabel(wxString::Format("WY: 0x%02X", wy));
	}
	if(wx != (temp = m_info->mem->read(WX)))
	{
		wx = temp;
		m_WX->SetLabel(wxString::Format("WX: 0x%02X", wx));
	}
	if(vbk != (temp = m_info->mem->read(VBK)))
	{
		vbk = temp;
		m_VBK->SetLabel(wxString::Format("VRAM: %d", vbk));
	}
}

void GbGpuDbg::drawTiles(void)
{
	int bufferWidth = m_tile_image->getBufferWidth();
	for(int y = 0; y < 24; y++)
	{
		for(int x = 0; x < 32; x++)
		{
			for(int yi = 0; yi < 8; yi++)
			{
				for(int xi = 0; xi < 8; xi++)
				{
					gbByte pixel = m_info->gpu->getTilePixel(m_info->mem->vram[x/16]+(y*16*8*2+(x%16)*8*2), xi, yi);
					switch(pixel)
					{
					case 0:
						m_tile_image->m_screen[(y*9+yi+1) * bufferWidth+(x*9+xi+1)] = 0xFFFFFF;
						break; 
					case 1:
						m_tile_image->m_screen[(y*9+yi+1) * bufferWidth+(x*9+xi+1)] = 0xD3D3D3;
						break;
					case 2:
						m_tile_image->m_screen[(y*9+yi+1) * bufferWidth+(x*9+xi+1)] = 0xA9A9A9;
						break;
					case 3:
						m_tile_image->m_screen[(y*9+yi+1) * bufferWidth+(x*9+xi+1)] = 0x000000;
						break;
					}
				}
			}
		}
	}
}

void GbGpuDbg::drawBackground(void)
{
	bool bgd = m_background_data->GetSelection() == 2 ? (m_info->mem->read(LCDC) & LCDC_BS) != 0 : m_background_data->GetSelection() == 1;
	bool bgm = m_background_map->GetSelection() == 2 ? (m_info->mem->read(LCDC) & LCDC_BTS) != 0 : m_background_map->GetSelection() == 1;
	bool gbc = (m_info->mem->read(CGB) & 0x80) != 0; // gameboy color mode
	bool grid = m_background_grid_chkbx->GetValue();
	for(int ty = 0; ty < 32; ty++)
	{
		for(int tx = 0; tx < 32; tx++)
		{
			gbBgMap tile = m_info->gpu->getTileData(bgd, bgm, tx+(ty*32));
			for(int y = 0; y < 8; y++)
			{
				for(int x = 0; x < 8; x++)
				{
					gbByte yindex = (tile.attributes & BGMAP_ATTR_VFLIP) ? 7 - y : y;
					gbByte xIndex = (tile.attributes & BGMAP_ATTR_HFLIP) ? 7 - x : x;
					gbByte pixel = m_info->gpu->getTilePixel(tile.tile, xIndex, yindex);
					//gbByte lineData = ((tile.tile[yindex*2] & (1<<xIndex)) ? 0x1 : 0x0)|((tile.tile[yindex*2+1] & (1<<xIndex)) ? 0x2 : 0x0);
					gbColor col;
					if(gbc)
					{
						col = m_info->gpu->convertPaletteData(pixel, tile.attributes & BGMAP_PALETTE, m_info->gpu->m_bgPaletteData);
					} else {
						col = m_info->gpu->convertPaletteData(pixel, m_info->mem->read(BGP));
					}
					if(grid && (x == 0 || y == 0))
					{
						col.color = ~col.color;
					}
					m_background_image->m_screen[tx*8+x+(ty*8+y)*256] = col.color;
				}
			}
		}
	}
	gbByte scx = m_info->mem->read(SCX);
	gbByte scy = m_info->mem->read(SCY);
	for(int x = scx; x < scx+SCREEN_WIDTH; x++)
	{
		m_background_image->m_screen[(x%256)+scy*256] ^= 0x00FF00;
		m_background_image->m_screen[(x%256)+((scy+SCREEN_HEIGHT)%256)*256] ^= 0x00FF00;
	}
	for(int y = scy; y < scy+SCREEN_HEIGHT; y++)
	{
		m_background_image->m_screen[scx+(y%256)*256] ^= 0x00FF00;
		m_background_image->m_screen[((scx+SCREEN_WIDTH)%256)+(y%256)*256] ^= 0x00FF00;
	}
}

void GbGpuDbg::onClose(wxCloseEvent &evt)
{
	if(evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}

void GbGpuDbg::onMouse(wxMouseEvent &evt)
{
	m_mouse = evt.GetPosition();
}