#include <stdio.h>
#include <stdlib.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"


namespace Decker::ui {

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
    printf("addTab: %d:%d, %d:%d\n", client.x1, client.y1, client.width(), client.height());
    ppl7::tk::Frame* widget=new ppl7::tk::Frame(client.x1, client.y1, client.width(), client.height());
    Tab new_tab(widget, title, icon);
    tabs[id]=new_tab;
    needsRedraw();
    if (current_tab < 0) setCurrentTab(id);
    return widget;
}

void TabWidget::addWidget(int id, const ppl7::String& title, ppl7::tk::Widget* widget, const ppl7::grafix::Drawable& icon)
{
    ppl7::grafix::Rect client=clientRect();
    widget->setPos(client.x1, client.y1);
    widget->setSize(client.width(), client.height());
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
    this->addChild(current_child);
    current_tab=id;
    printf("setCurrentTab gesetzt: %d\n", id);
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
    Widget* widget=getWidget(current_tab);
    int y=30;
    int w=draw.width();
    int h=draw.height() - y;
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();

    draw.drawRect(0, y, w, h, style.frameBackgroundColor);

    draw.line(0, y, w, y, style.frameBorderColorLight);
    draw.line(0, y, 0, h, style.frameBorderColorLight);
    draw.line(0, h, w, h, style.frameBorderColorShadow);
    draw.line(w, y, w, h, style.frameBorderColorShadow);
    int x=0;
    if (w) {
        std::map<int, Tab>::const_iterator it;
        for (it=tabs.begin();it != tabs.end();++it) {
            if (it->second.widget && it->second.widget->isVisible()) {

                //draw.drawRect(0, y, w, h, style.frameBackgroundColor);
            }
        }
    }
}

void TabWidget::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
    printf("TabWidget::mouseDownEvent\n");
}



}
