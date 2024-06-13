#ifndef INCLUDE_VIDEOPLAYER_H_
#define INCLUDE_VIDEOPLAYER_H_

#include <SDL.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>
#include "decker_sdl.h"

#include "audio.h"
extern "C" {
#include <dav1d/dav1d.h>
#include <dav1d/data.h>
}

/**
 * Settings structure
 * Hold all settings available for the player,
 * this is usually filled by parsing arguments
 * from the console.
 */
/*
typedef struct {
    const char* inputfile;
    const char* renderer_name;
    int highquality;
    int untimed;
    int zerocopy;
    int gpugrain;
} Dav1dPlaySettings;
*/
// FIFO structure
struct dp_fifo
{
    SDL_mutex* lock;
    SDL_cond* cond_change;
    size_t capacity;
    size_t count;
    void** entries;
    int push_wait;
    int flush;
};

/*
 * Dav1dPlay FIFO helper
 */

typedef struct dp_fifo Dav1dPlayPtrFifo;

/* Create a FIFO
 *
 * Creates a FIFO with the given capacity.
 * If the capacity is reached, new inserts into the FIFO
 * will block until enough space is available again.
 */
Dav1dPlayPtrFifo* dp_fifo_create(size_t capacity);

/* Destroy a FIFO
 *
 * The FIFO must be empty before it is destroyed!
 */
void dp_fifo_destroy(Dav1dPlayPtrFifo* fifo);

/* Shift FIFO
 *
 * Return the first item from the FIFO, thereby removing it from
 * the FIFO and making room for new entries.
 */
void* dp_fifo_shift(Dav1dPlayPtrFifo* fifo);

/* Push to FIFO
 *
 * Add an item to the end of the FIFO.
 * If the FIFO is full, this call will block until there is again enough
 * space in the FIFO, so calling this from the "consumer" thread if no
 * other thread will call dp_fifo_shift will lead to a deadlock.
 */
void dp_fifo_push(Dav1dPlayPtrFifo* fifo, void* element);

void dp_fifo_flush(Dav1dPlayPtrFifo* fifo, void (*destroy_elem)(void*));


/**
 * Render context structure
 * This structure contains informations necessary
 * to be shared between the decoder and the renderer
 * threads.
 */
typedef struct render_context
{
    //Dav1dPlaySettings settings;
    Dav1dSettings lib_settings;

    // Renderer private data (passed to callbacks)
    void* rd_priv;

    // Lock to protect access to the context structure
    SDL_mutex* lock;

    // Timestamp of last displayed frame (in timebase unit)
    int64_t last_ts;
    // Timestamp of last decoded frame (in timebase unit)
    int64_t current_ts;
    // Ticks when last frame was received
    uint32_t last_ticks;
    // PTS time base
    double timebase;
    // Seconds per frame
    double spf;
    // Number of frames
    uint32_t total;

    // Fifo
    Dav1dPlayPtrFifo* fifo;

    // Custom SDL2 event types
    uint32_t event_types;

    // User pause state
    uint8_t user_paused;
    // Internal pause state
    uint8_t paused;
    // Start of internal pause state
    uint32_t pause_start;
    // Duration of internal pause state
    uint32_t pause_time;

    // Seek accumulator
    int seek;

    // Indicates if termination of the decoder thread was requested
    uint8_t dec_should_terminate;
} Dav1dPlayRenderContext;

class IVFDemuxer
{
private:
    ppl7::File file;
    int broken;
    double timebase;
    uint64_t last_ts;
    uint64_t step;
    int video_width;
    int video_height;

    int read_header(ptrdiff_t* const sz, int64_t* const off_, uint64_t* const ts);

public:
    IVFDemuxer();
    bool open(const ppl7::String& filename, unsigned fps[2], unsigned* const num_frames, unsigned timebase[2]);
    int read(Dav1dData* const buf);
    int seek(const uint64_t pts);
    void close();
    int width() const;
    int height() const;

};


class VideoPlayer
{
private:
    SDL_Renderer* renderer;
    SDL_Texture* overlay;
    IVFDemuxer demuxer;
    unsigned int fps[2], num_frames, timebase[2];
    Dav1dSettings lib_settings;
    //Dav1dPicture* p;
    Dav1dContext* context = NULL;
    Dav1dData data;

    bool endreached;
    float video_framerate;
    int frame_width, frame_height;

    int sdl_update_texture(Dav1dPicture* dav1d_pic);
    int decode_frame(Dav1dPicture** p);


public:
    VideoPlayer(SDL_Renderer* renderer=NULL);
    ~VideoPlayer();
    void setRenderer(SDL_Renderer* renderer);
    void clear();
    bool load(const ppl7::String& filename);
    bool nextFrame();
    int width() const;
    int height() const;
    float framerate() const;
    bool eof() const;

    void renderFrame(const SDL_Rect* dstrect=NULL);
    SDL_Texture* getVideoTexture();

};






#endif
