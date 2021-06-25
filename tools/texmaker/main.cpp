#include "texmaker.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl6.h>

void help()
{
	printf ("PPL TexMaker\n"
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
		"   --pivot_detection  params|bricks\n"

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
	while (Dir.getNextPattern(Entry, it,Pattern)) {
		printf("Found: %s, ", (const char*) (Entry.File));
		if (!Tex.AddFile(Entry.File, id, px, py)) {
			printf("Debug 2\n");
			return 1;
		}
		id++;
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
			ppl7::Array Token(line,",");
			if (Token.size()!=4) {
				throw ppl7::Exception("ERROR: Ungueltige Anzahl Token");
			}
			int id=Token[0].toInt();
			ppl7::String file=basedir+"/"+Token[1];
			int px=Token[2].toInt();
			int py=Token[2].toInt();
			printf("Found: %s, ", (const char*) (file));
			if (!Tex.AddFile(file, id, px, py)) {
				throw ppl7::Exception("Debug 3");
			}
		}
	} catch (...) {

	}
}

int main(int argc, char **argv)
{
	const char *listfile=ppl6::getargv(argc,argv,"-f");
	const char *source=ppl6::getargv(argc,argv,"-s");
	const char *target=ppl6::getargv(argc,argv,"-t");
	const char *savepng=ppl6::getargv(argc,argv,"-x");
	const char *pivot_detection=ppl6::getargv(argc,argv,"--pivot_detection");

	if ((source==NULL && listfile==NULL) || target==NULL || argc<2) {
		help();
		return 0;
	}
	if (source!=NULL && listfile!=NULL) {
		help();
		return 1;
	}

	ppl7::grafix::Grafix Grafix;

	TextureFile Tex;

	const char *tmp;
	if ((tmp=ppl6::getargv(argc,argv,"-a"))) Tex.SetAuthor(tmp);
	if ((tmp=ppl6::getargv(argc,argv,"-c"))) Tex.SetCopyright(tmp);
	if ((tmp=ppl6::getargv(argc,argv,"-n"))) Tex.SetName(tmp);
	if ((tmp=ppl6::getargv(argc,argv,"-d"))) Tex.SetDescription(tmp);

	if (pivot_detection) {
		printf ("test: %s\n", pivot_detection);
		if (strcmp(pivot_detection,"params")==0) Tex.SetPivotDetection(PIVOT_PARAMS);
		else if (strcmp(pivot_detection,"bricks")==0) Tex.SetPivotDetection(PIVOT_BRICKS);
		else {
			printf ("ERROR: unknwon pivot_detection algorithm [%s]\n", pivot_detection);
			return (1);
		}
	}

	int t,w,h,px,py;
	t=ppl6::atoi(ppl6::getargv(argc,argv,"-mt"));
	w=ppl6::atoi(ppl6::getargv(argc,argv,"-w"));
	h=ppl6::atoi(ppl6::getargv(argc,argv,"-h"));
	px=ppl6::atoi(ppl6::getargv(argc,argv,"-px"));
	py=ppl6::atoi(ppl6::getargv(argc,argv,"-py"));

	Tex.SetMaxTextureNum(t);
	if (w>0 && h==0) h=w;
	if (h>0 && w==0) w=h;
	if (w>0) Tex.SetTextureSize(w,h);

	if (source) {
		int ret=loadFromDirectory(source, px, py, Tex);
		if (ret!=0) return ret;
	} else if (listfile) {
		try {
			loadFromListfile(listfile, Tex);
		} catch (const ppl7::Exception &ex) {
			ex.print();
			return 1;
		}

	}

	if (savepng) {
		printf("Speichere Texturen als PNG...\n");
		Tex.SaveTextures(savepng);
	}
	try {
		Tex.Save(target);
	} catch (const ppl7::Exception &ex) {
		ex.print();
		return 1;
	}
	return 0;
}
