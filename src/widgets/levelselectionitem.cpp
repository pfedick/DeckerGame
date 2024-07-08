#include <stdio.h>
#include <stdlib.h>

#include "ppltk.h"
#include "widgets.h"
#include "decker.h"



namespace Decker {
namespace ui {

LevelSelectionItem::LevelSelectionItem(LevelSelection* levelselection, const LevelDescription& descr, bool hidden)
{
    this->levelselection=levelselection;
    author=descr.Author;
    filename=descr.Filename;
    this->hidden=hidden;
    Game& g=GetGame();
    ppl7::String lang=g.config.TextLanguage;
    ppl7::String english("en");

    title=translate("unknown level");
    description=translate("no description");
    auto it=descr.LevelName.find(lang);
    if (it != descr.LevelName.end() && it->second.notEmpty()) title=it->second;
    else {
        it=descr.LevelName.find(english);
        if (it != descr.LevelName.end() && it->second.notEmpty()) title=it->second;
    }
    it=descr.Description.find(lang);
    if (it != descr.Description.end() && it->second.notEmpty()) description=it->second;
    else {
        it=descr.Description.find(english);
        if (it != descr.Description.end() && it->second.notEmpty()) description=it->second;
    }
    if (!descr.Thumbnail.isEmpty()) {
        artwork.load(descr.Thumbnail);
        //ppl7::PrintDebug("witdh=%d, height=%d\n", artwork.width(), artwork.height());
    }
    selected=false;
    setClientOffset(7, 7, 7, 7);

}

void PrintDescription(ppl7::grafix::Drawable& draw, const ppl7::String& text, ppl7::grafix::Font& font)
{
    //draw.cls();
    int x=0;
    int y=0;
    ppl7::grafix::Size space=font.measure(ppl7::ToString(" "));
    ppl7::Array words(text, " ");
    for (size_t i=0;i < words.size();i++) {
        ppl7::String& word=words.get(i);
        ppl7::grafix::Size s=font.measure(word);
        if (x + s.width > draw.width()) {
            x=0;
            y+=space.height;
        }

        draw.print(font, x, y, word);
        x+=s.width + space.width;
    }




}

void LevelSelectionItem::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(20, 15, 5, 220);
    ppl7::grafix::Color border(128, 96, 0, 255);

    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
    ppl7::grafix::Font font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(30);
    font.setOrientation(ppl7::grafix::Font::TOP);
    font.setColor(ppl7::grafix::Color(210, 210, 200, 255));

    if (selected) {
        border.setColor(255, 200, 0, 255);
        font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
        bg.setColor(50, 40, 15, 220);
    }
    draw.cls(bg);

    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, 200 - i, border);
    }

    ppl7::grafix::Drawable client=clientDrawable(draw);

    client.print(font, 0, 0, title);

    font.setSize(20);
    ppl7::grafix::Drawable printarea=client.getDrawable(10, 40, client.width() - 340, 160);
    PrintDescription(printarea, description, font);
    //client.print(font, 0, 40, description);


    font.setName("NotoSans");
    font.setBold(false);
    font.setSize(12);
    ppl7::String Tmp;
    if (author.notEmpty()) Tmp=translate("Author:") + " " + author + ", ";

    client.print(font, 0, 160, Tmp + translate("Filename:") + " " + filename);

    if (!artwork.isEmpty()) {
        client.blt(artwork, client.width() - 323, 3);
    }
}


void LevelSelectionItem::mouseEnterEvent(ppltk::MouseEvent* event)
{
    levelselection->clearSelection();
    selected=true;
    ppltk::Event e(ppltk::Event::SelectionChanged);
    e.setWidget(this);
    this->getParent()->selectionChangedEvent(&e);
    needsRedraw();
}

/*
void LevelSelectionItem::mouseLeaveEvent(ppltk::MouseEvent* event)
{
    selected=false;
    needsRedraw();
}
*/



}	// EOF namespace ui
}	// EOF namespace Decker
