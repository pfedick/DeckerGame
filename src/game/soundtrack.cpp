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
        }
    }
    if (!playing_song) {
        if (params.SongPlaylist.empty() && params.InitialSong.notEmpty()) {
            playing_song=new AudioStream(params.InitialSong, AudioClass::Music);
            audiosystem.play(playing_song);
        } else {
            if (params.randomSong) {
                song_index=ppl7::rand(0, params.SongPlaylist.size());
                playing_song=new AudioStream(params.SongPlaylist[song_index], AudioClass::Music);
                audiosystem.play(playing_song);
            } else {
                playing_song=new AudioStream(params.SongPlaylist[song_index], AudioClass::Music);
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
