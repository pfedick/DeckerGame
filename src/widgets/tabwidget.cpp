#include <stdio.h>
#include <stdlib.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"


namespace Decker::ui {

class TabWidgetItem : public ppl7::tk::Widget
{
public:
    TabWidgetItem(int x, int y, int width, int height);
    void paint(ppl7::grafix::Drawable& draw) override;

};

TabWidgetItem::TabWidgetItem(int x, int y, int width, int height)
    :ppl7::tk::Widget(x, y, width, height)
{

}

void TabWidgetItem::paint(ppl7::grafix::Drawable& draw)
{

}

TabWidget::Tab::Tab() {
    widget=NULL;
}

TabWidget::Tab::Tab(const Tab& other) {
    widget=other.widget;
    title=other.title;
    icon=other.icon;
}

TabWidget::Tab::Tab(ppl7::tk::Widget* widget, const ppl7::String& title, const ppl7::grafix::Drawable& icon) {
    this->widget=widget;
    this->title=title;
    this->icon=icon;
}




TabWidget::TabWidget(int x, int y, int width, int height)
    : ppl7::tk::Widget(x, y, width, height)
{
    this->setTransparent(true);
    current_tab=-1;
    //setClientOffset(5, 35, width - 10, height - 40);

    current_child=NULL;
}

TabWidget::~TabWidget()
{
    clear();
}

ppl7::tk::Widget* TabWidget::addTab(int id, const ppl7::String& title, const ppl7::grafix::Drawable& icon)
{
    ppl7::grafix::Rect client=clientRect();
    TabWidgetItem* widget=new TabWidgetItem(client.x1 + 4, client.y1 + 40, client.width() - 8, client.height() - 40);
    widget->setClientOffset(4, 4, 4, 4);
    Tab new_tab(widget, title, icon);
    tabs[id]=new_tab;
    needsRedraw();
    if (current_tab < 0) setCurrentTab(id);
    return widget;
}

void TabWidget::addWidget(int id, const ppl7::String& title, ppl7::tk::Widget* widget, const ppl7::grafix::Drawable& icon)
{
    ppl7::grafix::Rect client=clientRect();
    widget->setPos(client.x1 + 4, client.y1 + 40);
    widget->setSize(client.width() - 8, client.height() - 44);
    Tab new_tab(widget, title, icon);
    tabs[id]=new_tab;
    if (current_tab < 0) setCurrentTab(id);
    needsRedraw();
}

void TabWidget::removeTab(int id)
{
    Widget* widget=getWidget(id);
    if (widget) {
        if (widget == current_child) {
            this->removeChild(current_child);
            current_child=NULL;
        }
        tabs.erase(id);
        delete widget;
    }
    // TODO current_tab aendern
    if (current_tab == id) {
        std::map<int, Tab>::const_iterator it=tabs.begin();
        if (it != tabs.end()) setCurrentTab(it->first);
    }
    needsRedraw();
}

void TabWidget::clear()
{
    std::map<int, Tab>::iterator it;
    for (it=tabs.begin();it != tabs.end();++it) {
        if (it->second.widget) delete it->second.widget;
    }
    tabs.clear();
    current_child=NULL;
    needsRedraw();
}

void TabWidget::setVisible(int id, bool visible)
{
    std::map<int, Tab>::iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    if (it->second.widget) it->second.widget->setVisible(visible);

    needsRedraw();
}

void TabWidget::setEnabled(int id, bool enabled)
{
    std::map<int, Tab>::iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    if (it->second.widget) it->second.widget->setEnabled(enabled);
    needsRedraw();
}

void TabWidget::setTitle(int id, const ppl7::String& title)
{
    std::map<int, Tab>::iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    it->second.title=title;
    needsRedraw();
}

void TabWidget::setIcon(int id, const ppl7::grafix::Drawable& icon)
{
    std::map<int, Tab>::iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    it->second.icon=icon;
    needsRedraw();

}

void TabWidget::setWidget(int id, Widget* widget)
{
    std::map<int, Tab>::iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    if (it->second.widget) delete it->second.widget;
    it->second.widget=widget;
    needsRedraw();
}


ppl7::tk::Widget* TabWidget::getWidget(int id) const
{
    std::map<int, Tab>::const_iterator it;
    it=tabs.find(id);
    if (it != tabs.end()) return it->second.widget;
    return NULL;
}

void TabWidget::setCurrentTab(int id)
{
    if (id == current_tab) return;
    std::map<int, Tab>::const_iterator it;
    it=tabs.find(id);
    if (it == tabs.end()) return;
    if (!it->second.widget) return;
    if (!it->second.widget->isVisible()) return;
    if (current_child) removeChild(current_child);
    current_child=it->second.widget;
    ppl7::grafix::Rect client=clientRect();
    current_child->setPos(client.x1 + 4, client.y1 + 40);
    current_child->setSize(client.width() - 12, client.height() - 48);
    this->addChild(current_child);
    current_tab=id;
    //printf("setCurrentTab gesetzt: %d\n", id);
    needsRedraw();
}

int TabWidget::currentTab() const
{
    return current_tab;
}

size_t TabWidget::tabCount() const
{
    return tabs.size();
}

ppl7::String TabWidget::widgetType() const
{
    return "TabWidget";
}

void TabWidget::paint(ppl7::grafix::Drawable& draw)
{
    if (tabs.empty()) return;
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
    ppl7::grafix::Color light=style.frameBackgroundColor * 1.8f;
    ppl7::grafix::Color shadow=style.frameBackgroundColor * 0.4f;
    ppl7::grafix::Font font=style.labelFont;
    font.setColor(style.labelFontColor);
    font.setOrientation(ppl7::grafix::Font::Orientation::TOP);
    //draw.cls(ppl7::grafix::Color(255, 0, 0, 255));
    int y=30;
    int w=draw.width() - 1;
    int h=draw.height() - 1;
    ppl7::grafix::Color notselected_bg=style.frameBackgroundColor * 0.9f;

    //draw.drawRect(0, y, w, h, style.frameBackgroundColor);
    draw.fillRect(0, y, w, h, style.frameBackgroundColor);

    draw.line(0, y, w, y, light);
    draw.line(0, y, 0, h, light);
    draw.line(0, h, w, h, shadow);
    draw.line(w, y, w, h, shadow);
    if (w) {
        int x=0;
        std::map<int, Tab>::iterator it;
        for (it=tabs.begin();it != tabs.end();++it) {
            if (it->second.widget && it->second.widget->isVisible()) {
                it->second.x=x;
                ppl7::WideString text=it->second.title;
                ppl7::grafix::Size s=font.measure(text);
                w=s.width + 15;
                if (!it->second.icon.isEmpty()) w+=4 + it->second.icon.width();
                it->second.width=w + 1;
                y=0;
                if (it->first == current_tab) {
                    draw.fillRect(x, 0, x + w, 31, style.frameBackgroundColor);
                    draw.line(x, y, x + w, y, light);
                    draw.line(x, y, x, y + 30, light);
                    draw.line(x + w, y, x + w, y + 29, shadow);
                    int tx=x;
                    if (!it->second.icon.isEmpty()) {
                        draw.bltAlpha(it->second.icon, x + 8, y + 4);
                        tx+=it->second.icon.width();
                    }
                    draw.print(font, tx + 8, y + 4, text);
                } else {
                    draw.fillRect(x, 4, x + w, 30, notselected_bg);
                    draw.line(x, y + 4, x + w, y + 4, light);
                    if (x == 0) draw.line(x, y + 4, x, y + 30, light);
                    draw.line(x + w, y + 4, x + w, y + 29, shadow);
                    int tx=x;
                    if (!it->second.icon.isEmpty()) {
                        draw.bltAlpha(it->second.icon, x + 8, y + 4);
                        tx+=it->second.icon.width();
                    }
                    draw.print(font, tx + 8, y + 6, text);
                }

                x+=it->second.width;
            }
        }
    }
}

void TabWidget::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
    if (event->p.y <= 30) {
        std::map<int, Tab>::const_iterator it;
        for (it=tabs.begin();it != tabs.end();++it) {
            if (event->p.x >= it->second.x && event->p.x < it->second.x + it->second.width) {
                if (it->first != current_tab) setCurrentTab(it->first);
                return;
            }
        }
    }
}



}
