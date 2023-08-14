#include "translate.h"

static Translator* translator=NULL;


Translator& GetTranslator()
{
    return *translator;
}

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

Translator::Speech translate(int id)
{
    if (translator) return translator->tr(id);
    return Translator::Speech(id);
}


Translator::Translator()
{
    translator=this;
    lang="en";
}

Translator::~Translator()
{
    translator=NULL;
}

void Translator::load()
{
    ppl7::Dir dir;
    if (!dir.tryOpen("res/lang")) {
        return;
    };
    ppl7::Dir::Iterator it;
    dir.reset(it);
    ppl7::DirEntry e;
    while (dir.getNextPattern(e, it, "*.json")) {
        //ppl7::PrintDebug("loading language file: %s\n", (const char*)e.Filename);
        ppl7::String filename="res/lang/" + e.Filename;
        ppl7::AssocArray a;
        try {
            ppl7::File file(filename);
            ppl7::Json::load(a, file);
        } catch (const ppl7::Exception& exp) {
            ppl7::PrintDebug("ERROR: could not load language file: %s\n", (const char*)e.Filename);
            exp.print();
        }
        //a.list();
        data+=a;
    }
    //data.list();

}

void Translator::setLanguage(const ppl7::String& lang)
{
    this->lang=lang;
}

const ppl7::String& Translator::tr(const ppl7::String& text) const
{
    if (lang.isEmpty()) return text;
    ppl7::String path="ui/" + text;
    //ppl7::PrintDebugTime("path=>>%s<<\n", (const char*)path);
    if (data.exists(path)) {
        const ppl7::AssocArray& node=data.getAssocArray(path);
        if (node.exists(lang)) return node.getString(lang);
        if (lang == "en") return text;
    }
    ppl7::PrintDebugTime("missing translation: ui => %s\n", (const char*)text);
    return text;
}

ppl7::String Translator::tr(const char* text) const
{
    return tr(ppl7::String(text));
}

Translator::Speech Translator::tr(int id) const
{
    Translator::Speech speech(id);
    ppl7::String path=ppl7::ToString("speech/%d", id);
    if (data.exists(path)) {
        const ppl7::AssocArray& node=data.getAssocArray(path);
        if (node.exists("audiofile")) speech.audiofile=node.getString("audiofile");
        if (node.exists("en/text")) speech.text=node.getString("en/text");
        if (node.exists("en/phonetics")) speech.phonetics=node.getString("en/phonetics");
        if (lang.notEmpty() && node.exists(lang) && lang != "en") {
            const ppl7::AssocArray& lang_node=node.getAssocArray(lang);
            if (lang_node.exists("text")) speech.text=lang_node.getString("text");
            if (lang_node.exists("phonetics")) speech.phonetics=lang_node.getString("phonetics");
        }
    } else {
        ppl7::PrintDebugTime("missing translation for id: %d\n", id);
    }

    return speech;
}
