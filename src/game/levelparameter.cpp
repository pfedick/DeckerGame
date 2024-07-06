#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"
#include "particle.h"


LevelDescription::LevelDescription()
{
	partOfStory=false;
	visibleInLevelSelection=false;
	isCustomLevel=false;
	levelSort=0;
}

void LevelDescription::clear()
{
	partOfStory=false;
	visibleInLevelSelection=false;
	isCustomLevel=false;
	Filename.clear();
	levelSort=0;
	LevelName.clear();
	Description.clear();
	Thumbnail.clear();
}

void LevelDescription::loadFromAssocArray(const ppl7::AssocArray& a)
{
	ppl7::String Default;
	Default="";
	if (a.exists("partOfStory")) partOfStory=a.getString("partOfStory", Default).toBool();
	if (a.exists("visibleInLevelSelection")) visibleInLevelSelection=a.getString("visibleInLevelSelection", Default).toBool();
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

}

bool LevelDescription::loadFromFile(const ppl7::String& filename)
{
	ppl7::File ff;
	try {
		ff.open(filename, ppl7::File::READ);
		ppl7::ByteArray ba;
		ff.read(ba, 7);
		const char* buffer=ba.toCharPtr();
		if (memcmp(buffer, "Decker", 7) != 0) {
			//printf("Invalid Fileformat\n");
			return false;
		}
		while (!ff.eof()) {
			size_t bytes_read=ff.read(ba, 5);
			if (bytes_read != 5) return false;
			buffer=ba.toCharPtr();
			size_t size=ppl7::Peek32(buffer);
			int id=ppl7::Peek8(buffer + 4);
			//printf ("load id=%d, size=%zd\n",id,size);
			if (size <= 5) continue;

			if (id == Level::LevelChunkId::chunkLevelParameter) {
				bytes_read=ff.read(ba, size - 5);
				if (bytes_read != size - 5) return false;
				//ppl7::PrintDebug("   found levelparams [%s]\n", (const char*)filename);

				this->Filename=filename;

				const char* buffer=ba.toCharPtr();
				int version=ppl7::Peek8(buffer);
				if (version != 1) {
					printf("Can't load LevelParameter, unknown version! [%d]\n", version);
					return false;
				}
				ppl7::ByteArrayPtr assoc_ba(buffer + 1, ba.size() - 1);
				ppl7::AssocArray a;
				a.importBinary(assoc_ba);
				loadFromAssocArray(a);
				return true;
			} else {
				ff.seek(size - 5, ppl7::FileObject::SeekOrigin::SEEKCUR);
			}

		}

	} catch (...) {
		return false;
	}
	return false;
}

static void getLevelList(std::list<LevelDescription>& level_list, const ppl7::String& path)
{
	ppl7::Dir dir;
	try {
		dir.open(path, ppl7::Dir::Sort::SORT_FILENAME);
		ppl7::Dir::Iterator it;
		dir.reset(it);
		ppl7::DirEntry entry;
		while (dir.getNextPattern(entry, it, "*.lvl")) {
			//ppl7::PrintDebug("level file: %s\n", (const char*)entry.File);
			LevelDescription descr;
			if (descr.loadFromFile(entry.File)) {
				level_list.push_back(descr);
			}
		}
	} catch (...) {

	}
}

void getLevelList(std::list<LevelDescription>& level_list)
{
	getLevelList(level_list, "level");
	ppl7::String CustomLevelPath=ppl7::Dir::documentsPath(APP_COMPANY, APP_NAME);
	getLevelList(level_list, CustomLevelPath);

	//ppl7::PrintDebug("%d levels found\n", (int)level_list.size());
}


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
	randomSong=true;
	backgroundType=Background::Type::Color;
	BackgroundColor.setColor(32, 32, 64, 255);
	InitialSong.clear();
	SongPlaylist.clear();
	BackgroundImage.clear();
	GlobalLighting.setColor(255, 255, 255, 255);
	InitialItems.clear();
	drainBattery=false;
	batteryDrainRate=1.0f;
	flashlightOnOnLevelStart=false;
	LevelDescription::clear();

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

	if (params.drainBattery) a.set("drainBattery", "true");
	else a.set("drainBattery", "false");
	a.setf("batteryDrainRate", "%0.3f", params.batteryDrainRate);

	if (params.flashlightOnOnLevelStart) a.set("flashlightOnOnLevelStart", "true");
	else a.set("flashlightOnOnLevelStart", "false");

	for (auto it=params.InitialItems.begin();it != params.InitialItems.end();++it) {
		a.setf("InitialItems/[]", "%d", (*it));
	}


	//a.list();

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
	//a.list();

	width=a.getInt("level_width", width);
	height=a.getInt("level_height", height);

	LevelDescription::loadFromAssocArray(a);


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

	Default="false";
	if (a.exists("drainBattery")) drainBattery=a.getString("drainBattery", Default).toBool();
	if (a.exists("flashlightOnOnLevelStart")) flashlightOnOnLevelStart=a.getString("flashlightOnOnLevelStart", Default).toBool();
	Default="1.000";
	if (a.exists("batteryDrainRate")) batteryDrainRate=a.getString("batteryDrainRate", Default).toFloat();
	if (a.exists("InitialItems")) {
		ppl7::AssocArray& itemlist=a.getAssocArray("InitialItems");
		ppl7::AssocArray::const_iterator it;
		for (it=itemlist.begin();it != itemlist.end();++it) {
			InitialItems.insert((*it).second->toString().toInt());
		}
	}
	//a.list("level::parameter::load");
}
