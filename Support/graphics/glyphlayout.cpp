
#include <emuall/graphics/font.h>
#include <emuall/graphics/graphicsException.h>


GlyphLayout::GlyphLayout(int lineHeight, int startY) :
	_lineHeight(lineHeight), _startY(startY),
	_elements(new std::vector<GlyphLayout::Element>),
	_layout(new std::vector<GlyphLayout::Layout>)
{

}

GlyphLayout::GlyphLayout(const GlyphLayout &other) :
	_lineHeight(other._lineHeight), _startY(other._startY),
	_elements(new std::vector<GlyphLayout::Element>(*other._elements)),
	_layout(new std::vector<GlyphLayout::Layout>(*other._layout))
{

}

GlyphLayout::~GlyphLayout()
{
	if (_layout != nullptr) {
		delete _layout;
	}
	if (_elements != nullptr) {
		delete _elements;
	}
}

GlyphLayout & GlyphLayout::operator=(const GlyphLayout &other)
{
	_lineHeight = other._lineHeight;
	_startY = other._startY;
	_elements = new std::vector<GlyphLayout::Element>(*other._elements);
	_layout = new std::vector<GlyphLayout::Layout>(*other._layout);
	return *this;
}

void GlyphLayout::AddGlyph(unsigned int index, bool wrapPoint, int advanceX, int advanceY, int bitmapX, int bitmapY)
{
	GlyphLayout::Element element;
	element.index = index;
	element.forceNewLine = false;
	element.wrapPoint = wrapPoint;
	element.advanceX = advanceX;
	element.advanceY = advanceY;
	element.bitmapX = bitmapX;
	element.bitmapY = bitmapY;
	_elements->push_back(element);
}

void GlyphLayout::AddNewLine()
{
	GlyphLayout::Element element;
	element.index = 0;
	element.forceNewLine = true;
	element.wrapPoint = true;
	element.advanceX = 0;
	element.advanceY = 0;
	element.bitmapX = 0;
	element.bitmapY = 0;
	_elements->push_back(element);
}

void GlyphLayout::DoLayout(bool wrapping, int width /*= -1*/, Alignment hAlign /*= Left*/, int height /*= -1*/, Alignment vAlign /*= Top*/)
{
	if (_elements->size() == 0) {
		return;
	}
	int posX = 0;
	int posY = _startY;
	if (width <= 0) {
		wrapping = false;
	}
	_layout->clear();
	// First all characters in one line
	for (auto &element : *_elements) {
		GlyphLayout::Layout layoutElement;
		layoutElement.x = posX;
		layoutElement.y = element.bitmapY;
		layoutElement.index = element.index;
		layoutElement.lineStart = false;
		posX += element.advanceX;
		posY += element.advanceY;
		_layout->push_back(layoutElement);
	}
	// Split up in multiple lines
	posX = 0;
	for (int i = 0; i < (int)_elements->size(); i++) {
		if ((*_elements)[i].forceNewLine) {
			posX = (*_layout)[i].x;
			posY += _lineHeight;
			(*_layout)[i].lineStart = true;

		}
		(*_layout)[i].x -= posX;
		(*_layout)[i].y += -posY;
		if ((wrapping && (*_layout)[i].x >= (width - (*_elements)[i].advanceX))) {
			// Find first wrappable character
			int tooLongPos = i;
			int wrapPos = i;
			while (wrapPos > 0) {
				wrapPos--;
				if ((*_layout)[wrapPos].lineStart || wrapPos == 0) {
					// could not find a wrap point
					wrapPos = tooLongPos - 1;
					break;
				}
				if ((*_elements)[wrapPos].wrapPoint) {
					break;
				}
			}
			while (tooLongPos > wrapPos) {
				(*_layout)[tooLongPos].y -= -posY;
				(*_layout)[i].x += posX;
				tooLongPos--;
			}
			i = wrapPos;
			posX = (*_layout)[i + 1].x;
			(*_layout)[i + 1].lineStart = true;
			posY += _lineHeight;
		}
	}
	(*_layout)[0].lineStart = true;
	// Fix the horizontal alignment
	if (width > 0 && hAlign != Alignment::Left) {
		for (unsigned int i = 0; i < _layout->size(); i++)
		{
			int startX = (*_layout)[i].x;
			int startI = i;
			while ((i < (_layout->size() - 1)) && !(*_layout)[i + 1].lineStart) {
				i++;
			}
			int endI = i;
			int endX = (*_layout)[i].x + (*_elements)[i].advanceX;
			int lineWidth = endX - startX;
			int offset = 0;
			if (hAlign == Alignment::Center) {
				offset = (width - lineWidth) / 2;
			}
			else { // Right
				offset = width - lineWidth;
			}
			for (int j = startI; j <= endI; j++) {
				(*_layout)[j].x += offset;
			}
		}
	}
	// Fix the vertical alignment
	if (height > 0 && vAlign != Alignment::Top) {
		int startY = (*_layout)[0].y;
		int endY = (*_layout)[_layout->size() - 1].y + _lineHeight;
		int textHeight = startY - endY;
		int offset;
		if (vAlign == Alignment::Center) {
			offset = (height - textHeight) / 2;
		}
		else { // bottom
			offset = height - textHeight;
		}
		for (unsigned int i = 0; i < _layout->size(); i++) {
			(*_layout)[i].y -= offset;
		}
	}
}

int GlyphLayout::GetCount() const
{
	return (int)_layout->size();
}

int GlyphLayout::GetPosX(int index) const
{
	if (index >= GetCount())
		throw FontException("Index out of range in layout");
	return (*_layout)[index].x;
}

int GlyphLayout::GetPosY(int index) const
{
	if (index >= GetCount())
		throw FontException("Index out of range in layout");
	return (*_layout)[index].y;
}

unsigned GlyphLayout::GetGlyphIndex(int index) const
{
	if (index >= GetCount())
		throw FontException("Index out of range in layout");
	return (*_layout)[index].index;
}

bool GlyphLayout::IsNewLine(int index) const
{
	if (index >= GetCount())
		throw FontException("Index out of range in layout");
	return (*_elements)[index].forceNewLine;
}
