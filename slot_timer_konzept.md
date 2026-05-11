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
  - wenn `auto_rotation`: `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot, Schwenker/Drehen stoppt
- Timer pausiert → **Weiter** (`slot_status[i] = 1`)
  - wenn `auto_rotation`: `script_schwenker_start()`

### X-Button (rechts, Stop)
- Stoppt Stoppuhr / Countdown (gemeinsam, s. unten)
- Setzt Zeit zurück
- Wechselt zurück zur **Default-Ansicht**

---

## Countdown-Modus

### Short Press auf Countdown-Symbol (Default-Ansicht)
Ruft `script_open_countdown_overlay(slot_idx=i, mode=0)` auf (neuer Countdown).

Das Overlay erscheint **über dem jeweiligen Slot**:

```
┌─────────────────────────────────────────────────────┐
│  Minuten:                                     [ X ] │
├─────────────────────────────────────────────────────┤
│  🫙  [ 1 ][ 1.5 ][ 2 ][ 2.5 ][ 3 ]                 │
│  🍾  [ 4 ][ 5 ][ 6 ][ 7 ][ 8 ][ 9 ]                │
└─────────────────────────────────────────────────────┘
```

- **Dosen-Reihe** (jar-Icon `\uE516`): 1, 1.5, 2, 2.5, 3 Minuten (= 60, 90, 120, 150, 180 s)
- **Flaschen-Reihe** (bottle-Icon `\uF72F`): 4, 5, 6, 7, 8, 9 Minuten (= 240–540 s)
- Drücken einer Taste: `set_countdown_secs(secs)` → Zeit setzen, Countdown sofort starten (`mode=0`), Overlay schließen, Laufend-Ansicht zeigen
- **[X]**: Overlay schließen, nichts tun

### Countdown läuft (Laufend-Ansicht)
- **Short Press** auf den Timer-Bereich (außerhalb X): `script_slot_pause_resume(i)`
  - Pause (`slot_status[i] = 2`) → wenn `auto_rotation`: `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot
  - Weiter (`slot_status[i] = 1`) → wenn `auto_rotation`: `script_schwenker_start()` oder `script_drehen_start()` (je nach `dr_modus`)
- **Long Press**: `script_open_countdown_overlay(slot_idx=i, mode=1)` (Zeit-Update, kein Neustart):
  - Drücken einer Taste: **nur neue Zielzeit setzen** (`slot_countdown_max_ms[i] = secs * 1000`), Countdown läuft weiter ohne Reset
  - **[X]**: Overlay schließen, aktueller Zustand bleibt

### Countdown abgelaufen – Quittierung (Blink-Phase)

Wenn Countdown auf 0 läuft: `bin_slotN_blink = true`, Slot blinkt, zeigt `00:00`.

**Short Press auf blinkenden Slot-Bereich (nicht X):**
1. `bin_slotN_blink = false` (Blink stopp, quittiert)
2. `slot_status[i] = 2` (Pause / quittiert – Zeit bleibt bei `00:00`)
3. wenn `auto_rotation`: `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot, Schwenker/Drehen stoppt
4. Slot verbleibt in Laufend-Ansicht mit `00:00`, kein Zurück zu Default-Ansicht

Der Slot gilt jetzt als **gequittiert/parkiert** – das Getränk wartet an diesem Slot.  
Alle anderen noch aktiven Timer/Countdowns laufen im Hintergrund **unverändert weiter**.

**X-Button nach Quittierung:** s. unten (Smart Stop / Weiter).

### X-Button (rechts, Stop)
- Wie beim Stoppuhr-Modus (gleiche Logik, s. unten)

---

## X-Button / Stop (gemeinsam für beide Modi)

Gilt für Stoppuhr **und** Countdown identisch. **Neu: Smart Weiter-Logik.**

1. `slot_status[i] = 0`
2. `slot_elapsed_ms[i] = 0`, `slot_start_ms[i] = 0`
3. Blink stoppen falls aktiv (`bin_slotN_blink = false`)
4. Tab-Farbe zurücksetzen
5. Zeitlabel auf `"00:00"` (Stoppuhr) oder Countdown-Startwert zurücksetzen
6. Wechsel zurück zur **Default-Ansicht**
7. **Dreher-Automode Smart Stop/Weiter:** falls `auto_rotation == true`:
   - Prüfe ob noch andere Slots aktiv: irgendein `slot_status[j] > 0` (j ≠ i) **oder** irgendein `bin_slotN_blink == true`
   - **Aktive Slots vorhanden** → `script_schwenker_start()` – Schwenker läuft weiter
   - **Kein aktiver Slot mehr** → `script_schwenker_stop()` – alles stoppt

---

## Dreher-Automode-Integration

Die globale Variable `auto_rotation` (bool, in `schwenker.yaml`) steuert ob der Drehwurm automatisch mitläuft.

**Regel: Nur wenn `auto_rotation == true` greift die Timer-Kopplung.**

### Stoppuhr-Start
- Short auf Stoppuhr-Symbol → Timer startet → **wenn `auto_rotation`:** `script_schwenker_start()`

### Stoppuhr Pause
- Timer läuft → Pause → **wenn `auto_rotation`:** `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot

### Stoppuhr Weiter
- Timer pausiert → Weiter → **wenn `auto_rotation`:** `script_schwenker_start()`

### Stoppuhr Stop (X-Button)
- **wenn `auto_rotation`:** Smart Stop/Weiter (s. X-Button-Abschnitt)

### Countdown-Start (Minuten-Wahl 1–9)
- Countdown startet → **wenn `auto_rotation`:** `script_schwenker_start()`

### Countdown Pause
- Countdown läuft → Pause → **wenn `auto_rotation`:** `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot

### Countdown Weiter
- Countdown pausiert → Weiter → **wenn `auto_rotation`:** `script_schwenker_start()`

### Countdown abgelaufen (Blink-Phase)
- Countdown läuft ab → **wenn `auto_rotation`:** `script_schwenker_stop()` (gemäß Interval-Loop)
  - Motor bleibt wo er war – kein automatisches goto_slot; das macht erst der Nutzer per Touch

### Blink-Quittierung (Nutzer drückt blinkenden Slot)
- **wenn `auto_rotation`:** `script_schwenker_goto_slot(i+1)` → Motor parkt am Slot
  - Andere Slots laufen weiter, Schwenker pausiert

### Timer/Countdown Stop (X-Button)
- **wenn `auto_rotation`:** Smart Stop/Weiter (s. X-Button-Abschnitt)

### Zusammenfassung

| Ereignis | `auto_rotation == false` | `auto_rotation == true` |
|----------|--------------------------|-------------------------|
| Timer/CD Start | — | `script_schwenker_start()` |
| Pause (läuft → paused) | — | `script_schwenker_goto_slot(i+1)` |
| Weiter (paused → läuft) | — | `script_schwenker_start()` |
| CD abgelaufen (auto) | — | `script_schwenker_stop()` |
| Blink quittiert (Nutzer-Touch) | — | `script_schwenker_goto_slot(i+1)` |
| X-Stop, andere aktiv | — | `script_schwenker_start()` (weiter) |
| X-Stop, keine mehr aktiv | — | `script_schwenker_stop()` |

---

## Zentrale Park & Weiter-Logik

### Grundprinzip

Timer (Stoppuhr) und Countdown verhalten sich **identisch** – es gibt nur eine gemeinsame Logik:

> **Pause = Motor parkt am Slot.**  
> **X = Slot löschen + prüfen ob noch andere laufen → Weiter oder Stop.**

### Wann wird `script_schwenker_goto_slot(N)` ausgelöst?

1. Nutzer pausiert einen **laufenden Timer** (Stoppuhr, status 1→2)
2. Nutzer pausiert einen **laufenden Countdown** (status 1→2)
3. Nutzer **quittiert einen abgelaufenen Countdown** (Blink-Touch, bin_blink→false, status→2)

In allen drei Fällen: Motor fährt zu Slot N, Schwenker/Drehen pausiert.  
`script_schwenker_goto_slot` handhabt das interne Stoppen selbst (F5 + Warten + Positionsfahrt).

### Wann kommt der Schwenker wieder?

- Nutzer drückt pausierten Slot **(Weiter**, status 2→1) → `script_schwenker_start()`
- Nutzer drückt **X** auf einem Slot **und andere Slots noch aktiv** → `script_schwenker_start()`
- Nutzer drückt **X** auf dem letzten aktiven Slot → `script_schwenker_stop()`

### Definition „aktiv" für die X-Prüfung

Ein Slot gilt als aktiv, wenn:
- `slot_status[j] > 0` (läuft oder pausiert), **oder**
- `bin_slotN_blink == true` (abgelaufener Countdown wartet noch auf Quittierung)

### Beispiel-Ablauf: Rot 4 min + Gelb 2 min (Countdown)

1. Beide starten → Schwenker läuft.
2. Gelb fertig → blinkt, zeigt `00:00`. Interval-Loop stoppt Schwenker.
3. Nutzer drückt Gelb (blinkt) → Motor fährt zu Gelb. Rot läuft weiter im Hintergrund.
4. Nutzer entnimmt Flasche, drückt Gelb X → Gelb gelöscht. Rot noch aktiv → `script_schwenker_start()`.
5. Rot fertig → blinkt. Interval-Loop stoppt Schwenker erneut.
6. Nutzer drückt Rot (blinkt) → Motor fährt zu Rot.
7. Nutzer drückt Rot X → keine aktiven Slots mehr → `script_schwenker_stop()`.

### Hinweis: Timers laufen im Hintergrund weiter

Während das Getränk an einem gequittierten Slot wartet (status=2, Motor geparkt),  
laufen alle anderen Timers/Countdowns **unverändert weiter** – das Wasser ist kalt genug.

### Status-Wert-Konvention (Klärung)

Aktuelle Implementierung: kein `status=3`. Blink-Zustand wird über `bin_slotN_blink` separat geführt.  
Nach Quittierung: `bin_slotN_blink=false`, `slot_status[i]=2` (pausiert/quittiert).  
**Kein status=3 einführen** – `bin_slotN_blink` reicht als separates Flag.

---

## Mini-Overlay (Countdown-Wahl)

### Aufbau (`overlay_cd_mini`, 510×198 px)

```
┌─────────────────────────────────────────────────────┐
│  Minuten:                                     [ X ] │
├─────────────────────────────────────────────────────┤
│  🫙  [ 1 ][ 1.5 ][ 2 ][ 2.5 ][ 3 ]                 │
│  🍾  [ 4 ][ 5 ][ 6 ][ 7 ][ 8 ][ 9 ]                │
└─────────────────────────────────────────────────────┘
```

| Bereich | Inhalt |
|---------|--------|
| Kopfzeile | Label „Minuten:" + roter [X]-Button |
| Dosen-Reihe | `\uE516` (FA jar) + `btnmatrix_cd_can`: 1, 1.5, 2, 2.5, 3 min |
| Flaschen-Reihe | `\uF72F` (FA wine-bottle) + `btnmatrix_cd_bottle`: 4, 5, 6, 7, 8, 9 min |

**Werte intern in Sekunden** (nicht Minuten):  
1 min = 60 s, 1.5 = 90 s, 2 = 120 s, 2.5 = 150 s, 3 = 180 s, 4 = 240 s … 9 = 540 s

**Taste drücken** → `set_countdown_secs(secs)` → übernimmt Zeit, startet Countdown bei `mode=0`, schließt Overlay.

### Wiederverwendbarkeit

Das Overlay wird über `script_open_countdown_overlay(slot_idx, mode)` gesteuert:

| Variable | Typ | Bedeutung |
|----------|-----|-----------|
| `overlay_cd_slot` | `int` | Index des Slots (0–5) |
| `overlay_cd_mode` | `int` | `0` = neuer Countdown (startet sofort), `1` = Zeit-Update (kein Neustart) |
| `overlay_cd_selected_min[6]` | `int[6]` | Zuletzt gewählte **Sekunden** pro Slot (Namens-Relikt aus alter Min-Logik) |

### Positionierung
- Links (Slots 0–2): `x = 15`, `y = slot_ys[i]` (110 / 250 / 390)
- Rechts (Slots 3–5): `x = 1024 - 510 - 15 = 499`, `y = slot_ys[i]`

### Öffnen
```
script_open_countdown_overlay(slot_idx=i, mode=0)
  → overlay_cd_slot = i, overlay_cd_mode = mode
  → lv_obj_set_pos, lv_obj_clear_flag HIDDEN
```

### Schließen
- `[X]`-Button: `lv_obj_add_flag(overlay_cd_mini, LV_OBJ_FLAG_HIDDEN)`
- Nach Tasten-Wahl: wird in `set_countdown_secs` automatisch geschlossen

### Fonts
| Font | Verwendung |
|------|-----------|
| `font_icons` (40px FA) | Row-Icons (jar, bottle) |
| `font_cd_btn` (42px Roboto) | Zahlen-Buttons

---

## Globale Variablen (neu / relevant)

| Variable | Typ | Zweck |
|----------|-----|-------|
| `overlay_cd_slot` | `int` | Aktiver Slot im Mini-Overlay |
| `overlay_cd_mode` | `int` | 0 = neu starten, 1 = Zeit updaten (kein Neustart) |
| `overlay_cd_selected_min` | `int[6]` | Letzte gewählte **Sekunden** pro Slot (Name = Relikt, Wert = Sekunden) |

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
- **Zentrale Scripts (alle 6 Slots):**
  - `script_slot_start(slot_idx)` — Stoppuhr starten, Default→Laufend-Ansicht, auto_rotation-Start
  - `script_slot_stop(slot_idx)` — Stoppen, Zeit zurücksetzen, Laufend→Default-Ansicht, schwenker_stop (noch ohne Smart-Weiter)
  - `script_slot_pause_resume(slot_idx)` — Pause/Weiter, auto_rotation (`schwenker_stop` bei Pause; **noch: goto_slot statt stop**)
  - `set_countdown_secs(secs)` — Zeit setzen aus Mini-Overlay, bei `mode=0` sofort starten
  - `script_open_countdown_overlay(slot_idx, mode)` — Overlay positionieren + anzeigen
- `slot1_red` → rechte Hälfte komplett neu strukturiert (alle 6 Slots):
  - **Default-Ansicht** (`slotN_default_view`): Container mit zwei Buttons:
    - `slotN_btn_stopwatch` (⏱ `\uF2F2`): Short → `script_slot_start(i)`
    - `slotN_btn_countdown` (⏳ `\uF252`): Short → `script_open_countdown_overlay(i, 0)`
  - **Laufend-Ansicht** (`slotN_running_view`, hidden=true):
    - `slotN_running_touch` (Timer-Bereich): Short → `script_slot_pause_resume(i)`; Long → `script_open_countdown_overlay(i, 1)`
    - `btn_slotN_stop` (X `\uF00D`): → `script_slot_stop(i)`
    - `lbl_slotN_time`: Zeitanzeige (wird vom 500ms-Interval-Loop befüllt)
- Interval-Loop 500ms: bei Countdown-Ablauf → `script_schwenker_stop()` wenn `auto_rotation`
- Alle `ESP_LOGI`-Aufrufe aus LVGL-Event-Handlern (Core 1) entfernt → Stack-Overflow-Fix

#### `lvgl_overlays/cd_mini.yaml` *(eingebunden)*
- Mini-Overlay `overlay_cd_mini`: 510×198 px, zwei Reihen (Dose + Flasche), Kopfzeile + [X]
- `btnmatrix_cd_can`: 1, 1.5, 2, 2.5, 3 min (60–180 s)
- `btnmatrix_cd_bottle`: 4, 5, 6, 7, 8, 9 min (240–540 s)
- Positionierung via `script_open_countdown_overlay` (statische Slot-y-Tabelle)
- Font `font_cd_btn` (42px Roboto) für Zahlen

#### `lvgl_overlay.yaml`
- `cd_mini.yaml`-Include aktiv

---

### ⚠️ Crash-Diagnose (Stand 07.05.2026 – vermutlich gelöst)

**Symptom:** `__ssprint_r` auf Core 1 (LVGL-Task), reproduzierbar bei jedem Boot.  
**Bisherige Maßnahmen:**
1. `snprintf` + `lv_label_set_text` aus cd_mini-Buttons entfernt → Crash blieb
2. Alle `ESP_LOGI` aus LVGL-Event-Handlern entfernt → Crash blieb
3. cd_mini komplett deaktiviert (include + beide Lambdas) → **Crash behoben**
4. Neuaufbau cd_mini als `buttonmatrix`-basiertes Overlay (kein btns[]-Array mehr) → cd_mini jetzt aktiv eingebunden

**Ursache bestätigt:** `btns[9]`-Array-Initialisierung in Lambda auf Core 1 (LVGL-Task) → Stack-Overflow.  
**Lösung:** `buttonmatrix`-Widget ersetzt das btns-Array komplett; Aktion läuft über `set_countdown_secs`-Script (App-Core 0).

---

### 🔜 Noch zu implementieren

1. **Pause → goto_slot (neue Logik):**
   - `script_slot_pause_resume` ruft bei Pause aktuell noch `script_schwenker_stop()` — muss auf `script_schwenker_goto_slot(i+1)` geändert werden
   - Betrifft: `script_slot_pause_resume` in `lvgl_basis.yaml`

2. **Blink-Quittierung → goto_slot:**
   - Wenn `bin_slotN_blink == true` und Nutzer drückt Timer-Bereich (`slotN_running_touch`): `script_schwenker_goto_slot(i+1)` (statt aktuell nichts / schwenker_stop)
   - Status → 2 (pausiert/quittiert), Blink aus
   - Slot verbleibt in Laufend-Ansicht (kein Zurück zu Default)
   - Achtung: der bestehende Blink-Loop in `script_schwenker_stop` fährt den Motor bei manuell ausgelöstem Stop zu blinkenden Slots – das bleibt; neu ist, dass der Nutzer-Touch auf einem blinkenden Slot ebenfalls goto_slot triggert

3. **X-Button Smart Stop/Weiter (`script_slot_stop`):**
   - Nach Slot-Clear: Prüfung auf verbleibende aktive Slots (`slot_status[j]>0` oder `bin_slotN_blink==true` für j≠i)
   - Aktive vorhanden → `script_schwenker_start()` (weiter, nicht stop)
   - Keine mehr → `script_schwenker_stop()` (wie bisher)
   - Betrifft: `script_slot_stop` in `lvgl_basis.yaml`

4. **Interval-Loop Countdown-Ablauf:**
   - Bei Ablauf: `bin_slotN_blink = true` + `script_schwenker_stop()` — bleibt wie bisher
   - Kein automatisches goto_slot beim Ablauf; erst Nutzer-Touch auf Blink löst goto_slot aus

5. **`font_cd_btn` prüfen:** Muss in `font:`-Block von `lvgl_basis.yaml` definiert sein (42px Roboto).

6. **Icon `\uE516`** (FA jar) muss in `font_icons`-Glyphs eingetragen sein.

7. **Kein status=3:** `bin_slotN_blink` bleibt separates Flag. Nach Blink-Quittierung: status=2, bin_blink=false.
