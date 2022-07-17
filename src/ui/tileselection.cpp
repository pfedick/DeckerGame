#include "decker.h"
#include "ui.h"

namespace Decker::ui {


TilesSelection::TilesSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	for (int i=0;i <= MAX_TILESETS;i++)
		tilesets[i]=NULL;
	tileset=1;

	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(5, 5, 80, 30, "Tileset: "));
	tileset_combobox=new ppl7::tk::ComboBox(85, 5, client.width() - 85, 25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);

	this->addChild(new ppl7::tk::Label(5, 35, 70, 20, "Layer: "));
	layer0=new ppl7::tk::RadioButton(60, 35, 50, 20, "0", true);
	this->addChild(layer0);

	layer1=new ppl7::tk::RadioButton(110, 35, 50, 20, "1");
	this->addChild(layer1);

	layer2=new ppl7::tk::RadioButton(160, 35, 50, 20, "2");
	this->addChild(layer2);

	layer3=new ppl7::tk::RadioButton(210, 35, 50, 20, "3");
	this->addChild(layer3);

	tilesframe=new TilesFrame(5, 60, client.width() - 10, client.height() - 60 - 300, game);
	this->addChild(tilesframe);


	colorframe=new ColorSelectionFrame(5, client.height() - 300, client.width() - 10, 300, game->getLevel().palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);
	tilesframe->setColor(colorframe->color());
}

void TilesSelection::setSelectedTile(int nr)
{
	tilesframe->setSelectedTile(nr);
}

int TilesSelection::selectedTile() const
{
	return tilesframe->selectedTile();
}

int TilesSelection::currentLayer() const
{
	if (layer0->checked()) return 0;
	if (layer1->checked()) return 1;
	if (layer2->checked()) return 2;
	if (layer3->checked()) return 3;
	return 0;
}

void TilesSelection::setLayer(int layer)
{
	if (layer == 0) layer0->setChecked(true);
	if (layer == 1) layer1->setChecked(true);
	if (layer == 2) layer2->setChecked(true);
	if (layer == 3) layer3->setChecked(true);
}

void TilesSelection::setCurrentTileSet(int id)
{
	if (id<1 || id>MAX_TILESETS) return;
	if (tilesets[id] == NULL) return;
	tileset=id;
	tilesframe->setSprites(tilesets[id]);
	tileset_combobox->setCurrentIndex(id - 1);
	needsRedraw();
}

int TilesSelection::currentTileSet() const
{
	return tileset;
}

void TilesSelection::setTileSet(int id, const ppl7::String& name, SpriteTexture* sprites)
{
	if (id<1 || id>MAX_TILESETS) return;
	tilesets[id]=sprites;
	tilesetName[id]=name;
	tileset_combobox->add(name, ppl7::ToString("%d", id));
	if (id == 1) setCurrentTileSet(1);

}

int TilesSelection::colorIndex() const
{
	return colorframe->colorIndex();
}

void TilesSelection::setColorIndex(int index)
{
	colorframe->setColorIndex(index);
}

void TilesSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tileset_combobox) {
		//printf("value=%d\n",value);
		setCurrentTileSet(value + 1);
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
	}
}


ColorSelectionFrame::ColorSelectionFrame(int x, int y, int width, int height, ColorPalette& palette)
	: ppl7::tk::Widget(x, y, width, height), palette(palette)
{
	color_palette=NULL;
	color_name=NULL;
	slider_red=NULL;
	slider_green=NULL;
	slider_blue=NULL;
	color_red=NULL;
	color_green=NULL;
	color_blue=NULL;

	ppl7::grafix::Rect client=this->clientRect();

	color_palette=new ColorPaletteFrame(0, 0, client.width(), 160, palette);
	color_palette->setEventHandler(this);
	addChild(color_palette);

	int y1=165;

	addChild(new ppl7::tk::Label(0, y1, 50, 25, "Name:"));
	color_name=new ppl7::tk::LineInput(50, y1, client.width() - 50, 25, "");
	color_name->setEventHandler(this);
	color_name->setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	color_name->setColor(ppl7::grafix::Color(230, 230, 230, 255));
	addChild(color_name);
	y1+=25;

	addChild(new ppl7::tk::Label(0, y1, 50, 25, "red:"));
	slider_red=new ppl7::tk::HorizontalSlider(50, y1, client.width() - 50, 25);
	slider_red->setEventHandler(this);
	slider_red->setLimits(0, 255);
	addChild(slider_red);
	y1+=25;

	addChild(new ppl7::tk::Label(0, y1, 50, 25, "green:"));
	slider_green=new ppl7::tk::HorizontalSlider(50, y1, client.width() - 50, 25);
	slider_green->setEventHandler(this);
	slider_green->setLimits(0, 255);
	addChild(slider_green);
	y1+=25;

	addChild(new ppl7::tk::Label(0, y1, 50, 25, "blue:"));
	slider_blue=new ppl7::tk::HorizontalSlider(50, y1, client.width() - 50, 25);
	slider_blue->setEventHandler(this);
	slider_blue->setLimits(0, 255);
	addChild(slider_blue);
	y1+=30;

	int x1=0;
	addChild(new ppl7::tk::Label(x1, y1, 20, 25, "r:"));
	color_red=new ppl7::tk::SpinBox(x1 + 20, y1, 70, 25, 0);
	color_red->setEventHandler(this);
	color_red->setLimits(0, 255);
	color_red->setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	color_red->setColor(ppl7::grafix::Color(230, 230, 230, 255));

	addChild(color_red);
	x1+=90;

	addChild(new ppl7::tk::Label(x1, y1, 20, 25, "g:"));
	color_green=new ppl7::tk::SpinBox(x1 + 20, y1, 70, 25, 0);
	color_green->setEventHandler(this);
	color_green->setLimits(0, 255);
	color_green->setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	color_green->setColor(ppl7::grafix::Color(230, 230, 230, 255));

	addChild(color_green);
	x1+=90;

	addChild(new ppl7::tk::Label(x1, y1, 20, 25, "b:"));
	color_blue=new ppl7::tk::SpinBox(x1 + 20, y1, 70, 25, 0);
	color_blue->setEventHandler(this);
	color_blue->setLimits(0, 255);
	color_blue->setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	color_blue->setColor(ppl7::grafix::Color(230, 230, 230, 255));

	addChild(color_blue);

	color_palette->setColorIndex(2);

	ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
	wm->setKeyboardFocus(color_name);
}


int ColorSelectionFrame::colorIndex() const
{
	return color_palette->colorIndex();
}

ppl7::grafix::Color ColorSelectionFrame::color() const
{
	return color_palette->color();
}

void ColorSelectionFrame::setColorIndex(int index)
{
	color_palette->setColorIndex(index);
}

void ColorSelectionFrame::paint(ppl7::grafix::Drawable& draw)
{

}

void ColorSelectionFrame::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	ppl7::tk::Widget* w=event->widget();
	if (w == color_name) {
		if (text != palette.getName(color_palette->colorIndex())) {
			//printf("Name changed\n");
			palette.setName(color_palette->colorIndex(), text.trimmed());
		}

	}
	//printf("ColorSelectionFrame::textChangedEvent\n");

}

void ColorSelectionFrame::sendEventValueChanged()
{
	ppl7::tk::Event new_event(ppl7::tk::Event::ValueChanged);
	new_event.setWidget(this);
	EventHandler::valueChangedEvent(&new_event, color_palette->colorIndex());
}


void ColorSelectionFrame::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	ppl7::tk::Widget* w=event->widget();
	if (w == color_palette) {
		int color_index=color_palette->colorIndex();
		ColorPaletteItem item=palette.get(color_index);
		if (color_name) color_name->setText(item.name);
		if (slider_red) slider_red->setValue(item.color.red());
		if (slider_green) slider_green->setValue(item.color.green());
		if (slider_blue) slider_blue->setValue(item.color.blue());
		if (color_red) color_red->setValue(item.color.red());
		if (color_green) color_green->setValue(item.color.green());
		if (color_blue) color_blue->setValue(item.color.blue());
		sendEventValueChanged();
	}
}
void ColorSelectionFrame::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	ppl7::tk::Widget* w=event->widget();
	if (w == slider_red) {
		if (color_red) color_red->setValue(slider_red->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setRed(slider_red->value());
		palette.setColor(color_palette->colorIndex(), c);
		color_palette->needsRedraw();
		sendEventValueChanged();
	} else if (w == slider_green) {
		if (color_green) color_green->setValue(slider_green->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setGreen(slider_green->value());
		palette.setColor(color_palette->colorIndex(), c);
		color_palette->needsRedraw();
		sendEventValueChanged();
	} else if (w == slider_blue) {
		if (color_blue) color_blue->setValue(slider_blue->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setBlue(slider_blue->value());
		palette.setColor(color_palette->colorIndex(), c);
		color_palette->needsRedraw();
		sendEventValueChanged();
	} else if (w == color_red) {
		if (slider_red) slider_red->setValue(color_red->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setRed(color_red->value());
		palette.setColor(color_palette->colorIndex(), c);
		sendEventValueChanged();
	} else if (w == color_green) {
		if (slider_green) slider_green->setValue(color_green->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setGreen(color_green->value());
		palette.setColor(color_palette->colorIndex(), c);
		sendEventValueChanged();
	} else if (w == color_blue) {
		if (slider_blue) slider_blue->setValue(color_blue->value());
		ppl7::grafix::Color c=palette.getColor(color_palette->colorIndex());
		c.setBlue(color_blue->value());
		palette.setColor(color_palette->colorIndex(), c);
		sendEventValueChanged();
	}
}

void ColorSelectionFrame::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	//printf("ColorSelectionFrame::keyDownEvent\n");
	ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
	ppl7::tk::Widget* widget=event->widget();
	if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
		&& (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) == 0) {
		   // Tab forward
		if (widget == color_red) wm->setKeyboardFocus(color_green);
		else if (widget == color_green) wm->setKeyboardFocus(color_blue);
		else if (widget == color_blue) wm->setKeyboardFocus(color_name);
		else if (widget == color_name) wm->setKeyboardFocus(color_red);

	} else if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
		&& (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) != 0) {
		   // Tab backward
		if (widget == color_blue) wm->setKeyboardFocus(color_green);
		else if (widget == color_green) wm->setKeyboardFocus(color_red);
		else if (widget == color_red) wm->setKeyboardFocus(color_name);
		else if (widget == color_name) wm->setKeyboardFocus(color_blue);
	}
}


ColorPaletteFrame::ColorPaletteFrame(int x, int y, int width, int height, ColorPalette& palette)
	: ppl7::tk::Frame(x, y, width, height, ppl7::tk::Frame::BorderStyle::Inset), palette(palette)
{
	color_index=0;
	setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	//this->setClientOffset(4, 4, 4, 4);
	tsize=20;
	ppl7::grafix::Rect client=this->clientRect();
	scrollbar=new ppl7::tk::Scrollbar(client.x2 - 28, 0, 27, client.height());
	scrollbar->setEventHandler(this);
	//printf("this->width=%d, client.width=%d\n", this->width(), client.width());
	items_per_row=(client.width() - 30) / tsize;
	rows=255 / items_per_row;
	//printf("items per row: %d, rows: %d\n", items_per_row, rows);
	scrollbar->setSize(rows);
	scrollbar->setVisibleItems(client.height() / tsize);
	scrollbar->setPosition(0);
	this->addChild(scrollbar);
}

ColorPaletteFrame::~ColorPaletteFrame()
{
	delete scrollbar;
}

void ColorPaletteFrame::setColorIndex(int index)
{
	color_index=index;
	needsRedraw();
	ppl7::tk::Event event(ppl7::tk::Event::ValueChanged);
	event.setWidget(this);
	EventHandler::valueChangedEvent(&event, color_index);
}

int ColorPaletteFrame::colorIndex() const
{
	return color_index;
}

ppl7::grafix::Color ColorPaletteFrame::color() const
{
	return palette.getColor(color_index);
}

void ColorPaletteFrame::paint(ppl7::grafix::Drawable& draw)
{
	Frame::paint(draw);
	ppl7::grafix::Rect client=this->clientRect();
	client.x2=client.x2 - 30;
	//draw.drawRect(client, ppl7::grafix::Color(255, 0, 0, 255));
	int x=client.x1;
	int y=client.y1;
	ppl7::grafix::Color selection(255, 255, 255, 255);
	ppl7::grafix::Color bg=this->backgroundColor();
	//printf("draw.width=%d, client.width=%d\n", draw.width(), client.width());

	for (int i=scrollbar->position() * items_per_row;i < 256;i++) {
		if (i == color_index) draw.fillRect(x, y, x + tsize - 1, y + tsize - 1, selection);
		draw.fillRect(x + 1, y + 1, x + tsize - 2, y + tsize - 2, palette.getColor(i));
		if (i == color_index) draw.drawRect(x + 1, y + 1, x + tsize - 2, y + tsize - 2, bg);
		x+=tsize;
		if (x + tsize > client.width()) {
			x=client.x1;
			y+=tsize;
			if (y + tsize > client.height()) break;
		}
	}

}

void ColorPaletteFrame::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == this) {
		int new_index=(event->p.y / tsize) * items_per_row + (event->p.x / tsize) + scrollbar->position() * items_per_row;
		if (new_index < 256) {
			const Uint8* state = SDL_GetKeyboardState(NULL);
			if (event->buttonMask & ppl7::tk::MouseState::Left && !state[SDL_SCANCODE_LSHIFT] && new_index != color_index) {
				color_index=new_index;
			} else if (event->buttonMask & ppl7::tk::MouseState::Right) {
				color_clipboard=palette.getColor(new_index);

			} else if (event->buttonMask & ppl7::tk::MouseState::Middle || (event->buttonMask & ppl7::tk::MouseState::Left && state[SDL_SCANCODE_LSHIFT])) {
				palette.setColor(new_index, color_clipboard);
			}
			color_index=new_index;
			needsRedraw();
			ppl7::tk::Event event(ppl7::tk::Event::ValueChanged);
			event.setWidget(this);
			EventHandler::valueChangedEvent(&event, color_index);
		}
	}
}

void ColorPaletteFrame::mouseWheelEvent(ppl7::tk::MouseEvent* event)
{
	if (event->wheel.y != 0) {
		scrollbar->setPosition(scrollbar->position() + event->wheel.y * -1);
		needsRedraw();
	}
}

void ColorPaletteFrame::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == scrollbar) {
		needsRedraw();
	}
}

} //EOF namespace Decker::ui
