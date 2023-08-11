#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_
#include <ppl7.h>

ppl7::String translate(const char* text);
const ppl7::String& translate(const ppl7::String& text);
const ppl7::String& translate(const ppl7::String& text, const ppl7::String& context);

class Translator
{
private:
public:
    Translator();
    ~Translator();
    void load(const ppl7::String& filename);
    const ppl7::String& tr(const ppl7::String& text);
    ppl7::String tr(const char* text);
};

#endif
