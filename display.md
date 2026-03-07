# Display Drehwurm - Pflichtenheft

## Projekt Übersicht
ESPHome LVGL Display-Konfiguration für einen "Supercooler Drehwurm Kühler"

Datei lvgl_basis.yaml wird in esphome mit Packages: importiert.
Daher nur folgendes generieren:
- lvgl:
- fonts:
- grafiken.

Nicht generieren:
- sonstigen esphome code

## Hardware Spezifikation
- **Display Auflösung**: 1024x600 Pixel
- **Display Typ**: LVGL-kompatibles TFT Display
- **Plattform**: ESPHome mit LVGL Integration


## UI Design Anforderungen

### Page 1: Start/Hauptseite
- **Layout**: Vertikales Layout
- **Titel**: "Supercooler Drehwurm Kühler"
  - Position: Oben mittig
  - Schriftgröße: 60
  - Ausrichtung: Zentriert
- **Sensordaten-Bereiche** (werden später ergänzt):
  - Temperaturanzeigen
  - Status-Indikatoren
  - Grafische Widgets

### Page 2: Einstellungen
- **Layout**: Vertikales Layout
- **Titel**: "Einstellungen"
  - Position: Oben mittig
  - Schriftgröße: 60
  - Ausrichtung: Zentriert
- **Funktionen** (werden später ergänzt):
  - Konfigurations-Optionen
  - System-Einstellungen
  - Navigation zurück zur Hauptseite

## Technische Implementierung

### Fonts
- **Hauptschrift für Titel**: Größe 60 (Roboto Bold oder Montserrat Bold)
- **Standard Text**: Größe 24-32
- **Small Text**: Größe 16-20

### Farbschema
- **Hintergrund**: Weiß/Grau für Bereiche
- **Primärfarbe**: dunkelblau
- **Text**: Schwarz 

### LVGL Komponenten
- Pages (Screens)
- Labels für Text
- Buttons für Navigation
- Container für Layout-Strukturierung
- Sensoren-Integration via Lambda-Funktionen

## Workflow
1. `display.md` definiert das UI-Design
2. GitHub Copilot generiert `lvgl_basis.yaml` basierend auf der Spezifikation
3. Keine manuelle Code-Anpassung erforderlich
4. Änderungen nur in `display.md` vornehmen

## Status
- [x] Basis-Struktur definiert
- [x] Page-Layout spezifiziert
- [ ] Sensoren hinzufügen
- [ ] Interaktive Elemente implementieren
- [ ] Animations-Effekte definieren
