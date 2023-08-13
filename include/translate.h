#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_
#include <ppl7.h>

ppl7::String translate(const char* text);
const ppl7::String& translate(const ppl7::String& text);
const ppl7::String& translate(const ppl7::String& text, const ppl7::String& context);


class Speech
{
public:
    ppl7::String audiofile;
    ppl7::String text;
    ppl7::String phonetics;
    float timeout=0.0f;
    int id;
    Speech(int id) : id(id) {};
};

Speech translate(int id);

class Translator
{
private:
    ppl7::String lang;
    ppl7::AssocArray data;

public:
    Translator();
    ~Translator();
    void load();
    void setLanguage(const ppl7::String& lang);
    const ppl7::String& tr(const ppl7::String& text) const;
    ppl7::String tr(const char* text) const;
    Speech tr(int id) const;
};

Translator& GetTranslator();


#endif
