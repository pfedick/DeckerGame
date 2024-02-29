/*
 * Copyright © 2018, VideoLAN and dav1d authors
 * Copyright © 2018, Two Orioles, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "videoplayer.h"

/*
static const uint8_t probe_data[] ={
	'D', 'K', 'I', 'F',
	0, 0, 0x20, 0,
	'A', 'V', '0', '1',
};


static int ivf_probe(const uint8_t* const data) {
	return !memcmp(data, probe_data, sizeof(probe_data));
}
*/

static unsigned rl16(const uint8_t* const p) {
	return ((uint32_t)p[1] << 8U) | p[0];
}


static unsigned rl32(const uint8_t* const p) {
	return ((uint32_t)p[3] << 24U) | (p[2] << 16U) | (p[1] << 8U) | p[0];
}

static int64_t rl64(const uint8_t* const p) {
	return (((uint64_t)rl32(&p[4])) << 32) | rl32(p);
}


IVFDemuxer::IVFDemuxer()
{
	broken=0;
	timebase=0.0f;
	last_ts=0;
	step=0;
}



bool IVFDemuxer::open(const ppl7::String& filename,
	unsigned fps[2], unsigned* const num_frames, unsigned timebase[2])
{
	uint8_t hdr[32];
	file.open(filename, ppl7::File::READ);
	if (file.read(hdr, 32) != 32) {
		ppl7::PrintDebugTime("Failed to read stream header\n");
		return false;
	}


	if (memcmp(hdr, "DKIF", 4)) {
		fprintf(stderr, "%s is not an IVF file [tag=%.4s|0x%02x%02x%02x%02x]\n",
			(const char*)filename, hdr, hdr[0], hdr[1], hdr[2], hdr[3]);
		return false;
	} else if (memcmp(&hdr[8], "AV01", 4)) {
		fprintf(stderr, "%s is not an AV1 file [tag=%.4s|0x%02x%02x%02x%02x]\n",
			(const char*)filename, &hdr[8], hdr[8], hdr[9], hdr[10], hdr[11]);
		return false;
	}
	video_width=rl16(&hdr[12]);
	video_height=rl16(&hdr[14]);

	timebase[0] = rl32(&hdr[16]);
	timebase[1] = rl32(&hdr[20]);
	const unsigned duration = rl32(&hdr[24]);


	uint8_t data[8];
	broken = 0;
	try {
		for (*num_frames = 0;; (*num_frames)++) {
			if (file.fread(data, 4, 1) != 1) break; // EOF
			size_t sz = rl32(data);
			if (file.fread(data, 8, 1) != 1) break; // EOF
			const uint64_t ts = rl64(data);
			if (*num_frames && ts <= last_ts)
				broken = 1;
			last_ts = ts;
			file.seek(sz, ppl7::File::SEEKCUR);
		}
	} catch (const ppl7::EndOfFileException&) {}

	uint64_t fps_num = (uint64_t)timebase[0] * *num_frames;
	uint64_t fps_den = (uint64_t)timebase[1] * duration;
	if (fps_num && fps_den) { /* Reduce fraction */
		uint64_t gcd = fps_num;
		for (uint64_t a = fps_den, b; (b = a % gcd); a = gcd, gcd = b);
		fps_num /= gcd;
		fps_den /= gcd;

		while ((fps_num | fps_den) > UINT_MAX) {
			fps_num >>= 1;
			fps_den >>= 1;
		}
	}
	if (fps_num && fps_den) {
		fps[0] = (unsigned)fps_num;
		fps[1] = (unsigned)fps_den;
	} else {
		fps[0] = fps[1] = 0;
	}
	this->timebase = (double)timebase[0] / timebase[1];
	step = duration / *num_frames;

	file.seek(32, ppl7::File::SEEKSET);
	last_ts = 0;

	return true;
}

int IVFDemuxer::read_header(ptrdiff_t* const sz,
	int64_t* const off_, uint64_t* const ts)
{
	uint8_t data[8];
	int64_t const off = file.tell();
	if (off_) *off_ = off;
	if (file.fread(data, 4, 1) != 1) return -1; // EOF
	*sz = rl32(data);
	if (!broken) {
		if (file.fread(data, 8, 1) != 1) return -1;
		*ts = rl64(data);
	} else {
		if (file.seek(8, ppl7::File::SEEKCUR)) return -1;
		*ts = off > 32 ? last_ts + step : 0;
	}
	return 0;
}

int IVFDemuxer::read(Dav1dData* const buf) {
	uint8_t* ptr;
	ptrdiff_t sz;
	int64_t off;
	uint64_t ts;
	try {
		if (read_header(&sz, &off, &ts)) return -1;
		//ppl7::PrintDebug("buf->sz=%zd, new sz=%zd, data=%x\n", buf->sz, sz, buf->data);

		if (!(ptr = dav1d_data_create(buf, sz))) return -1;
		if (file.fread(ptr, sz, 1) != 1) {
			ppl7::PrintDebugTime("Failed to read frame data: %s\n", strerror(errno));
			dav1d_data_unref(buf);
			return -1;
		}
		buf->m.offset = off;
		buf->m.timestamp = ts;
		last_ts = ts;
		return 0;
	} catch (const ppl7::EndOfFileException&) {
		dav1d_data_unref(buf);
		return -1;
	}
}

int IVFDemuxer::seek(const uint64_t pts) {
	uint64_t cur;
	const uint64_t ts = llround((pts * timebase) / 1000000000.0);
	if (ts <= last_ts)
		if (file.seek(32, ppl7::File::SEEKSET)) goto error;
	while (1) {
		ptrdiff_t sz;
		if (read_header(&sz, NULL, &cur)) goto error;
		if (cur >= ts) break;
		if (file.seek(sz, ppl7::File::SEEKCUR)) goto error;
		last_ts = cur;
	}
	if (file.seek(-12, ppl7::File::SEEKCUR)) goto error;
	return 0;
error:
	ppl7::PrintDebugTime("Failed to seek: %s\n", strerror(errno));
	return -1;
}

void IVFDemuxer::close() {
	file.close();
	broken=0;
	timebase=0.0f;
	last_ts=0;
	step=0;

}

int IVFDemuxer::width() const
{
	return video_width;
}
int IVFDemuxer::height() const
{
	return video_height;
}
