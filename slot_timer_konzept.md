# Slot-Timer / Countdown – Konzept (ab Slot 1)

## Strukturübersicht

Jeder Slot-Button (rechte Hälfte, `slotN_timer_area`) hat drei **Ansichten**:

| Ansicht | Inhalt |
|---------|--------|
| `default` | Zwei Symbol-Buttons: Stoppuhr + Countdown |
| `laufend` | Zeitanzeige (mm:ss) + X-Stop-Button (rechts) |

Der Wechsel zwischen Ansichten erfolgt durch Sichtbarkeit der Labels/Widgets.

---

## Ansicht: Default

Inhalt der rechten Timer-Fläche:

```
[ ⏱ Stoppuhr ]  [ ⏳ Countdown ]
```

- Zwei Symbole nebeneinander (oder übereinander)
- Kein Zeitlabel sichtbar
- Kein Play/Pause-Symbol

---

## Stoppuhr-Modus

### Short Press auf Stoppuhr-Symbol
- Wechselt zur **Laufend-Ansicht** (Zeitanzeige mm:ss + X-Button)
- Startet die Stoppuhr sofort (`slot_status[i] = 1`, `slot_start_ms[i] = millis()`)

### Short Press auf laufende Stoppuhr (Laufend-Ansicht)
- Timer läuft → **Pause** (`slot_status[i] = 2`)
- Timer pausiert → **Weiter** (`slot_status[i] = 1`)

### X-Button (rechts, Stop)
- Stoppt Stoppuhr / Countdown (gemeinsam, s. unten)
- Setzt Zeit zurück
- Wechselt zurück zur **Default-Ansicht**

---

## Countdown-Modus

### Short Press auf Countdown-Symbol (Default-Ansicht)
Öffnet **Mini-Overlay** (Minuten-Wahl) mit `overlay_cd_mode = 0` (neuer Countdown):

```
[ 1 ][ 2 ][ 3 ][ 4 ][ 5 ][ 6 ][ 7 ][ 8 ][ 9 ][ X ]
```

- Die zuvor eingestellte Minute ist **rot hinterlegt** (falls vorhanden)
- Drücken von 1–9: Countdown-Zeit setzen, Countdown sofort starten, Overlay schließen, Laufend-Ansicht zeigen
- Drücken von X: Overlay schließen, nichts tun

### Countdown läuft (Laufend-Ansicht)
- **Short Press** auf den Timer-Bereich (außerhalb X): **Pause / Weiter**
- **Long Press**: Öffnet Mini-Overlay erneut mit `overlay_cd_mode = 1` (Zeit-Update, kein Neustart):
  - Drücken von 1–9: **nur neue Zielzeit setzen** (`slot_countdown_max_ms[i] = N * 60000`), Countdown läuft weiter ohne Reset — effektiv z. B. 8→9 Minuten = +60 s zur Zielzeit
  - Die aktuell eingestellte Minute bleibt **rot hinterlegt** (`overlay_cd_selected_min[i]`)
  - Drücken von X: Overlay schließen, aktueller Zustand bleibt

### X-Button (rechts, Stop)
- Wie beim Stoppuhr-Modus (gleiche Logik, s. unten)

---

## X-Button / Stop (gemeinsam für beide Modi)

Gilt für Stoppuhr **und** Countdown identisch:

1. `slot_status[i] = 0`
2. `slot_elapsed_ms[i] = 0`, `slot_start_ms[i] = 0`
3. Blink stoppen falls aktiv
4. Tab-Farbe zurücksetzen
5. Zeitlabel auf `"00:00"` (Stoppuhr) oder Countdown-Startwert zurücksetzen
6. Wechsel zurück zur **Default-Ansicht**
7. **Dreher-Automode:** falls `auto_rotation == true` → `script_schwenker_stop()` aufrufen

---

## Dreher-Automode-Integration

Die globale Variable `auto_rotation` (bool, in schwenker.yaml) steuert ob der Drehwurm automatisch mitläuft.

**Regel: Nur wenn `auto_rotation == true` greift die Timer-Kopplung.**

### Stoppuhr-Start
- Short auf Stoppuhr-Symbol → Timer startet → **wenn `auto_rotation`:** `script_schwenker_start()`

### Stoppuhr Pause / Weiter
- Pause → **wenn `auto_rotation`:** `script_schwenker_stop()`
- Weiter → **wenn `auto_rotation`:** `script_schwenker_start()`

### Stoppuhr Stop (X-Button)
- **wenn `auto_rotation`:** `script_schwenker_stop()`

### Countdown-Start (Minuten-Wahl 1–9)
- Countdown startet → **wenn `auto_rotation`:** `script_schwenker_start()`

### Countdown Pause / Weiter
- Pause → **wenn `auto_rotation`:** `script_schwenker_stop()`
- Weiter → **wenn `auto_rotation`:** `script_schwenker_start()`

### Countdown abgelaufen (Blink-Phase, slot_status = 3)
- Countdown läuft ab → **wenn `auto_rotation`:** `script_schwenker_stop()`

### Countdown Stop (X-Button)
- **wenn `auto_rotation`:** `script_schwenker_stop()`

### Zusammenfassung

| Ereignis | `auto_rotation == false` | `auto_rotation == true` |
|----------|--------------------------|-------------------------|
| Timer/CD Start | — | `script_schwenker_start()` |
| Timer Pause | — | `script_schwenker_stop()` |
| Timer Weiter | — | `script_schwenker_start()` |
| CD abgelaufen | — | `script_schwenker_stop()` |
| X-Stop | — | `script_schwenker_stop()` |

---

## Mini-Overlay (Minuten-Wahl)

### Wiederverwendbarkeit

Das Overlay wird einmal definiert und über zwei Variablen gesteuert:

| Variable | Typ | Bedeutung |
|----------|-----|-----------|
| `overlay_cd_slot` | `int` | Index des Slots (0–5) |
| `overlay_cd_mode` | `int` | `0` = neuer Countdown, `1` = Zeit-Update ohne Neustart |
| `overlay_cd_selected_min[6]` | `int[6]` | Zuletzt gewählte Minuten pro Slot (für Rot-Highlight) |

### Positionierung
- `x` / `y` werden vor dem Einblenden anhand des aktiven Slots gesetzt
- Overlay erscheint **über dem jeweiligen Slot-Container**

### Öffnen
```
overlay_cd_slot = i
overlay_cd_mode = 0 (oder 1)
Overlay sichtbar machen
```

### Schließen
- Overlay unsichtbar machen
- `overlay_cd_slot` zurücksetzen (optional)

---

## Globale Variablen (neu)

| Variable | Typ | Zweck |
|----------|-----|-------|
| `overlay_cd_slot` | `int` | Aktiver Slot im Mini-Overlay |
| `overlay_cd_mode` | `int` | 0 = neu, 1 = update |
| `overlay_cd_selected_min` | `int[6]` | Merkt gewählte Minuten je Slot (Rot-Highlight) |

---

## Symbole (LVGL / Font Awesome)

Die ⏱/⏳ aus dem Markdown sind Unicode-Emoji (U+23F1/U+23F3) und können auf dem LVGL-Display **nicht farbig** gerendert werden.

Stattdessen werden Font Awesome 6 Glyphen aus der bereits eingebundenen `fa-solid-900.ttf` verwendet:

| Funktion | FA6-Code | Unicode | Aussehen |
|----------|----------|---------|----------|
| Stoppuhr | `fa-stopwatch` | `\uF2F2` | ⏱ (monochrom) |
| Countdown | `fa-hourglass-half` | `\uF252` | ⏳ (monochrom) |
| Stop / Schließen | `fa-xmark` | `\uF00D` | ✕ (bereits geladen) |

Beide neuen Glyphen (`\uF2F2`, `\uF252`) müssen in `font_icons` (lvgl_basis.yaml) eingetragen werden.

---

## Widget-IDs (Slot 1, exemplarisch)

| ID | Beschreibung |
|----|--------------|
| `slot1_default_view` | Container mit Stoppuhr- und Countdown-Symbol |
| `slot1_btn_stopwatch` | Stoppuhr-Symbol-Button |
| `slot1_btn_countdown` | Countdown-Symbol-Button |
| `lbl_slot1_time` | Zeitanzeige mm:ss (in Laufend-Ansicht) |
| `btn_slot1_stop` | X-Stop-Button (rechts, in Laufend-Ansicht) |

Für Slots 2–6 identisch mit entsprechendem Index.

---

## Status

- [x] Mode 1: Nur die neue Zielzeit wird gesetzt (8→9 = +60 s), kein Neustart
- [x] Symbole: Font Awesome `\uF2F2` (Stoppuhr) und `\uF252` (Sanduhr), beide neu in `font_icons` eintragen

---

## Implementierungsstand & offene Punkte

### ✅ Umgesetzt und aktiv

#### `lvgl_basis.yaml`
- `font_icons`: `\uF2F2` (fa-stopwatch) + `\uF252` (fa-hourglass-half) hinzugefügt
- Neue Globals: `overlay_cd_slot` (int), `overlay_cd_mode` (int), `overlay_cd_selected_min` (int[6], restore=true)
- `slot1_red` → rechte Hälfte komplett neu strukturiert:
  - **Default-Ansicht** (`slot1_default_view`): Container mit zwei Buttons:
    - `slot1_btn_stopwatch` (⏱ `\uF2F2`): Short → Stoppuhr starten + Laufend-Ansicht zeigen + auto_rotation-Start
    - `slot1_btn_countdown` (⏳ `\uF252`): Short → **aktuell deaktiviert (Crash-Test)**
  - **Laufend-Ansicht** (`slot1_running_view`, hidden=true):
    - `slot1_running_touch` (178px breit): Short → Pause/Weiter + auto_rotation
    - Long-Press → **aktuell deaktiviert (Crash-Test)**
    - `btn_slot1_stop` (X `\uF00D`): Stop + Default-Ansicht + auto_rotation-Stop
    - `lbl_slot1_time`: Zeitanzeige (wird vom 500ms-Interval-Loop befüllt)
    - `lbl_slot1_timer_title`: hidden (Kompatibilität Interval-Loop)
    - `lbl_slot1_symbol`: hidden (Kompatibilität Interval-Loop)
- Interval-Loop: bei Countdown-Ablauf → `script_schwenker_stop()` wenn `auto_rotation`
- Alle `ESP_LOGI`-Aufrufe aus LVGL-Event-Handlern (Core 1) entfernt → Stack-Overflow-Fix

#### `lvgl_overlay.yaml`
- `cd_mini.yaml`-Include **auskommentiert** (Crash-Test)

#### `lvgl_overlays/cd_mini.yaml` *(Datei existiert, aber nicht eingebunden)*
- Mini-Overlay `overlay_cd_mini`: 290×120px, Buttons 1–9 + X, 2 Zeilen
- Positionierung via `lv_obj_set_pos()` vor dem Einblenden
- Rot-Highlight für zuvor gewählte Minute (`overlay_cd_selected_min[slot]`)
- Alle `ESP_LOGI` und `snprintf`/`lv_label_set_text` bereits entfernt (Stack-Reduktion)

---

### ⚠️ Crash-Diagnose (Stand 07.05.2026)

**Symptom:** `__ssprint_r` auf Core 1 (LVGL-Task), reproduzierbar bei jedem Boot.  
**Bisherige Maßnahmen:**
1. `snprintf` + `lv_label_set_text` aus cd_mini-Buttons entfernt → Crash blieb
2. Alle `ESP_LOGI` aus LVGL-Event-Handlern entfernt → Crash blieb
3. cd_mini komplett deaktiviert (include + beide Lambdas) → **noch zu testen**

**Nächster Schritt nach positivem Test (kein Crash):**
- `btns[9]`-Array-Initialisierung in Lambdas als Ursache bestätigt
- Lösung: btns-Array durch direkten Aufruf eines `script_cd_mini_open`-Scripts ersetzen (Script läuft auf App-Core 0, nicht auf LVGL-Core 1)

**Nächster Schritt bei weiterhin positivem Crash:**
- Ursache liegt **nicht** im cd_mini-Code
- Dann: Stoppuhr-Start-Lambda isolieren (auto_rotation-Calls auf Core 1?)
- `script_schwenker_start/stop` auf Core 1 aufrufen = potenziell unsafe

---

### 🔜 Noch zu implementieren

1. **cd_mini Crash-sichere Variante:**
   - `btns[9]`-Loop aus Lambda herausziehen → eigenes Script `script_cd_mini_open(int slot, int mode)`
   - Script läuft auf App-Core → kein LVGL-Stack-Problem
   - Nach positivem Crash-Test wieder einbinden

2. **Slots 2–6:** Identische Struktur wie Slot 1 (Default/Laufend-Ansicht + cd_mini-Anbindung)

3. **Interval-Loop Slot1:** `lbl_slot1_timer_title` zeigt im Originalcode "Timer"/"Countdown" — aktuell hidden, evtl. wieder sichtbar machen wenn Layout passt
