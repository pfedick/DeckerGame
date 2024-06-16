#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"
#include "particle.h"

ModifiableParameter::ModifiableParameter()
{
	backgroundType=Background::Type::Color;
}

void ModifiableParameter::clear()
{
	backgroundType=Background::Type::Color;
	BackgroundColor.setColor(32, 32, 64, 255);
	CurrentSong.clear();
	BackgroundImage.clear();
	GlobalLighting.setColor(255, 255, 255, 255);
}

bool ModifiableParameter::operator==(const ModifiableParameter& other) const
{
	if (backgroundType != other.backgroundType) return false;
	if (BackgroundColor != other.BackgroundColor) return false;
	if (BackgroundImage != other.BackgroundImage) return false;
	if (CurrentSong != other.CurrentSong) return false;
	if (GlobalLighting != other.GlobalLighting) return false;
	return true;

}

LevelParameter::LevelParameter()
{
	clear();
}

void LevelParameter::clear()
{
	width=0;
	height=0;
	levelSort=0;
	randomSong=true;
	partOfStory=false;
	visibleInLevelSelection=true;
	backgroundType=Background::Type::Color;
	BackgroundColor.setColor(32, 32, 64, 255);
	Thumbnail.clear();
	LevelName.clear();
	Description.clear();
	InitialSong.clear();
	SongPlaylist.clear();
	BackgroundImage.clear();
	GlobalLighting.setColor(255, 255, 255, 255);
}


static void storeParameters(ppl7::AssocArray& a, const LevelParameter& params)
{
	a.clear();
	a.setf("level_width", "%d", params.width);
	a.setf("level_height", "%d", params.height);
	//a.set("level_name", params.Name);
	a.set("initial_song", params.InitialSong);
	if (params.randomSong) a.set("randomSong", "true");
	else a.set("randomSong", "false");

	if (params.partOfStory) a.set("partOfStory", "true");
	else a.set("partOfStory", "false");

	if (params.visibleInLevelSelection) a.set("visibleInLevelSelection", "true");
	else a.set("visibleInLevelSelection", "false");

	a.setf("levelSort", "%d", params.levelSort);

	if (!params.Thumbnail.isEmpty()) a.set("Thumbnail", params.Thumbnail);

	std::vector<ppl7::String>::const_iterator it;
	for (it=params.SongPlaylist.begin();it != params.SongPlaylist.end();++it) {
		a.set("additional_playlist/[]", (*it));
	}
	if (params.backgroundType == Background::Type::Image)
		a.set("background_type", "image");
	else if (params.backgroundType == Background::Type::Color)
		a.set("background_type", "color");

	a.set("BackgroundImage", params.BackgroundImage);
	a.setf("BackgroundColor", "%d,%d,%d,%d", params.BackgroundColor.red(),
		params.BackgroundColor.green(),
		params.BackgroundColor.blue(),
		params.BackgroundColor.alpha()
	);
	a.setf("GlobalLighting", "%d,%d,%d,%d", params.GlobalLighting.red(),
		params.GlobalLighting.green(),
		params.GlobalLighting.blue(),
		params.GlobalLighting.alpha()
	);


	//std::map<ppl7::String, LevelParameter::TranslatedStrings>::const_iterator it;
	for (auto it=params.LevelName.begin();it != params.LevelName.end();++it) {
		a.set("LevelName/" + it->first, it->second);
	}
	for (auto it=params.Description.begin();it != params.Description.end();++it) {
		a.set("Description/" + it->first, it->second);
	}

	a.list();

}

size_t LevelParameter::size() const
{
	ppl7::AssocArray a;
	storeParameters(a, *this);
	return a.size();
}

void LevelParameter::save(ppl7::File& ff, int chunk_id) const
{
	ppl7::AssocArray a;
	storeParameters(a, *this);
	ppl7::ByteArray ba;
	a.exportBinary(ba);
	unsigned char buffer[6];
	ppl7::Poke32(buffer + 0, ba.size() + 6);
	ppl7::Poke8(buffer + 4, chunk_id);
	ppl7::Poke8(buffer + 5, 1);		// Version
	ff.write(buffer, 6);
	ff.write(ba.ptr(), ba.size());
}

void LevelParameter::load(const ppl7::ByteArrayPtr& ba)
{
	clear();
	ppl7::AssocArray a;
	ppl7::String Default, Tmp;
	const char* buffer=ba.toCharPtr();
	int version=ppl7::Peek8(buffer);
	if (version != 1) {
		printf("Can't load LevelParameter, unknown version! [%d]\n", version);
		return;
	}
	ppl7::ByteArrayPtr assoc_ba(buffer + 1, ba.size() - 1);

	a.importBinary(assoc_ba);
	Default="";

	width=a.getInt("level_width", width);
	height=a.getInt("level_height", height);

	if (a.exists("partOfStory")) partOfStory=a.getString("partOfStory", Default).toBool();
	if (a.exists("visibleInLevelSelection")) partOfStory=a.getString("visibleInLevelSelection", Default).toBool();
	if (a.exists("levelSort")) levelSort=a.getString("levelSort", Default).toInt();
	if (a.exists("level_name")) LevelName["en"]=a.getString("level_name", Default);

	if (a.exists("Thumbnail")) Thumbnail=a.get("Thumbnail").toByteArray();

	if (a.exists("LevelName")) {
		ppl7::AssocArray::const_iterator it;
		ppl7::AssocArray& data=a.getAssocArray("LevelName");
		for (it=data.begin();it != data.end();++it) {
			LevelName.insert(std::pair<ppl7::String, ppl7::String>(it->first, it->second->toString()));
		}
	}

	if (a.exists("Description")) {
		ppl7::AssocArray::const_iterator it;
		ppl7::AssocArray& data=a.getAssocArray("Description");
		for (it=data.begin();it != data.end();++it) {
			Description.insert(std::pair<ppl7::String, ppl7::String>(it->first, it->second->toString()));
		}
	}


	InitialSong=a.getString("initial_song", Default);
	BackgroundImage=a.getString("BackgroundImage", Default);
	Default="true";
	randomSong=a.getString("randomSong", Default).toBool();
	Default="color";
	Tmp=a.getString("background_type", Default);
	if (Tmp == "color") backgroundType=Background::Type::Color;
	else if (Tmp == "image") backgroundType=Background::Type::Image;
	if (a.exists("BackgroundColor")) {
		Tmp=a.getString("BackgroundColor");
		ppl7::Array Tok(Tmp, ",");
		BackgroundColor.setRed(Tok[0].toInt());
		BackgroundColor.setGreen(Tok[1].toInt());
		BackgroundColor.setBlue(Tok[2].toInt());
		BackgroundColor.setAlpha(Tok[3].toInt());
	}
	if (a.exists("additional_playlist")) {
		ppl7::AssocArray& songlist=a.getAssocArray("additional_playlist");
		ppl7::AssocArray::const_iterator it;
		for (it=songlist.begin();it != songlist.end();++it) {
			SongPlaylist.push_back((*it).second->toString());
		}
	}
	if (a.exists("GlobalLighting")) {
		Tmp=a.getString("GlobalLighting");
		ppl7::Array Tok(Tmp, ",");
		GlobalLighting.setRed(Tok[0].toInt());
		GlobalLighting.setGreen(Tok[1].toInt());
		GlobalLighting.setBlue(Tok[2].toInt());
		GlobalLighting.setAlpha(Tok[3].toInt());
	}

	//a.list("level::parameter::load");
}
