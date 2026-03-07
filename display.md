# Display Drehwurm - Pflichtenheft

## Projekt Übersicht

ESPHome LVGL Display-Konfiguration für einen "Supercooler Drehwurm Kühler".

`lvgl_basis.yaml` wird in ESPHome via `packages:` importiert.  
**Nur generieren:** `font:`, `globals:`, `interval:`, `lvgl:`  
**Nicht generieren:** sonstiger ESPHome-Code (sensors, lights, etc.)

---

## Hardware

| Parameter | Wert |
|---|---|
| Plattform | ESP32-P4 |
| PSRAM | 32 MB |
| Display | 1024 × 600 px |
| LVGL Version | 8.x (kein LVGL 9!) |

---

## Fonts

| ID | Datei | Größe | Verwendung |
|---|---|---|---|
| `font_title` | Roboto 700 (gfonts) | 60 | Seitentitel |
| `font_normal` | Roboto 400 (gfonts) | 28 | Labels, Slot-Text |
| `font_tab` | Roboto 400 (gfonts) | 30 | Tab-Beschriftungen |
| `font_icons` | Font Awesome Solid 6.5.0 (CDN) | 40 | Play/Pause-Icons (U+F04B, U+F04C) |

Icon-Bytes in Lambdas: Play = `\xef\x81\x8b`, Pause = `\xef\x81\x8c`

---

## Globale Variablen

Für 6 Timer-Slots (Index 0–5):

| ID | Typ | Bedeutung |
|---|---|---|
| `slot_start_ms` | `std::array<uint32_t, 6>` | millis() beim letzten Start |
| `slot_elapsed_ms` | `std::array<uint32_t, 6>` | akkumulierte Zeit in ms |
| `slot_status` | `std::array<int, 6>` | 0=gestoppt, 1=läuft, 2=pausiert |

**Interval:** 500 ms → aktualisiert alle 6 Timer-Labels wenn Status = 1.

---

## Page 1: Hauptseite (`page_main`)

### Titel
- Text: "Supercooler Drehwurm Kühler"
- Font: `font_title`, Farbe: `#003366`, oben mittig, y=20

### Tank-Widget (Platzhalter)
- **TODO: durch echtes PNG-Bild ersetzen**
- Position: zentriert, y=20, 360×360 px
- Aufbau aus LVGL `obj`-Widgets (Zylinder-Illusion):
  - `cyl_rim`: oben, grau, oval (Öffnung)
  - `cyl_body`: Mantelrechteck, grau
  - `water_surface`: hellblaue Ellipse (Wasseroberfläche)
  - `water_body`: blaues Rechteck (Wasserkörper)
  - `cyl_bottom`: dunkelgraue Ellipse (Boden)

### 6 Farbslots (Timer-Buttons)

Anordnung im Uhrzeigersinn nach Farbrad:

| Slot | Farbe | Seite | y-Position | Index |
|---|---|---|---|---|
| 1 | Rot `#FF0000` | links | 110 | 0 |
| 2 | Gelb `#FFFF00` | links | 250 | 1 |
| 3 | Grün `#00FF00` | links | 390 | 2 |
| 4 | Cyan `#00FFFF` | rechts | 110 | 3 |
| 5 | Blau `#0000FF` | rechts | 250 | 4 |
| 6 | Magenta `#FF00FF` | rechts | 390 | 5 |

**Jeder Slot ist ein `button`** (290×120 px, radius=16, clip_corner=true, bg_opa=TRANSP):

- **Linker Tab** (50×100%, `clickable: false`): volle Slot-Farbe, Slot-Nummer zentriert
- **Rechter Bereich** (240×100%, `clickable: false`): 50% Opacity, enthält:
  - Label "Timer:" oben links
  - Zeitanzeige `MM:SS` darunter
  - Play/Pause-Icon rechtsbündig (x=-17)

**Touch-Verhalten:**
- `on_short_click`: Start (Status 0→1) / Pause (Status 1→2) Toggle
- `on_long_press`: Reset — nur wenn Status=2 (pausiert) → Status=0, Anzeige=00:00

> `on_short_click` feuert bei Long Press **nicht** → kein Extra-Flag nötig

**Textfarben** (auf 50%-Hintergrund):
| Slot | Textfarbe |
|---|---|
| 1 R | `#880000` |
| 2 Y | `#666600` |
| 3 G | `#004400` |
| 4 C | `#004444` |
| 5 B | `#FFFFFF` |
| 6 M | `#880044` |

### Navigation
- Button "Einstellungen" unten rechts → `lvgl.page.show: page_settings`

---

## Page 2: Einstellungen (`page_settings`)

### Titel
- Text: "Einstellungen", Font: `font_title`, Farbe: `#003366`, oben mittig

### TabView (3 Tabs, `font_tab`, Hintergrund `#E0E0E0`)

**Tab "Bildschirm":**
- Zeile: Label "Helligkeit" + Slider (`slider_helligkeit`, 0–100, Standard 80)
  - `on_value` → `light.control` auf `light_screen_background` mit `brightness: x/100.0`
- Farbtest-Quadrate (150×150 px): Rot / Grün / Blau nebeneinander zentriert

**Tab "System":** Platzhalter

**Tab "Kühler":** Platzhalter

### Navigation
- Button "Zurück" unten links, `#444444` → `lvgl.page.show: page_main`

---

## Bekannte LVGL-8-Einschränkungen

| Problem | Lösung |
|---|---|
| `transform_scale_y` nicht verfügbar (LVGL9) | nicht verwenden |
| `bg_opa` nur `%`-Suffix oder Keywords | `50%`, `COVER`, `TRANSP` |
| ESPHome setzt `LV_OBJ_FLAG_CLICKABLE` auf alle `obj` | `clickable: false` direkt im YAML setzen |
| `obj` empfängt keinen `on_click` zuverlässig | Widget-Typ `button` verwenden |

---

## Status

- [x] Font-Definitionen
- [x] Timer-Globals (6 Slots)
- [x] Interval 500ms (alle 6 Slots)
- [x] Hauptseite Titel
- [x] Tank-Widget (Platzhalter)
- [x] 6 Farbslots im Farbrad-Layout
- [x] Split-Design: voller Farbtab + 50%-Timer-Bereich
- [x] Touch: `on_short_click` / `on_long_press`
- [x] Helligkeit-Slider → `light_screen_background`
- [x] Farbtest-Quadrate in Einstellungen
- [ ] Tank-Platzhalter durch echtes PNG ersetzen
- [ ] Einstellungen Tab "System" füllen
- [ ] Einstellungen Tab "Kühler" füllen
