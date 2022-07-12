#include "widgets.h"
#include "translate.h"

#ifdef WIN32
#include <fileapi.h>
#endif

namespace Decker {
namespace ui {

FileDialog::FileDialog(int width, int height, FileMode mode)
    : Dialog(width, height, Dialog::None)
{
    my_state=DialogState::Open;
    ppl7::String path=ppl7::Dir::currentPath();
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    if (mode == FileMode::AnyFile) {
        setWindowTitle(translate("save file"));
        setWindowIcon(gfx->Toolbar.getDrawable(33));

    } else {
        setWindowTitle(translate("load file"));
        setWindowIcon(gfx->Toolbar.getDrawable(32));
    }
    ppl7::grafix::Size clientarea=this->clientSize();

    ok_button=new ppl7::tk::Button(20, clientarea.height - 40, 200, 30, translate("OK"), gfx->Toolbar.getDrawable(24));
    ok_button->setEventHandler(this);
    cancel_button=new ppl7::tk::Button(clientarea.width - 220, clientarea.height - 40, 200, 30, translate("Cancel"), gfx->Toolbar.getDrawable(25));
    cancel_button->setEventHandler(this);
    this->addChild(ok_button);
    this->addChild(cancel_button);
    ppl7::tk::Label* label;
    ppl7::String text;
    ppl7::grafix::Size text_size;
    text=translate("Path:");

    label=new ppl7::tk::Label(10, 0, 100, 30, text);
    text_size=label->font().measure(text);
    label->setWidth(text_size.width + 10);

    this->addChild(label);

    path_lineinput=new ppl7::tk::LineInput(10 + text_size.width + 10, 0, clientarea.width - 30 - text_size.width, 30, path);
    path_lineinput->setEventHandler(this);
    this->addChild(path_lineinput);

    int y=40;
#ifdef WIN32
    text=translate("Drives:");
    text_size=label->font().measure(text);
    label=new ppl7::tk::Label(10, 40, text_size.width + 10, 30, text);
    text_size=label->font().measure(text);
    drives_combobox=new Decker::ui::ComboBox(20 + text_size.width, 40, 100, 30);
    drives_combobox->setEventHandler(this);
    wchar_t* drives=new wchar_t[MAX_PATH * sizeof(wchar_t)]();
    if (drives != NULL && GetLogicalDriveStringsW(MAX_PATH * sizeof(wchar_t), drives) > 0) {
        int i=0;
        while (drives[i] != 0) {
            ppl7::WideString str(drives + i);
            if (ppl7::Dir::canOpen(str)) drives_combobox->add(str);
            i+=str.size() + 1;
        }
    }
    delete[] drives;
    this->addChild(label);
    this->addChild(drives_combobox);
    y+=35;
#endif
    dir_list=new ppl7::tk::ListWidget(10, y, 260, clientarea.height - y - 90);
    dir_list->setEventHandler(this);
    this->addChild(dir_list);

    file_list=new ppl7::tk::ListWidget(280, y, clientarea.width - 290, clientarea.height - y - 90);
    file_list->setEventHandler(this);
    this->addChild(file_list);


    text=translate("Filename:");
    text_size=label->font().measure(text);
    label=new ppl7::tk::Label(10, clientarea.height - 80, text_size.width + 10, 30, text);
    this->addChild(label);
    filename_lineinput=new ppl7::tk::LineInput(20 + text_size.width, clientarea.height - 80, clientarea.width - text_size.width - 30 - 300, 30);
    filename_lineinput->setEventHandler(this);
    this->addChild(filename_lineinput);
    setDirectory(path);
}

FileDialog::~FileDialog()
{

}

ppl7::String FileDialog::directory() const
{
    ppl7::String path=path_lineinput->text();
    path.trim();
    path.replace("//", "/");
#ifdef WIN32
    path.replace("/", "\\");
    path.replace("\\\\", "\\");
#endif
    return path;
}

ppl7::String FileDialog::filename() const
{
    ppl7::String filename=directory() + "/" + ppl7::String(filename_lineinput->text()).trimmed();
    filename.replace("//", "/");
#ifdef WIN32
    filename.replace("/", "\\");
    filename.replace("\\\\", "\\");
#endif
    return filename;
}

FileDialog::DialogState FileDialog::state() const
{
    return my_state;
}


void FileDialog::setFilename(const ppl7::String& filename)
{
    ppl7::String filecopy=filename;
#ifdef WIN32
    filecopy.replace("/", "\\");
    if (filecopy.len() < 3) filecopy=ppl7::Dir::currentPath() + "\\" + filename;
    filecopy.replace("\\\\", "\\");
    if (filecopy.mid(1, 2) != ":\\") filecopy=ppl7::Dir::currentPath() + "\\" + filename;
    filecopy.replace("\\\\", "\\");
#else
    filecopy.replace("\\", "/");
    if (filecopy.left(1) != "/") filecopy=ppl7::Dir::currentPath() + "/" + filename;
    filecopy.replace("//", "/");
#endif

    ppl7::String path=ppl7::File::getPath(filecopy);
    ppl7::String file=ppl7::File::getFilename(filecopy);
    setDirectory(path);
    filename_lineinput->setText(file);
    file_list->setCurrentText(file);
}

void FileDialog::setDirectory(const ppl7::String& path)
{
    ppl7::DirEntry entry;
    ppl7::String new_path=path.trimmed();
#ifdef WIN32
    new_path.replace("/", "\\");
#else
    new_path.replace("\\", "/");
#endif
    if (!ppl7::File::tryStatFile(new_path, entry)) {
        return;
    }
    if (entry.isFile()) {
        setDirectory(ppl7::File::getPath(new_path));
        return;
    }

    if (!entry.isDir()) return;
    ppl7::Dir dir;
    if (!dir.tryOpen(new_path, ppl7::Dir::SORT_FILENAME_IGNORCASE)) {
        printf("kann dir >>%s<< nicht oeffnen\n", (const char*)new_path);
        return;
    }
    dir_list->clear();
    file_list->clear();
    path_lineinput->setText(new_path);
    ppl7::Dir::Iterator it;
    dir.reset(it);
    while (dir.getNext(entry, it)) {
        if (entry.isDir() && entry.Filename != ".") {
            dir_list->add(entry.Filename);
        } else if (entry.isFile()) {
            file_list->add(entry.Filename);
        }
    }
}

void FileDialog::setFilter(const ppl7::String& filter)
{

}

void FileDialog::mouseDblClickEvent(ppl7::tk::MouseEvent* event)
{
    //printf("FileDialog::mouseDblClickEvent\n");
    if (event->widget() == dir_list) {
        ppl7::String new_path=dir_list->currentText();
        if (new_path == ".") return;
        if (new_path == "..") {
            new_path=ppl7::String(path_lineinput->text()).trimmed();
            new_path.replace("\\", "/");
            ppl7::Array token=ppl7::StrTok(new_path, "/");
            try {
                token.pop();
#ifdef WIN32
                new_path=token.implode("/") + "/";
#else
                new_path="/" + token.implode("/");
#endif
                new_path.replace("//", "/");

                setDirectory(new_path);
            }
            catch (...) {}

        } else {
            new_path=ppl7::String(path_lineinput->text()).trimmed();
            new_path.trimRight("/\\");
            new_path+="/" + dir_list->currentText().trimmed();
            setDirectory(new_path);
        }
        return;
    } else if (event->widget() == file_list) {
        ppl7::String new_file=file_list->currentText();
        filename_lineinput->setText(new_file);
        my_state=DialogState::OK;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    }
    Dialog::mouseDblClickEvent(event);
}

void FileDialog::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == file_list) {
        ppl7::String new_file=file_list->currentText();
        filename_lineinput->setText(new_file);
        return;
    } else if (event->widget() == ok_button) {
        my_state=DialogState::OK;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    } else if (event->widget() == cancel_button) {
        my_state=DialogState::Aborted;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    }
    Dialog::mouseClickEvent(event);
}

void FileDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == file_list) {
        ppl7::String new_file=file_list->currentText();
        filename_lineinput->setText(new_file);
    }
    Dialog::mouseDownEvent(event);

}

void FileDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
#ifdef WIN32
    if (event->widget() == drives_combobox) {
        ppl7::String path=drives_combobox->currentText();
        dir_list->clear();
        file_list->clear();
        setDirectory(path);
    }
#endif
    Dialog::valueChangedEvent(event, value);
}

void FileDialog::keyDownEvent(ppl7::tk::KeyEvent* event)
{
    //printf("FileDialog::keyDownEvent(keycode=%i, repeat=%i, modifier: %i)\n", event->key, event->repeat, event->modifier);
    if (event->widget() == path_lineinput) {
        //printf("FileDialog::keyDownEvent(keycode=%i, repeat=%i, modifier: %i)\n", event->key, event->repeat, event->modifier);
        if (event->key == ppl7::tk::KeyEvent::KEY_RETURN) {
            ppl7::String new_path=directory();
            setDirectory(new_path);
        }
    } else if (event->widget() == filename_lineinput) {
        if (event->key == ppl7::tk::KeyEvent::KEY_RETURN) {
            my_state=DialogState::OK;
            ppl7::tk::Event event(ppl7::tk::Event::Close);
            event.setWidget(this);
            EventHandler::closeEvent(&event);
            return;
        }
    }
}


}	// EOF namespace ui
}	// EOF namespace Decker
