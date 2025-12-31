# GitHub Copilot Context für DeckerGame

## Projektübersicht

DeckerGame ist ein Jump'n'Run Game mit einer eigenen 2D-GameEngine, basierend auf SDL. Das Spiel ist eine Fortsetzung eines LEGO-Stopmotion-Films und verwendet ausschließlich digitale LEGO-Bausteine und Minifiguren für alle Grafik-Assets.

## Technologie-Stack

### Core Libraries
- **SDL**: Aktuell SDL2, Migration zu SDL3 (Version 3.2.28+) geplant
- **ppltk**: Eigene UI-Toolkit-Library (SDL2-basiert), eingebunden als git submodule
- **pplib**: Eigene Library für OS-Abstraktion (String, File-IO, Grafik), eingebunden als git submodule
- **Sprache**: C++
- **Build-System**: autoconf, gcc/clang
- **Plattformen**: Windows (mingw64/msys), Linux, FreeBSD

### Grafik-Pipeline
- Alle 2D-Grafiken sind mit Lightwave 3D erstellt und vorgerendert
- Aktuell: SDL_Renderer (Software/Hardware-Rendering)
- Geplant: SDL3 GPU-Features mit Shader-Unterstützung

### Audio
- SDL2 Audio-Subsystem
- Aktuell: Integer-basierte Audio-Berechnung
- Optional geplant: Float-basierte Audio-Engine

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

## Modernisierungsziele

### 1. SDL2 zu SDL3 Migration (Priorität: Hoch)
- Migration von SDL2 auf SDL3 (Version 3.2.28 oder höher)
- Betrifft: Grafik, Audio, Controller-Unterstützung
- **Wichtig**: API-Änderungen beachten (viele Funktionen umbenannt/umstrukturiert)

### 2. GPU-Rendering (Priorität: Hoch)
- Umstellung von SDL_Renderer auf SDL3 GPU-Features
- Ermöglicht moderne Rendering-Techniken
- Basis für Shader-Integration

### 3. Shader-Integration (Priorität: Mittel)
- Verwendung von Shadern für visuelle Effekte
- Speziell: Unschärfe-Effekte für Parallax-Ebenen (Depth of Field)
- Verbesserung der visuellen Tiefenwirkung

### 4. Audio-Engine Modernisierung (Priorität: Niedrig/Optional)
- Umstellung von Integer auf Float-basierte Audio-Berechnung
- Verbesserte Audio-Qualität und moderne Standards

## Entwicklungsphilosophie

**Unterstützungsmodus gewünscht**: Der Entwickler möchte beim Modernisierungsprozess **lernen**. 

### Copilot sollte:
- ✅ Erklärungen und Hinweise zu SDL3-API-Änderungen geben
- ✅ Kleinere Code-Snippets und Beispiele vorschlagen
- ✅ Auf potenzielle Probleme und Best Practices hinweisen
- ✅ Schrittweise Anleitungen für komplexe Änderungen geben
- ✅ Shader-Code und GPU-Integration erklären

### Copilot sollte NICHT:
- ❌ Große Codeblöcke automatisch ersetzen ohne Erklärung
- ❌ Vollständige Datei-Rewrites durchführen
- ❌ Eigenständige umfangreiche Refactorings vornehmen
- ❌ Änderungen ohne Kontext und Lernmöglichkeit einbauen

## SDL2 → SDL3 Migrationshinweise

### Wichtige API-Änderungen (SDL3)
- `SDL_Window` und `SDL_Renderer` API stark überarbeitet
- Viele Funktionen von `SDL_*` zu `SDL3_*` umbenannt
- Event-System modernisiert
- Audio-System komplett neu designt
- Controller-API vereinfacht

### GPU-Rendering in SDL3
- Neue `SDL_GPU*` API für modernes Rendering
- Unterstützt Vulkan, Metal, D3D12
- Shader-basierter Rendering-Pipeline
- Bessere Performance und Flexibilität

## Lizenz & Rechtliches

- OpenSource-Projekt
- Alle Grafiken basieren auf LEGO-Designs
- Potenzielle zukünftige Änderung: Ersatz LEGO-basierter Assets für Steam-Veröffentlichung

## Zusätzliche Informationen

Weitere Details siehe [README.md](../README.md)

---

**Zuletzt aktualisiert**: 31. Dezember 2025
