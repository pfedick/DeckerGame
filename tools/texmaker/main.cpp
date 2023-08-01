#include "texmaker.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>

void help()
{
	printf("PPL TexMaker\n"
		"\n"
		"Optionen:\n"
		"   -s DIR  Quellverzeichnis\n"
		"   -t FILE Zieldatei\n"
		"   -f FILE Datei mit Sprite-Liste\n"
		"   -w #    Breite einer Textur (Default=256)\n"
		"   -h #    Hohe einer Textur (Default=Breite)\n"
		"   -px #   Pivot-Punkt für alle Texturen, x-Koordinate (Default=0)\n"
		"   -py #   Pivot-Punkt für alle Texturen, y-Koordinate (Default=0)\n"
		"   -mt #   Maximale Anzahl Texturen\n"
		"   -a TEXT Name des Authors\n"
		"   -c TEXT Copyright-String\n"
		"   -n TEXT Name der Datei\n"
		"   -d TEXT Description\n"
		"   -x FILE Speichert jede fertige Textur als PNG\n"
		"   --pivot_detection  params|bricks|lower_middle\n"
		"   --help  zeigt diese Hilfe an\n"

	);
}

int loadFromDirectory(const char* source, int px, int py, TextureFile& Tex) {
	int id = 0;
	ppl7::Dir Dir;
	ppl7::String Path, Pattern;
	Path = source;
	if (Path.instr("*") < 0) {
		Path = ppl7::File::getPath(Path);
		Pattern = "*.png";
	} else {
		Pattern = ppl7::File::getFilename(Path);
		Path = ppl7::File::getPath(Path);
	}
	Dir.open(Path, ppl7::Dir::SORT_FILENAME);
	ppl7::Dir::Iterator it;
	Dir.reset(it);
	ppl7::DirEntry Entry;
	while (Dir.getNextPattern(Entry, it, Pattern)) {
		//printf("Found: %s, ", (const char*)(Entry.File));
		if (!Tex.AddFile(Entry.File, id, px, py)) {
			//printf("Debug 2\n");
			return 1;
		}
		id++;
	}
	return 0;
}

int loadFromFile(const ppl7::String& source, int px, int py, TextureFile& Tex, int sx, int sy) {
	int id = 0;
	if (sx <= 0 || sy <= 0) {
		printf("ERROR: parameters -sx and -sy are required!\n");
		return 1;
	}
	ppl7::grafix::Image img;
	ppl7::grafix::Point p;
	ppl7::grafix::Size s(sx, sy);

	img.load(source);
	for (p.y=0;p.y < img.height();p.y+=sy) {
		for (p.x=0;p.x < img.width();p.x+=sx) {
			ppl7::grafix::Drawable d=img.getDrawable(p, s);
			if (!Tex.AddSurface(d, NULL, id, px, py)) {
				printf("Could not add surface to Texture\n");
				return 1;
			}
			id++;
		}

	}
	return 0;
}


void loadFromListfile(const char* listfile, TextureFile& Tex)
{

	ppl7::File ff(listfile);
	ppl7::String basedir=ppl7::File::getPath(listfile);
	try {
		while (!ff.eof()) {
			ppl7::String line=ff.gets(1024);
			ppl7::Array Token(line, ",");
			if (Token.size() != 4) {
				throw ppl7::Exception("ERROR: Ungueltige Anzahl Token");
			}
			int id=Token[0].toInt();
			ppl7::String file=basedir + "/" + Token[1];
			int px=Token[2].toInt();
			int py=Token[2].toInt();
			//printf("Found: %s, ", (const char*)(file));
			if (!Tex.AddFile(file, id, px, py)) {
				throw ppl7::Exception("Debug 3");
			}
		}
	} catch (...) {

	}
}

int main(int argc, char** argv)
{
	ppl7::String listfile=ppl7::GetArgv(argc, argv, "-f");
	ppl7::String source=ppl7::GetArgv(argc, argv, "-s");
	ppl7::String target=ppl7::GetArgv(argc, argv, "-t");
	ppl7::String savepng=ppl7::GetArgv(argc, argv, "-x");
	ppl7::String pivot_detection=ppl7::GetArgv(argc, argv, "--pivot_detection");

	if (ppl7::HaveArgv(argc, argv, "--help") || (source.isEmpty() && listfile.isEmpty()) || target.isEmpty() || argc < 2) {
		help();
		return 0;
	}
	if (source.notEmpty() && listfile.notEmpty()) {
		help();
		return 1;
	}

	ppl7::grafix::Grafix Grafix;

	TextureFile Tex;

	ppl7::String Tmp;
	Tmp=ppl7::GetArgv(argc, argv, "-a"); if (Tmp.notEmpty()) Tex.SetAuthor(Tmp);
	Tmp=ppl7::GetArgv(argc, argv, "-c"); if (Tmp.notEmpty()) Tex.SetCopyright(Tmp);
	Tmp=ppl7::GetArgv(argc, argv, "-n"); if (Tmp.notEmpty()) Tex.SetName(Tmp);
	Tmp=ppl7::GetArgv(argc, argv, "-d"); if (Tmp.notEmpty()) Tex.SetDescription(Tmp);

	if (pivot_detection.notEmpty()) {
		printf("test: %s\n", (const char*)pivot_detection);
		if (pivot_detection == "params") Tex.SetPivotDetection(PIVOT_PARAMS);
		else if (pivot_detection == "bricks") Tex.SetPivotDetection(PIVOT_BRICKS);
		else if (pivot_detection == "lower_middle") Tex.SetPivotDetection(PIVOT_LOWER_MIDDLE);
		else {
			printf("ERROR: unknwon pivot_detection algorithm [%s]\n", (const char*)pivot_detection);
			return (1);
		}
	}

	int t, w, h, px, py, sx, sy;
	t=ppl7::GetArgv(argc, argv, "-mt").toInt();
	w=ppl7::GetArgv(argc, argv, "-w").toInt();
	h=ppl7::GetArgv(argc, argv, "-h").toInt();
	px=ppl7::GetArgv(argc, argv, "-px").toInt();
	py=ppl7::GetArgv(argc, argv, "-py").toInt();
	sx=ppl7::GetArgv(argc, argv, "-sx").toInt();
	sy=ppl7::GetArgv(argc, argv, "-sy").toInt();

	Tex.SetMaxTextureNum(t);
	if (w > 0 && h == 0) h=w;
	if (h > 0 && w == 0) w=h;
	if (w > 0) Tex.SetTextureSize(w, h);

	if (source.notEmpty()) {
		if (ppl7::File::exists(source)) {
			printf("source=%s\n", (const char*)source);
			ppl7::DirEntry d=ppl7::File::statFile(source);
			if (d.isFile()) {
				int ret=loadFromFile(source, px, py, Tex, sx, sy);
				if (ret != 0) return ret;
			} else {
				printf("ERROR: this is not a file [%s]\n", (const char*)source);
				return 1;
			}
		} else {
			int ret=loadFromDirectory(source, px, py, Tex);
			if (ret != 0) return ret;
		}
	} else if (listfile.notEmpty()) {
		try {
			loadFromListfile(listfile, Tex);
		} catch (const ppl7::Exception& ex) {
			ex.print();
			return 1;
		}

	}

	try {
		Tex.Save(target);
	} catch (const ppl7::Exception& ex) {
		ex.print();
		return 1;
	}
	if (savepng.notEmpty()) {
		printf("saving texturen...\n");
		Tex.SaveTextures(savepng);
	}
	return 0;
}
