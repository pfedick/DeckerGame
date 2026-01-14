# GitHub Copilot Context für DeckerGame

## Projektübersicht

DeckerGame ist ein Jump'n'Run Game mit einer eigenen 2D-GameEngine, basierend auf SDL. Das Spiel ist eine Fortsetzung eines LEGO-Stopmotion-Films und verwendet ausschließlich digitale LEGO-Bausteine und Minifiguren für alle Grafik-Assets.

## Technologie-Stack

### Core Libraries
- **SDL**: SDL3 (Version 3.2.28+) – Migration abgeschlossen (Januar 2026)
- **ppltk**: Eigene UI-Toolkit-Library (SDL3-basiert), eingebunden als git submodule
- **pplib**: Eigene Library für OS-Abstraktion (String, File-IO, Grafik, Audio), eingebunden als git submodule
- **Sprache**: C++
- **Build-System**: autoconf, gcc/clang
- **Plattformen**: Windows (mingw64/msys), Linux, FreeBSD

### Grafik-Pipeline
- Alle 2D-Grafiken sind mit Lightwave 3D erstellt und vorgerendert
- SDL3 Renderer (Hardware-accelerated) mit VSync
- Geplant: SDL3 GPU-Features mit Shader-Unterstützung für visuelle Effekte

### Audio
- SDL3 Audio-Subsystem mit Float32-Pipeline (F32LE)
- Float-basierte Audio-Engine (32-bit floating point)
- Unterstützt MP3, OGG, WAVE, AIFF (16/24-bit)
- Mono- und Stereo-Dateien werden unterstützt
- AudioDecoder mit Float-Ausgabe für alle Formate

## Projektstruktur

```
/src         - Hauptquellcode des Spiels
/include     - Header-Dateien
/res         - Ressourcen (Texturen, Fonts, etc.)
/level       - Level-Definitionen
/lang        - Sprachdateien (JSON)
/pplib       - Git submodule: pplib Library
/ppltk       - Git submodule: ppltk UI-Toolkit
```

## Abgeschlossene Modernisierungen

### ✅ SDL2 zu SDL3 Migration (Abgeschlossen: Januar 2026)
- Migration von SDL2 auf SDL3 (Version 3.2.28+) vollständig umgesetzt
- Grafik: SDL3 Renderer mit VSync-Unterstützung
- Audio: SDL3 AudioStream mit Float32-Pipeline
- Controller: SDL3 Gamepad-API integriert
- ppltk und pplib auf SDL3 migriert

### ✅ Audio-Engine Modernisierung (Abgeschlossen: Januar 2026)
- Umstellung auf Float-basierte Audio-Berechnung (32-bit float)
- Alle AudioDecoder unterstützen Float-Ausgabe (STEREOSAMPLE_FLOAT)
- Verbesserte Audioqualität durch native Float-Pipeline
- Korrekte 24-Bit-Unterstützung für AIFF/WAVE
- Mono-Dateien werden automatisch auf Stereo dupliziert

### ✅ Shader-Integration: Parallax Blur (Abgeschlossen: Januar 2026)
- Implementierung von Unschärfe-Effekten für Parallax-Ebenen
- Nutzung der SDL 3.4 Shader-Features mit SDL_Renderer
- Verbesserung der visuellen Tiefenwirkung (Depth of Field)

## Zukünftige Modernisierungsziele

### 1. Erweiterung Sprite-Format (Priorität: Mittel)
- Erweiterung der `.tex` Dateien (PFP-Format)
- **NRML Chunk**: Normal-Map Surface
- **SPEC Chunk**: Specular-Map Surface
- Ziel: Bessere Oberflächenbeleuchtung in Kombination mit Shadern

## Entwicklungsphilosophie

**Unterstützungsmodus gewünscht**: Der Entwickler möchte beim Modernisierungsprozess **lernen**. 

### Copilot sollte:
- ✅ Erklärungen und Hinweise zu SDL3 GPU-API geben
- ✅ Kleinere Code-Snippets und Beispiele vorschlagen
- ✅ Auf potenzielle Probleme und Best Practices hinweisen
- ✅ Schrittweise Änderungen mit Erklärungen (nicht alles auf einmal)
- ✅ Shader-Code und GPU-Integration erklären
- ✅ Bei Fragen warten und nicht eigenständig Code ändern
- ✅ **WICHTIG**: Wenn der Entwickler sagt "ich mache X", dann nur unterstützend helfen, aber **NICHT** selbst Code ändern
- ✅ **Vermutungen kennzeichnen**: Wenn eine Lösung nur eine Vermutung ist, dies sofort kommunizieren
- ✅ **Vermutungen prüfen**: Wenn möglich, Vermutungen durch Code-Analyse oder Tool-Verwendung überprüfen, bevor sie vorgeschlagen werden

### Copilot sollte NICHT:
- ❌ Ohne Rückfrage größere Refactorings durchführen
- ❌ Ganze Dateien ohne Nachfrage umschreiben
- ❌ Komplexe Änderungen ohne Erklärung vornehmen
- ❌ Code ändern, wenn der Entwickler sagt "ich mache das"
- ❌ Vermutungen als Fakten präsentieren

## SDL3 Implementierungsdetails

### Grafik (Aktueller Stand)
- `SDL_Renderer` mit Hardware-Beschleunigung
- VSync via `SDL_SetRenderVSync(renderer, 1)`
- Standard-Format: SDL_PIXELFORMAT_ARGB8888
- Funktioniert stabil auf Windows/Linux/FreeBSD

### Audio (Aktueller Stand)
- Format: SDL_AUDIO_F32LE (Float32 Little-Endian)
- Samplerate: 44100 Hz, Stereo
- AudioStream mit Get-Callback für Echtzeit-Mixing
- Float-Pipeline in pplib für alle Decoder (MP3, OGG, WAVE, AIFF)
- Clipping-Detection im Mixer integriert

### Controller (Aktueller Stand)
- SDL3 Gamepad-API vollständig implementiert
- Automatische Erkennung und Rumble-Support
- Konfigurierbare Button-Mappings

### Nächster Schritt: Sprite-Format Erweiterung
- Erweiterung des `.tex` Formats (PFP) um NRML und SPEC Chunks
- Integration von Normal-Maps für Beleuchtungseffekte
- Anpassung der Shader zur Nutzung der neuen Textur-Informationen

## Rendering-Architektur (Wichtig!)

### Sprite-System
- **SpriteTexture-Klasse**: Verwaltet komprimierte `.tex` Dateien (PFP-Format, Version 1)
- **Format**: Chunk-basiert (INDX = Index, SURF = Surface Textur, NRML/SPEC = geplant)
- **Index-Items**: Enthalten Sprite-ID, TextureId, Textur-Koordinaten, Pivot-Punkt, Offset
- **Wichtig**: Pivot (Ursprungspunkt) und Offset (Platzierung) sind kritisch für Animation
- **Buffers**: Unterstützt SDL-Buffer (GPU) und Memory-Buffer (CPU/Collision)

### Rendering-Pipeline
- **Layer-System**: 7 Ebenen (Horizon → Far → Player → Front → Near) mit jeweils 2 Sub-Layer
- **Parallax-Faktor**: Jede Ebene hat eigene Bewegungsgeschwindigkeit (`planeFactor`)
- **Render-Targets**: Texture-basiert für Lightmap-Mixing (additive Blending)
- **Aktuell**: ~10.000 SDL_RenderTexture() Calls/Frame bei 2-3 ms Latenz → SDL_Renderer batched intern
- **Zukunft**: Texture-Atlas bereits vorhanden (wenige große Texturen statt Hunderte) → ideal für GPU-Batching

### Normale Workflow für Sprites pro Frame
```cpp
Level::draw(renderer, worldcoords, player, metrics, glimmer) {
    for (plane in Horizon..Near) {
        drawNonePlayerPlane(renderer, plane, ...);  // Tile + Sprite Layer
        addLightmap(renderer, plane_lightmap);      // Multiplicative lighting
        drawParticles(renderer, layer);
    }
}
```

### Farbmodulation & Lighting
- **Diffuse Lighting**: `.tex` Dateien geladen mit optional Farbmodulation
- **Lichtkarten**: Separate `lightmaps.tex` mit ADD Blending (`SDL_BLENDMODE_ADD`)
- **Z-Koordinate**: Aktuell keine echte Tiefensortierung (Zeichenreihenfolge-abhängig)
- **Geplant**: Normal-Map + Specular-Map für bessere Oberflächenbeleuchtung

## Wichtige Code-Locations

- [SpriteTexture](src/game/spritetexture.cpp#L116): Sprite-Verwaltung und Rendering
- [Level::draw()](src/game/level.cpp#L463): Haupt-Rendering-Loop mit Ebenen-System
- [Level-Format](include/decker.h#L200): SpriteSystem pro Ebene, Plane-Definition
- [Lightmap-Handling](src/game/level.cpp#L419): prepareLayer(), addLightmap() mit Render-Targets
- [Parallax-Faktoren](src/game/level.cpp#L165): spritesystem() für plane/layer Abfragen

## Sprite-Format erweiterbar (Geplant)

Sprite-Dateien erweitern um:
- **NRML Chunk**: Normal-Map Surface (Camera_Tangent_Space_XYZ aus Lightwave)
- **SPEC Chunk**: Specular-Map Surface (Specular Direct + Indirect kombiniert)
- **Gleiche TextureId-Referenz**: Keine Index-Änderungen nötig
- **Implementierung**: User entwickelt Tool zur Datei-Generierung, GPU-API später integriert

## Build & Compile

```bash
./configure
make -j 8              # Parallel build mit 8 Cores
./decker              # Run game
```

**Dependencies**: SDL3, pplib, ppltk (Submodules), libmpg123, libvorbis, libpng, zlib, bzip2

## Lizenz & Rechtliches

- OpenSource-Projekt
- Alle Grafiken basieren auf LEGO-Designs
- Potenzielle zukünftige Änderung: Ersatz LEGO-basierter Assets für Steam-Veröffentlichung

## Zusätzliche Informationen

Weitere Details siehe [README.md](../README.md)

---

**Zuletzt aktualisiert**: 2. Januar 2026
