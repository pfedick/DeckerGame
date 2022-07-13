#include "translate.h"

static Translator* translator=NULL;

ppl7::String translate(const char* text)
{
    if (translator) return translator->tr(text);
    return ppl7::String(text);
}

const ppl7::String& translate(const ppl7::String& text)
{
    if (translator) return translator->tr(text);
    return text;
}


Translator::Translator()
{
    translator=this;
}

Translator::~Translator()
{
    translator=NULL;
}

void Translator::load(const ppl7::String& filename)
{

}

const ppl7::String& Translator::tr(const ppl7::String& text)
{
    return text;
}

ppl7::String Translator::tr(const char* text)
{
    return ppl7::String(text);
}
