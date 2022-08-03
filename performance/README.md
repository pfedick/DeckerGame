# Try to increase Particle Performance

## Original Version

we use the plus and multiplay operations of the ppl7::PointF object
```c++
velocity+=(gravity * weight * frame_rate_compensation);
p+=(velocity * frame_rate_compensation);
```

```
==================== METRICS DUMP ==============================
Total Frames collected: 3657
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15067, visible Particles:  14123

Timer total:  4.480 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.437
    update sprites:        0.001
    update objects:        0.252
    update particles:      0.813
    draw background:       0.002
    draw tiles:            0.156
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.206
  particle update thread:  6.036
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15055, visible Particles:  14116

Timer total:  4.534 ms, Time Frame: 16.679
  draw userinterface:      0.033
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.490
    update sprites:        0.001
    update objects:        0.248
    update particles:      0.830
    draw background:       0.002
    draw tiles:            0.155
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.247
  particle update thread:  6.098

```
## unroll the multiplication and addition oparations:
```c++
velocity.x+=gravity.x * weight * frame_rate_compensation;
velocity.y+=gravity.y * weight * frame_rate_compensation;
p.x+=velocity.x * frame_rate_compensation;
p.y+=velocity.y * frame_rate_compensation;
```


```
==================== METRICS DUMP ==============================
Total Frames collected: 3657
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14863, visible Particles:  13939

Timer total:  3.911 ms, Time Frame: 16.679
  draw userinterface:      0.033
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.867
    update sprites:        0.001
    update objects:        0.232
    update particles:      0.564
    draw background:       0.001
    draw tiles:            0.154
    draw sprites:          0.001
    draw objects:          0.002
    draw particles:        2.909
  particle update thread:  5.256
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15074, visible Particles:  14134

Timer total:  4.003 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.960
    update sprites:        0.001
    update objects:        0.233
    update particles:      0.597
    draw background:       0.001
    draw tiles:            0.154
    draw sprites:          0.001
    draw objects:          0.002
    draw particles:        2.968
  particle update thread:  5.412
```


## unroll operations if particle->update()
We don't call particle->update() and do all calculations in the main loop:
```

```

```
==================== METRICS DUMP ==============================
Total Frames collected: 3657
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15175, visible Particles:  14222

Timer total:  4.604 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.561
    update sprites:        0.001
    update objects:        0.259
    update particles:      0.812
    draw background:       0.001
    draw tiles:            0.157
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.324
  particle update thread:  5.856
==================== METRICS DUMP ==============================
Total Frames collected: 3595
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14967, visible Particles:  14036

Timer total:  4.548 ms, Time Frame: 16.688
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.506
    update sprites:        0.001
    update objects:        0.251
    update particles:      0.796
    draw background:       0.001
    draw tiles:            0.157
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.292
  particle update thread:  5.743
```
Hmm, that's a surprise, it's a bit slower.

## what does color calculation cost?
lets disable it...
```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15020, visible Particles:  14083

Timer total:  3.813 ms, Time Frame: 16.679
  draw userinterface:      0.031
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.771
    update sprites:        0.001
    update objects:        0.258
    update particles:      0.638
    draw background:       0.001
    draw tiles:            0.153
    draw sprites:          0.001
    draw objects:          0.002
    draw particles:        2.714
  particle update thread:  4.417
```
That bring's us a milliscond! Let's see, if we can sppedup things here...

### Baseline
we inlined the multiply operations
```
==================== METRICS DUMP ==============================
Total Frames collected: 3654
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14979, visible Particles:  14045

Timer total:  4.054 ms, Time Frame: 16.693
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.011
    update sprites:        0.001
    update objects:        0.240
    update particles:      0.556
    draw background:       0.002
    draw tiles:            0.154
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.052
  particle update thread:  5.380
```

### get rid of clamp
The multiplay factor cannot get lower than zero or higher than 1. So the multiplication result could never get smaller than 0 or bigger than 255. We don't need clamp. Get rid of it...

```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15018, visible Particles:  14077

Timer total:  3.985 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.942
    update sprites:        0.001
    update objects:        0.231
    update particles:      0.600
    draw background:       0.002
    draw tiles:            0.155
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        2.946
  particle update thread:  5.458
```
no gains

### compile with -O3?
```
==================== METRICS DUMP ==============================
Total Frames collected: 3655
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14963, visible Particles:  14033

Timer total:  4.011 ms, Time Frame: 16.688
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.968
    update sprites:        0.001
    update objects:        0.241
    update particles:      0.559
    draw background:       0.001
    draw tiles:            0.143
    draw sprites:          0.001
    draw objects:          0.002
    draw particles:        3.014
```
No difference here

### Bug in SpriteTexture::drawScaled

Method was defined as
```c++
void drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor, ppl7::grafix::Color color_modulation) const;
```
So we accidently copied the color instead of giving a reference! Let's fix that
and change it to a const reference.
```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14987, visible Particles:  14051

Timer total:  3.972 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          3.929
    update sprites:        0.001
    update objects:        0.229
    update particles:      0.574
    draw background:       0.001
    draw tiles:            0.154
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        2.962    <======
  particle update thread:  5.393
```
It seems this bug didn't had a big performance impact.

### penality of calling SDL_SetTextureAlphaMod and SDL_SetTextureColorMod

```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15038, visible Particles:  14098

Timer total:  4.044 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.002
    update sprites:        0.001
    update objects:        0.254
    update particles:      0.763
    draw background:       0.002
    draw tiles:            0.158
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        2.818
  particle update thread:  5.684
```
about 0.200 ms. That's a little bit and we need this functions.

### back to the multiplyWithAlpha method. let's try to unroll it

```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15017, visible Particles:  14080

Timer total:  4.565 ms, Time Frame: 16.679
  draw userinterface:      0.033
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.523
    update sprites:        0.001
    update objects:        0.250
    update particles:      0.792
    draw background:       0.001
    draw tiles:            0.156
    draw sprites:          0.001
    draw objects:          0.002
    draw particles:        3.315
  particle update thread:  5.667

```
That doesn't change anything, again. I guess the compiler already did this job.

Let's go back to the original multiplyWithAlpha from pplib

## remove check if particle is visible
let's do so, as if every particle is visible and remove the check for this

```
==================== METRICS DUMP ==============================
Total Frames collected: 3597
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15093, visible Particles:  14199

Timer total:  4.288 ms, Time Frame: 16.679
  draw userinterface:      0.032
  handle events:           0.010
  misc:                    0.000
  draw the world:          4.246
    update sprites:        0.001
    update objects:        0.241
    update particles:      0.685
    draw background:       0.001
    draw tiles:            0.159
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.152
  particle update thread:  5.326
```
This would bring a bit. But most of the particles in this scene are visible. So this is not realy a good idea.


## replace map with list
we insert all visible particles into a map, which is ordered by drawing coordinates. But
does drawing order is relevant for particles? We could dramatically reduce cpu time when
we replace the map by a simple list:

### timings with map
```
==================== METRICS DUMP ==============================
Total Frames collected: 3656
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  15028, visible Particles:  14090

Timer total:  4.141 ms, Time Frame: 16.679
  draw userinterface:      0.043
  handle events:           0.019
  misc:                    0.000
  draw the world:          4.078
    update sprites:        0.001
    update objects:        0.242
    update particles:      0.660
    draw background:       0.002
    draw tiles:            0.157
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        3.009
  particle update thread:  5.545
```
### timings with list
```
==================== METRICS DUMP ==============================
Total Frames collected: 3653
fps: 59
Total Sprites:        0, visible sprites:        0
Total Objects:        6, visible Objects:        6
Total Particles:  14915, visible Particles:  14864

Timer total:  4.328 ms, Time Frame: 16.692
  draw userinterface:      0.035
  handle events:           0.010
  misc:                    0.001
  draw the world:          4.283
    update sprites:        0.001
    update objects:        0.229
    update particles:      0.954
    draw background:       0.002
    draw tiles:            0.161
    draw sprites:          0.001
    draw objects:          0.003
    draw particles:        2.929
  particle update thread:  3.522
```
But Flames look much better when we keep the drawing order
