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

Inhalt der rechten Timer-Fläche (`slotN_timer_area`, 230×100%, 40% Opacity):

```
[ ⏱ SVG Stoppuhr 60px ]  [ ⏳ SVG Countdown 60px ]
```

- Zwei unsichtbare Buttons nebeneinander (je 105×100%, `style_invis_btn`)
- Links: `img_stopwatch` (SVG `graphics/stopwatch_23f1.svg`, 60×60)
- Rechts: `img_countdown` (SVG `graphics/cowntdown_23f3.svg`, 60×60)
- Kein Zeitlabel sichtbar, kein Play/Pause-Symbol

---

## Ansicht: Laufend

Struktur der `slotN_running_view` (initial `hidden: true`):

```
[ BG-Symbol 120px 60% (Stopuhr oder Countdown, x:-26) ]
[ Timer-Touch-Bereich 178px | X-Button 50px ]
        lbl_slotN_time: font_timer 60px, CENTER
```

- **Hintergrundsymbol** (`img_bgN`): 120×120 px SVG, 60% Opacity, `align: CENTER, x:-26`
  - Quelle wird per `lv_image_set_src()` im 500ms-Interval getauscht:
    - `slot_is_countdown[i] == false` → `img_stopwatch_bg`
    - `slot_is_countdown[i] == true`  → `img_countdown_bg`
  - `x:-26` kompensiert den 50px X-Button (Timer-Bereich-Mitte = 89px statt 115px)
- **Zeitanzeige** `lbl_slotN_time`: `font_timer` (digital-7_mono, 60px), `align: CENTER, x:0, y:0`
- **X-Button** (50×50, rechts, `btn_slotN_stop`): `script_slot_stop(idx)`

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

### Countdown abgelaufen – automatisches Anfahren + Quittierung (Blink-Phase)

Wenn Countdown auf 0 läuft:
1. `bin_slotN_blink = true`, Slot blinkt, zeigt `00:00`
2. **`slot_status[i] = 2`** (pausiert/abgelaufen)
3. **wenn `auto_motorbetrieb`:** `script_schwenker_goto_slot(i+1)` → Motor fährt **automatisch** zum abgelaufenen Slot

Der Motor kommt selbstständig zum Getränk. Kein manueller Tap nötig, um den Slot anzufahren.

**Short Press auf blinkenden Slot-Bereich (nicht X) — Quittierung:**
1. `bin_slotN_blink = false` (Blink stopp, quittiert)
2. `slot_status[i] = 2` bleibt (bereits pausiert)
3. **kein erneutes `goto_slot`** — Motor steht bereits am Slot
4. Slot verbleibt in Laufend-Ansicht mit `00:00`

Der Slot gilt jetzt als **gequittiert** — der Nutzer hat bestätigt, dass er das Getränk gesehen hat.  
Alle anderen noch aktiven Timer/Countdowns laufen im Hintergrund **unverändert weiter**.

**Mehrere CDs laufen gleichzeitig ab:** Jeder Ablauf triggert `goto_slot`. Da `script_schwenker_goto_slot` `mode: restart` hat, bricht der zweite Aufruf die erste Fahrt ab und fährt zum neueren Slot. Kein Problem.

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
   - **Aktive Slots vorhanden** → Trigger: `script_schwenker_start()` – wird ignoriert falls Motor bereits läuft
   - **Kein aktiver Slot mehr** → Trigger: `script_schwenker_stop()` – wird ignoriert falls Motor bereits steht

---

## Trigger-Logik (Grundprinzip)

Alle timer-ausgelösten Motor-Aktionen sind **einseitige Anfragen (Fire-and-Forget)**, keine erzwungenen Zustandsübergänge.

### Regeln

1. **Trigger prüft aktuellen Motorstatus** und handelt nur wenn der angeforderte Zielzustand sich vom Ist-Zustand unterscheidet.
   - START-Trigger, Motor läuft bereits → **ignoriert**
   - STOP-Trigger, Motor steht bereits → **ignoriert**

2. **Manueller Override erlaubt.** Der Nutzer kann den Motor jederzeit manuell starten oder stoppen — das bricht keine laufende Timer-Logik. Die Timer laufen weiter.

3. **Nächster Trigger wirkt normal.** Wurde der Motor manuell gestartet (z. B. Drehen) und der nächste Pause-Trigger kommt → Motor wird pausiert/goto_slot. Der manuelle Stand wird nicht „gemerkt" und nicht bevorzugt.

4. **Doppelte Trigger werden ignoriert.** Feuern zwei Events dasselbe an (z. B. Weiter-Trigger, aber Motor dreht schon manuell) → kein zweifaches Starten, kein Fehler.

### Praxisbeispiel 1: Manuelles Weiterdrehen nach Pause

```
[Trigger] Pause → goto_slot(N)         Motor stoppt + fährt zu Slot N
[Manuell] Nutzer startet Drehen        Motor dreht
[Trigger] nächster Pause → goto_slot   Motor stoppt + fährt zu neuem Slot
```
→ Funktioniert korrekt. Manueller Start zwischen Triggern ist ausdrücklich erlaubt.

### Praxisbeispiel 2: Quittierung, Motor läuft bereits

```
[Manuell] Nutzer dreht manuell         sw_aktiv / dr_aktiv = true
[Touch]   Nutzer quittiert blink-Slot  → Trigger: schwenker_start() (Weiter, weil andere Slots aktiv)
[Prüfung] Motor läuft bereits          → Trigger ignoriert (Doppel-Trigger = same state)
```
→ Das „Weiter"-Signal der Quittierung wird geschluckt, weil Motor schon läuft.

### Was ist „gleicher Zustand" (Duplikat)?

| Trigger | Ignoriert wenn |
|---------|---------------|
| `schwenker_start()` | `sw_aktiv == true` |
| `drehen_start()` | `dr_aktiv == true` |
| `schwenker_stop()` | `!sw_aktiv && !dr_aktiv` |
| `goto_slot(N)` | Motor steht bereits und aktuelle Position ≈ Slot N (innerhalb Toleranz) |

### Wo wird geprüft?

Die Duplikat-Prüfung liegt in den **Scripts selbst** (`script_schwenker_start`, `script_drehen_start`, `script_schwenker_stop`), nicht im aufrufenden Event-Handler. Die Timer-Events müssen nichts prüfen — sie feuern einfach.

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
- Countdown läuft ab → **wenn `auto_motorbetrieb`:** `script_schwenker_goto_slot(i+1)` (Interval-Loop)
  - `goto_slot` stoppt Motor intern (Stop-Befehl + warten) und fährt dann zur Zielposition

### Blink-Quittierung (Nutzer drückt blinkenden Slot)
- **kein erneutes `goto_slot`** — Motor steht bereits am Slot (automatisch hingefahren)
- Nur: `bin_slotN_blink = false` (Blink quittieren)
- Andere Slots laufen weiter

### Timer/Countdown Stop (X-Button)
- **wenn `auto_rotation`:** Smart Stop/Weiter (s. X-Button-Abschnitt)

### Zusammenfassung

| Ereignis | `auto_rotation == false` | `auto_rotation == true` | Trigger ignoriert wenn |
|----------|--------------------------|-------------------------|------------------------|
| Timer/CD Start | — | `script_schwenker_start()` | Motor läuft bereits |
| Pause (läuft → paused) | — | `script_schwenker_goto_slot(i+1)` | Motor steht + bereits an Slot N |
| Weiter (paused → läuft) | — | `script_schwenker_start()` | Motor läuft bereits |
| CD abgelaufen (auto) | — | `script_schwenker_goto_slot(i+1)` | Motor steht bereits an Slot N |
| Blink quittiert (Nutzer-Touch) | — | *(kein goto_slot – Motor schon dort)* | — |
| X-Stop, andere aktiv | — | `script_schwenker_start()` | Motor läuft bereits |
| X-Stop, keine mehr aktiv | — | `script_schwenker_stop()` | Motor steht bereits |

---

## Zentrale Park & Weiter-Logik

### Grundprinzip

Timer (Stoppuhr) und Countdown verhalten sich **identisch** – es gibt nur eine gemeinsame Logik:

> **Pause = Motor parkt am Slot.**  
> **X = Slot löschen + prüfen ob noch andere laufen → Weiter oder Stop.**

### Wann wird `script_schwenker_goto_slot(N)` ausgelöst?

1. Nutzer pausiert einen **laufenden Timer** (Stoppuhr, status 1→2)
2. Nutzer pausiert einen **laufenden Countdown** (status 1→2)
3. **Countdown läuft automatisch ab** (Interval-Loop, bin_blink→true, status→2) ← neu
4. ~~Nutzer quittiert einen abgelaufenen Countdown~~ — entfällt; Motor steht bereits am Slot

In allen drei Fällen: Motor fährt zu Slot N, Schwenker/Drehen pausiert.  
`script_schwenker_goto_slot` handhabt das interne Stoppen **und Warten** selbst — s. nächster Abschnitt.

### Wie stellt das System fest, ob der Motor steht? (`goto_slot` intern)

`script_schwenker_goto_slot` ist ein ESPHome-Script mit Delays und `wait_until` — es läuft als **Coroutine auf AppCore** und kann tatsächlich warten:

```
goto_slot(slot):
  sw_motor_busy = true

  if dr_aktiv:
    dr_aktiv = false
    F6 speed=0 (Bremsbefehl per CAN)
    delay: 500ms          ← feste Wartezeit, KEINE echte Stillstands-Erkennung (⚠ s.u.)

  if sw_aktiv:
    sw_stop_pending = true         ← Signal an Interval-Loop: nächsten Halbzyklus beenden
    wait_until(!sw_aktiv, 10s)    ← Interval-Loop setzt sw_aktiv=false am Zyklusende
    delay: 200ms                   ← Beruhigungspause

  Motor-Init (Closed Loop, 128 MSTEP)
  script_motor_goto_relative_degree(speed=96, acc=100, rel_deg)
  delay: 3500ms                    ← max. Fahrzeit 180° bei 12 RPM
  sw_motor_busy = false
```

**Schwenken — echte Stillstands-Erkennung:**  
Der 50ms-Interval-Loop prüft während `sw_braking=true` den Positionssensor:  
`delta = |sensor_motor_position[now] - sensor_motor_position[prev]|`  
Erst wenn `delta < 5` (≈ 0,1° bei 16384 Ticks) → Motor steht → neue Richtung starten.  
Für goto_slot: `sw_stop_pending=true` → Interval setzt `sw_aktiv=false` am nächsten Bremsende → `wait_until(!sw_aktiv)` in goto_slot wartet darauf.

**Drehen — nur feste Wartezeit, kein Winkel-Check: ⚠**  
`goto_slot` sendet F6 speed=0 und wartet pauschal 500ms. Die tatsächliche Bremszeit hängt aber von Geschwindigkeit und `dr_acc` ab:  
`ramp_ms = speed_raw × (256 − acc) × 0,05 ms`  
Beispiel: 30 RPM bei 128 MSTEP → speed_raw=240, acc=20 → ramp_ms = 240 × 236 × 0,05 ≈ **2830 ms**  
→ Bei hoher Drehzahl oder sanftem acc kann 500ms zu kurz sein. Motor dreht noch, wenn Positionsfahrt startet → falsche Startposition.

**Lösung: Winkel-basiertes Warten auch für Drehen (⚠ noch zu implementieren):**  
Nach F6 speed=0 auf `sensor_motor_position`-Delta warten, exakt wie beim Schwenken:
```cpp
// Nach dr_aktiv=false + F6 speed=0:
wait_until(abs(pos_now - pos_prev) < 5, timeout=5s)
```
Alternativ: Bremszeit dynamisch berechnen und als delay verwenden:
```cpp
uint32_t ramp_ms = (uint32_t)(speed_raw * (256 - dr_acc) * 0.05f) + 200;
delay: ramp_ms
```

**goto_slot `mode: restart`:** Ein zweiter Aufruf während die Fahrt läuft bricht ab und startet neu. Das ist gewünscht (z.B. zwei CDs laufen gleichzeitig ab — zweiter Slot gewinnt).

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

## Praxis-Szenarien (simulierte Click-Abläufe)

Alle Szenarien: `auto_motorbetrieb = true`, `dr_modus = false` (Schwenken).  
Slot-Nummerierung 1–6 (1-basiert), `i = slot_idx` (0-basiert intern).

---

### Szenario A: Zwei Countdowns, beide enden zu verschiedenen Zeiten

**Setup:** Slot 1 = 2 min, Slot 2 = 4 min

```
t=0:00  [Touch] Slot 1, Countdown-Symbol (⏳)
          → overlay erscheint, Nutzer drückt [2] (Dosen-Reihe)
          → set_countdown_secs(120)
          → slot_status[0]=1, countdown_max_ms[0]=120000
          → script_schwenker_start() — Motor startet Schwenken
          → overlay geschlossen, Slot 1 Laufend-Ansicht

t=0:05  [Touch] Slot 2, Countdown-Symbol
          → Nutzer drückt [4] (Flaschen-Reihe)
          → set_countdown_secs(240)
          → slot_status[1]=1, countdown_max_ms[1]=240000
          → script_schwenker_start() →→ IGNORIERT (sw_aktiv=true ①)
          → Slot 2 Laufend-Ansicht, Motor schwenkt weiter

t=2:00  [Interval] Slot 1: total_ms ≥ 120000
          → slot_status[0]=2, bin_slot1_blink=true, lbl="00:00"
          → script_schwenker_goto_slot(1)  (Slot 1 = 0°) — automatisch!
          Motor fährt zu Position 0°, 3.5 s. Slot 2 läuft weiter ②

t=2:03  Motor an Slot 1. Slot 1 blinkt, zeigt "00:00".
          Slot 2 läuft noch: verbleibend ~1:57 min

t=2:05  [Touch] Slot 1 blinkt → Nutzer drückt auf Slot 1
          → bin_slot1_blink=false  (quittiert, kein erneutes goto_slot)

t=2:07  [Touch] [X] Slot 1 (Getränk entnommen)
          → slot_status[0]=0, zurück Default-Ansicht
          → Smart-Prüfung: slot_status[1]=1 → any_active=true
          → script_schwenker_start() — Motor startet wieder Schwenken

t=4:05  [Interval] Slot 2: total_ms ≥ 240000
          → slot_status[1]=2, bin_slot2_blink=true, lbl="00:00"
          → script_schwenker_goto_slot(2)  (Slot 2 = 60°) — automatisch!
          Motor fährt zu 60°

t=4:08  [Touch] Slot 2 blinkt → Nutzer drückt
          → bin_slot2_blink=false  (quittiert)
          Motor fährt zu Position 60°

          [Touch] [X] Slot 2
          → slot_status[1]=0, zurück Default
          → Smart-Prüfung: alle Slots status=0, kein Blink → any_active=false
          → script_schwenker_stop() →→ IGNORIERT (Motor steht bereits ③)
```

**Anmerkungen:**  
① `set_countdown_secs` ruft `schwenker_start()` auf — Guard in `script_schwenker_start` ignoriert es, weil `sw_aktiv=true`.  
② Slot 2 bleibt in `status=1`; `slot_start_ms[1]` wird nicht zurückgesetzt, Zeit läuft weiter.  
③ `script_schwenker_stop` hat Guard `if (!sw_aktiv) return` — kein doppelter Stop.

---

### Szenario B: Countdown pausiert, Zeit per Long-Press angepasst, dann fortgesetzt

**Setup:** Slot 3 = 3 min

```
t=0:00  [Touch] Slot 3, Countdown-Symbol
          → Nutzer drückt [3] (Dosen-Reihe)
          → set_countdown_secs(180)
          → slot_status[2]=1, countdown_max[2]=180000
          → script_schwenker_start() — Motor startet

t=1:00  [Touch] Slot 3 (Laufend-Ansicht, kein Blink)
          → bin_slot3_blink=false → script_slot_pause_resume(2)
          → Pause-Zweig: elapsed[2] += 60000, status[2]=2
          → script_schwenker_goto_slot(3)  (Slot 3 = 120°)
          Motor fährt zu 120°, 3.5 s

t=1:10  [Long Press] Slot 3 (Laufend-Ansicht, pausiert)
          → script_open_countdown_overlay(2, mode=1)
          → Overlay erscheint (mode=1 = Update, kein Neustart)
          → Nutzer drückt [5] (Flaschen-Reihe, 300 s)
          → set_countdown_secs(300):
              overlay_cd_mode=1 → nur: countdown_max[2] = 300000
              Countdown läuft NICHT neu (status bleibt 2, elapsed bleibt 60000)
          → neues Restziel: 300 000 - 60 000 = 240 s verbleibend
          → Overlay geschlossen

t=1:15  [Touch] Slot 3 (Laufend-Ansicht, pausiert, kein Blink)
          → script_slot_pause_resume(2)
          → Weiter-Zweig: slot_start_ms[2]=millis(), status[2]=1
          → script_schwenker_start() — Motor startet wieder Schwenken

t=5:15  [Interval] Slot 3: elapsed(60s) + laufend(240s) = 300s ≥ 300000
          → slot_status[2]=2, bin_slot3_blink=true, lbl="00:00"
          → script_schwenker_stop()

t=5:17  [Touch] Slot 3 (blinkt)
          → bin_slot3_blink=false
          → script_schwenker_goto_slot(3)  (120°)

          [Touch] [X] Slot 3
          → status[2]=0, Default-Ansicht
          → any_active=false → script_schwenker_stop() →→ IGNORIERT (steht)
```

---

### Szenario C: Drei Countdowns gleichzeitig, mittlerer wird manuell abgebrochen

**Setup:** Slot 4 = 1 min, Slot 5 = 1.5 min, Slot 6 = 2 min *(drei kurze Dosen)*

```
t=0:00  [Touch] Slot 4 → [1] → 60 s → schwenker_start()
t=0:05  [Touch] Slot 5 → [1.5] → 90 s → schwenker_start() IGNORIERT
t=0:10  [Touch] Slot 6 → [2] → 120 s → schwenker_start() IGNORIERT
          Motor schwenkt, alle drei laufen

t=0:45  [Touch] [X] Slot 5 (mitten im Lauf, kein Blink, Nutzer bricht ab)
          → status[4]=0, elapsed[4]=0, Default-Ansicht
          → Smart-Prüfung: status[3]=1 (Slot 4), status[5]=1 (Slot 6) → any_active=true
          → script_schwenker_start() →→ IGNORIERT (läuft bereits)
          Motor schwenkt weiter

t=1:00  [Interval] Slot 4 abgelaufen
          → bin_slot4_blink=true, status[3]=2
          → script_schwenker_goto_slot(4)  (Slot 4 = 180°) — automatisch!
          Motor fährt zu 180°

          Slot 6 läuft weiter (noch ~58 s verbleibend)

t=1:03  Motor an Slot 4. Slot 4 blinkt.

t=1:05  [Touch] Slot 4 (blinkt)
          → bin_slot4_blink=false  (quittiert, kein erneutes goto_slot)

          Slot 6 läuft noch, Motor steht an 180°, sw_aktiv=false

t=1:10  [Touch] [X] Slot 4
          → status[3]=0, Default-Ansicht
          → Smart-Prüfung: status[5]=1 (Slot 6 läuft noch) → any_active=true
          → script_schwenker_start() — Motor startet Schwenken wieder

t=2:10  [Interval] Slot 6 abgelaufen
          → bin_slot6_blink=true, status[5]=2
          → script_schwenker_goto_slot(6)  (Slot 6 = 300°) — automatisch!
          Motor fährt zu 300°

t=2:13  [Touch] Slot 6 (blinkt)
          → bin_slot6_blink=false  (quittiert)

          [Touch] [X] Slot 6
          → status[5]=0
          → alle Slots leer, kein Blink → any_active=false
          → script_schwenker_stop() →→ IGNORIERT (steht bereits)
```

---

### Szenario D: Countdown, Motor manuell übersteuert, Pause-Trigger kommt trotzdem

```
t=0:00  [Touch] Slot 2 → [2] → 120 s → script_schwenker_start()
          Motor schwenkt, auto_motorbetrieb=true

t=0:30  [Manuell] Nutzer schaltet Motor auf Drehen (btn_motor_main Long Press → Einstellungen → Tab Drehen → Start)
          → script_drehen_start() läuft: sw_aktiv=true → hard-stop Schwenker, dann Drehen startet
          → dr_aktiv=true, sw_aktiv=false, dr_modus=true

t=1:00  [Touch] Slot 2 (läuft, kein Blink)
          → script_slot_pause_resume(2)
          → Pause-Zweig: elapsed[1]+=60000, status[1]=2
          → script_schwenker_goto_slot(2)  (Slot 2 = 60°)
          → goto_slot: dr_aktiv=true → dr_aktiv=false, Drehen wird gestoppt, 500 ms Delay
          → Motor fährt zu 60°

t=1:04  [Touch] Slot 2 (pausiert, kein Blink)
          → script_slot_pause_resume(2)
          → Weiter-Zweig: status[1]=1
          → dr_modus=true → script_drehen_start()
          Motor dreht wieder (letzten gespeicherten Drehen-Modus)

t=2:00  [Interval] Slot 2 abgelaufen
          → bin_slot2_blink=true, status[1]=2
          → script_schwenker_goto_slot(2) — automatisch!
          → goto_slot: dr_aktiv=true → Drehen wird intern gestoppt (Hard-Stop + 500ms)
          → Motor fährt zu 60°

t=2:04  Motor an Slot 2. Slot 2 blinkt.

t=2:06  [Touch] Slot 2 (blinkt)
          → bin_slot2_blink=false  (quittiert)

          [Touch] [X] Slot 2
          → status[1]=0, Default-Ansicht
          → any_active=false → script_schwenker_stop() →→ IGNORIERT (steht)
```

**Anmerkung (aktualisiert):** Bei CD-Ablauf wird nun direkt `goto_slot(ii+1)` ausgelöst — dieser stoppt intern sowohl Schwenken als auch Drehen (Hard-Stop + Warten). Separate `schwenker_stop`/`drehen_stop`-Aufrufe beim CD-Ablauf entfallen.

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

### ✅ Alle 5 Punkte umgesetzt (11.05.2026)

#### ① `script_slot_pause_resume` — Pause-Zweig → `goto_slot`
`lvgl_basis.yaml`: Pause-Zweig ruft jetzt `id(script_schwenker_goto_slot)->execute(i + 1)` statt `schwenker_stop()`.

#### ② `script_slot_stop` — Smart Stop/Weiter
`lvgl_basis.yaml`: Loop prüft alle anderen Slots (`slot_status[j] > 0 || blink_s[j]->state`).  
Aktive vorhanden → `drehen_start()` oder `schwenker_start()`; keine mehr → `schwenker_stop()`.

#### ③ `slotN_running_touch` on_short_click — Blink-Check
`lvgl_basis.yaml`, alle 6 Slots: Lambda prüft `blink_s[i]->state`.  
Blinkt → `publish_state(false)` + `goto_slot(i+1)`.  
Nicht blinkt → `script_slot_pause_resume(i)`.

#### ④ `script_schwenker_start` + `script_drehen_start` — Idempotenz-Guard
`schwenker.yaml`: Erste Zeile jedes Scripts prüft `sw_aktiv` bzw. `dr_aktiv` → `return` wenn bereits aktiv.

#### ⑤ Interval-Loop — `drehen_stop` bei CD-Ablauf (aus Szenario D)
`lvgl_basis.yaml`, Interval-Loop: Bei CD-Ablauf beide Scripts aufgerufen wenn `auto_motorbetrieb`:
```
if (sw_aktiv) script_schwenker_stop()
if (dr_aktiv) script_drehen_stop()
```

---

#### ✅ Bereits korrekt (kein Änderungsbedarf)

- `script_schwenker_stop`: Guard `if (!sw_aktiv)` → No-op ✅  
- `script_schwenker_stop`: Blink-Loop → goto_slot wenn Slot blinkt ✅  
- `script_drehen_stop`: Blink-Loop → goto_slot ✅  
- Ring-Marker-Buttons (`ring_slotN_marker`): rufen `goto_slot` direkt auf, ohne stop-Trigger ✅  
- Interval-Loop Countdown-Ablauf: `schwenker_stop()` bei CD=0 ✅  
- `set_countdown_secs` (mode=0): `schwenker_start()`/`drehen_start()` je nach `dr_modus` ✅  
- `script_slot_start`: `schwenker_start()`/`drehen_start()` je nach `dr_modus` ✅
