# Schwenker: Aufgabenanalyse
## 1) Umstellung auf 128 Microsteps  ·  2) Sinus-Kurve optional

---

## Ausgangslage

| Parameter | Schwenken | Drehen |
|-----------|-----------|--------|
| Microstep-Befehl | `0x84, 0x40` (= 64) | `0x84, 0x80` (= 128) |
| Steps/Umdrehung | 64 × 200 = **12 800** | 128 × 200 = **25 600** |
| Geschwindigkeit | F5-speed = direkt `sw_max_speed_rpm` | F6-speed = `dr_speed_rpm × 8` |
| Positionseinheit | 16 384 Ticks/Rev (Encoder) | — |
| init-Befehl gesendet in … | `script_schwenker_start`, `script_schwenker_goto_slot`, `script_motor_reboot` | `script_drehen_start` |

Der Kommentar in `goto_slot` lautet **„64 Sub zurücksetzen! (zurück von Drehen-128Sub!)"** — hier wird bewusst von 128 auf 64 zurückgeschaltet, wenn der Schwenker nach einem Drehvorgang anfährt. Nach der Umstellung entfällt dieser Wechsel vollständig.

---

## Teil 1 – Umstellung auf 128 Microsteps

### 1.1 Betroffene Code-Stellen (schwenker.yaml)

| Zeile | Kontext | Aktuell | Neu |
|-------|---------|---------|-----|
| ~481 | `script_schwenker_start`, Motor-Init | `{ 0x84, 0x40 }` // MSTEP: 64 | `{ 0x84, 0x80 }` // MSTEP: 128 |
| ~601 | `script_schwenker_goto_slot`, nach Stop | `{ 0x84, 0x40 }` // 64 Sub zurücksetzen! | `{ 0x84, 0x80 }` // MSTEP: 128 |
| ~655 | `script_motor_reboot`, Re-Init | `{ 0x84, 0x40 }` // MSTEP: 64 | `{ 0x84, 0x80 }` // MSTEP: 128 |

### 1.2 Empfehlung: Substitution anlegen

In `schwippschwenker.yaml` eine Substitution ergänzen, damit der Wert nicht an 3 Stellen hardcodiert ist:

```yaml
# schwippschwenker.yaml – Abschnitt substitutions
c_sw_mstep:  "0x80"   # Schwenker Subdivisions: 0x40=64  0x80=128
c_dr_mstep:  "0x80"   # Drehen Subdivisions:    0x80=128 (zur Dokumentation)
```

In `schwenker.yaml` dann `{ 0x84, 0x40 }` → `{ 0x84, ${c_sw_mstep} }` ersetzen.

### 1.3 F5-Geschwindigkeit – Klärungsbedarf (Hardwaretest!)

Es gibt einen wichtigen Unterschied zwischen F5 (Schwenker) und dem Drehen-Befehl:

- **Drehen (F6-ähnlich via `gen_data_can_motor_set_speed`):**  
  Kommentar: *„128 Subdivisions: echte UPM × 8 = F6-Speed-Wert"*  
  → `speed_val = dr_speed_rpm * 8` vor dem Senden

- **Schwenken (F5 – Fahrt auf absAxis):**  
  `sw_max_speed_rpm` wird **direkt** als speed-Bytes gesendet, kein Multiplikator.

Zwei mögliche Szenarien nach dem Wechsel auf 128 mstep:

| Szenario | Bedeutung | Folge |
|----------|-----------|-------|
| **A (wahrscheinlich)** | F5-Speed ist feste physikalische RPM, unabhängig von mstep | Kein RPM-Faktor nötig; Motor läuft wie bisher |
| **B** | F5-Speed ist intern pulsrate-basiert wie F6 → 2× für 128 vs 64 | `sw_max_speed_rpm` müsste halbiert werden; Presets anpassen |

**→ Pflicht: Hardwaretest mit einem Preset (z. B. 100 RPM) nach dem mstep-Wechsel, bevor alle Presets angepasst werden.**

### 1.4 F5 absAxis – kein Anpassungsbedarf

Der absAxis-Wert `±8 000 000` ist ein **Richtungsindikator** (Fahrziel liegt jenseits jeder realen Position). Dieser Wert ist microstep-unabhängig. → **Keine Änderung nötig.**

### 1.5 goto_slot / F4 relative – kein Anpassungsbedarf

Die Positionsberechnung in `script_schwenker_goto_slot` und `script_motor_goto_relative_degree` verwendet Encoder-Ticks:

```cpp
double steps_double = (16384.0 / 360.0) * degree_rel;  // Encoder-basiert
```

Die 16 384 Ticks/Umdrehung kommen vom internen Encoder des MKS Servo42D und sind **microstep-unabhängig**. → **Keine Änderung nötig.**

### 1.6 Kommentare aktualisieren

- `goto_slot`: Kommentar *„64 Sub zurücksetzen! (zurück von Drehen-128Sub!)"* entfernen – nach der Umstellung laufen beide Modi auf 128; kein Wechsel mehr nötig.
- `script_motor_reboot`: Kommentar *„64 subdivisions"* → *„128 subdivisions"*
- `script_schwenker_start`: Kommentar *„64 Subdivisions"* → *„128 Subdivisions"*

### 1.7 Vorteil der Umstellung

- **Glattere Richtungsumkehr:** 25 600 statt 12 800 interne Schritte/Umdrehung → feineres Geschwindigkeitsprofil
- **Kein Modus-Wechsel mehr:** `goto_slot` muss den mstep-Wert nicht mehr zurücksetzen; spart einen CAN-Frame und vereinfacht die Zustandsmaschine
- **Konsistenz:** Schwenken und Drehen verwenden identische Motor-Konfiguration

---

## Teil 2 – Sinus-Kurve optional

### 2.1 Aktueller Kurventyp (Speed-Trapez mit Sinus-Flanken)

Im 50ms-Interval wird `sinval` so berechnet:

```
sw_acc = 10–100 %  →  ramp_ms = sw_acc/100 × T × 0.5
```

| sw_acc | Profil | Flankenform |
|--------|--------|-------------|
| 100 % | Voller Sinus-Bogen (kein Plateau) | sin(0 → π/2) aufwärts, sin(π/2 → 0) abwärts |
| < 100 % | Sinus-Trapez | sin-Anlauf · Plateau · sin-Ablauf |
| 10 % | Minimale Flanke + langes Plateau | kurze sin-Spitzen |

### 2.2 Neuer Modus: Lineares Trapez

Gleiche `sw_acc`-Steuerung, aber Flanken = **lineare Interpolation** (konstante Beschleunigung):

| sw_acc | Profil | Flankenform |
|--------|--------|-------------|
| 100 % | Dreieck | linear 0→1, linear 1→0 |
| < 100 % | Lineares Trapez | linear an · Plateau · linear ab |

### 2.3 Neuer Global

```yaml
# schwenker.yaml – globals
- id: sw_sinus_aktiv
  type: bool
  restore_value: yes
  initial_value: 'true'   # true = Sinus (bisheriges Verhalten, abwärtskompatibel)
```

### 2.4 Änderung im 50ms-Interval (ca. 10 Zeilen)

Nur der sinval-Block wird durch eine if/else-Verzweigung ersetzt:

```
Sinus-Pfad  (sw_sinus_aktiv == true):  sinval = sinf(...)
Linear-Pfad (sw_sinus_aktiv == false): val = phase_f / ramp_ms  (linear 0→1 / 1→0)
```

Alle anderen Teile des Regelzyklus (Flip-Trigger, F5-Paket-Bau, Stop-Logik) bleiben unverändert.

### 2.5 UI – benötigte Änderungen

#### In `lvgl_overlays/schwenker.yaml` (Settings-Overlay):

| Element | Typ | Funktion |
|---------|-----|---------|
| `sw_sinus_toggle` | Toggle-Switch | schreibt `sw_sinus_aktiv` |
| Beschriftung | Label | z. B. *„Sinus"* / *„Linear"* |
| Optionale zweite Zeile | Info-Label | z. B. *„Sanft / S-Kurve"* vs. *„Trapez / konstante Beschl."* |

#### Optional: Sinus-Einstellung pro Preset

Pro Preset (1–5) einen zusätzlichen bool:

```yaml
- id: preset1_sinus
  type: bool
  restore_value: yes
  initial_value: 'true'
# ... analog für preset2–5
```

Bei Preset-Abruf: `sw_sinus_aktiv = presetN_sinus` mitsetzt.

### 2.6 Vererbung auf `script_schwenker_settings_open`

Das Öffnen des Settings-Overlays synchronisiert bereits Arc- und Label-Werte. Den Toggle-Zustand ebenfalls synchronisieren:

```
lv_obj_add/clear_state(id(sw_sinus_toggle), LV_STATE_CHECKED, id(sw_sinus_aktiv))
```

---

## Zusammenfassung: Änderungsliste

### Pflicht-Änderungen

| # | Datei | Änderung |
|---|-------|----------|
| 1 | `schwenker.yaml` L481 | `0x84, 0x40` → `0x84, 0x80` (script_schwenker_start) |
| 2 | `schwenker.yaml` L601 | `0x84, 0x40` → `0x84, 0x80` (script_schwenker_goto_slot) |
| 3 | `schwenker.yaml` L655 | `0x84, 0x40` → `0x84, 0x80` (script_motor_reboot) |
| 4 | `schwenker.yaml` L601 | Kommentar „64 Sub zurücksetzen" entfernen/anpassen |
| 5 | Hardwaretest | F5-RPM-Faktor bei 128 mstep verifizieren (Szenario A vs. B) |
| 6 | `schwenker.yaml` | `sw_sinus_aktiv`-Global hinzufügen |
| 7 | `schwenker.yaml` | 50ms-Interval: sinval-Block in if/else aufteilen |
| 8 | `lvgl_overlays/schwenker.yaml` | Toggle-Switch für `sw_sinus_aktiv` |

### Empfohlene Erweiterungen

| # | Datei | Änderung |
|---|-------|----------|
| 9 | `schwippschwenker.yaml` | Substitution `c_sw_mstep: "0x80"` ergänzen |
| 10 | `schwenker.yaml` | Hardcoded `0x40` durch `${c_sw_mstep}` ersetzen |
| 11 | `schwenker.yaml` | `preset[1..5]_sinus`-Globals für Sinus-pro-Preset |

### Nicht betroffen (kein Änderungsbedarf)

- F5 absAxis-Wert `±8 000 000` (Richtungsindikator, mstep-unabhängig)
- goto_slot Positionsberechnung (`16384 / 360.0 × degree_rel`, Encoder-basiert)
- Preset-RPM-Globals `preset[1..5]_rpm` (sofern Szenario A bestätigt wird)
- Drehen-Scripte und F6-Geschwindigkeitsformel (bereits korrekt für 128 mstep)
- CAN-Frame-Parsing und Sensor-Auswertung

---

## Reihenfolge der Umsetzung (empfohlen)

1. **128-mstep init** in den drei Scripten ändern (Aufgaben 1–4)
2. **Hardwaretest** F5-RPM bei neuer mstep-Einstellung, Preset 100 RPM beobachten
3. Bei Bedarf Preset-RPMs halbieren (Szenario B) oder belassen (Szenario A)
4. **Sinus optional**: Global + Interval-Lambda + Toggle-UI (Aufgaben 6–8)
5. Optional: Substitution + Preset-Sinus-Globals (Aufgaben 9–11)
