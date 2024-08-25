#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"


Soundtrack::Soundtrack(AudioSystem& audio, const LevelParameter& level_params)
    : audiosystem(audio), params(level_params)
{
    playing_song=NULL;
    song_index=0;
}

Soundtrack::~Soundtrack()
{
    if (playing_song) audiosystem.stop(playing_song);
    delete playing_song;
}

void Soundtrack::update()
{
    if (playing_song) {
        if (!audiosystem.isPlaying(playing_song)) {
            delete playing_song;
            playing_song=NULL;
            currentSong.clear();
        }
    }
    if (!playing_song) {
        currentSong.clear();
        if (params.SongPlaylist.empty() && params.InitialSong.notEmpty()) {
            playSong(params.InitialSong);
        } else if (!params.SongPlaylist.empty()) {
            if (params.randomSong) {
                song_index=ppl7::rand(0, params.SongPlaylist.size());
                playSong(params.SongPlaylist[song_index]);
            } else {
                if (song_index >= params.SongPlaylist.size()) song_index=0;
                playSong(params.SongPlaylist[song_index]);
                song_index++;
                if (song_index >= params.SongPlaylist.size())song_index=0;
                audiosystem.play(playing_song);
            }
        }
    }
}

void Soundtrack::playInitialSong()
{
    if (params.InitialSong.notEmpty()) playSong(params.InitialSong);
}

void Soundtrack::playSong(const ppl7::String& filename)
{
    if (playing_song) {
        //if (currentSong == filename) return;
        playing_song->fadeout(4.0f);
        playing_song->setAutoDelete(true);
        playing_song=NULL;
    }
    if (filename.notEmpty()) {
        playing_song=new AudioStream(filename, AudioClass::Music);
        audiosystem.play(playing_song);
    }
    song_index=0;
}


void Soundtrack::fadeout(float seconds)
{
    if (playing_song) {
        playing_song->fadeout(seconds);
        playing_song->setAutoDelete(true);
        playing_song=NULL;
    }
}
