# Schwenken: Umstellung auf 128 Microsteps + Sinus-Abschaltung

**Datum:** 2026-05-11  
**Status: ✅ DURCHGEFÜHRT**  
**Ziel:** Schwenkmodus auf 128 Microsteps (wie Drehmodus) umstellen.  
Sinus-Hüllkurve entfernen → Motor übernimmt Beschleunigung komplett per `acc`-Byte.

**Abweichungen vom ursprünglichen Plan:**
- ACC-Einheit: **5–100 % in 5%-Schritten** (nicht 1–254 raw wie in Plan A1 skizziert)
  - `100 %` = `acc_byte 254` (schnellste nutzbare Rampe, nicht 255 = Ruck)
  - `5 %` = `acc_byte ~13` (sanfteste Rampe)
- Motorbetrieb: **F6** (`gen_data_can_motor_set_speed`) statt F5 Positionsmodus — kein "fernes Ziel" mehr
- `lbl_trapez_shape`: `hidden: true` + `text: ""` (statt nur leer)
- Snap-Logik im Arc: rekursionssichere Variante (`if (val != (int)x) lv_arc_set_value(...)`)
- `sw_acc` initial_value: **50** (Standard, vorher 100 = purer Sinus war zu aggressiv als ACC)


---

## Hintergrund & Begründung

| | Alt (64 Sub, Sinus) | Neu (128 Sub, Motor-acc) |
|---|---|---|
| Microsteps | 64 (`0x84, 0x40`) | 128 (`0x84, 0x80`) |
| Beschleunigung | Software-Sinus-Hüllkurve, 50ms-Takt | Motor-interne Rampe per acc-Byte 1–254 |
| Speed-Faktor F5 | `sw_max_speed_rpm × 1` (inkonsistent) | `sw_max_speed_rpm × 8` (echte UPM, wie Drehen) |
| `sw_acc` Bedeutung | Rampenanteil der Halbperiode in % (10–100) | Motor-acc-Byte 1–254 |
| Motoreigene Beschleunigung | Zu schnell bei ≤64 Sub (härtester Rampentyp) | Bei 128 Sub ist der langsamste Motor-acc ~8× feiner → nutzbar |
| F5-Sendeintervall | Jede 50ms (Sinus-Update) | Nur bei Richtungswechsel (oder weiter 50ms, Motor ignoriert Wiederholungen) |

**Warum macht 128 Sub den Motor-acc brauchbar:**  
Der MKS Servo42D interpoliert den acc-Rampe über `(256 - acc_byte) × 0,05ms × Schrittzahl`.  
Bei 128 Sub sind es 128× so viele Schritte pro Umdrehung wie bei 1 Sub.  
Damit ist die Rampendauer bei gleichem acc-Byte ~8× länger als bei 16 Sub (Faktor 128/16).  
Selbst `acc = 254` (schnellste Rampe) ergibt bei 128 Sub mit wenigen UPM eine weiche Beschleunigung.

---

## Betroffene Dateien

| Datei | Änderungen |
|---|---|
| `schwenker.yaml` | Globals `sw_acc`/`sw_motor_ramp`, alle 3 MSTEP-Befehle, 50ms-Interval-Kern |
| `lvgl_overlays/schwenker.yaml` | `arc_acc` Range/Format, Trapez-Label, Preset-Label-Strings |

---

## TODO-Liste (Durchführungsreihenfolge)

### A – `schwenker.yaml`: Globals

---

#### A1: Global `sw_acc` — Semantik ändern

**Aktuell** (Rampenanteil % der Halbperiode, 10–100):
```yaml
  - id: sw_acc
    type: int
    restore_value: yes
    initial_value: '100'        # 10–100 %: Anteil der Halbperiode für Rampen (acc+dec zusammen)
                                # 100 % = purer Sinus (wie bisher) | 50 % = 25% rauf, Plateau, 25% runter
```

**Neu** (Motor-acc-Byte, 1–254):
```yaml
  - id: sw_acc
    type: int
    restore_value: yes
    initial_value: '128'        # Motor-internes acc-Byte 1–254 für F5 (hardware-Rampe)
                                # 1 = schnellste Rampe | 254 = langsamste | 0/255 = kein Ramping (Ruck!)
                                # bei 128 Sub: acc=200 ≈ ~1.5s auf Ziel-UPM
```

**Hinweis:** NVS-Restore-Wert wird beim ersten Boot nach Flashem ignoriert wenn ein anderer Wert
noch gespeichert ist. Ggf. `restore_value: no` temporär setzen → nach erstem Boot zurück auf `yes`.

---

#### A2: Global `sw_motor_ramp` — als veraltet markieren (nicht entfernen)

**Aktuell:** Motor-acc-Byte für F5 beim Schwenken (1–254).  
**Neu:** Wird nicht mehr aktiv genutzt. `sw_acc` übernimmt diese Rolle direkt.  
Nicht entfernen — NVS-Key bleibt erhalten, dient als Backup für Sinus-Wiederherstellung.

```yaml
  - id: sw_motor_ramp           # [VERALTET] war Motor-internes acc-Byte für Sinus-Modus
    type: int                   # Ersetzt durch sw_acc (trägt jetzt den acc-Byte-Wert)
    restore_value: yes          # NVS-Key beibehalten für ggf. Sinus-Rollback
    initial_value: '200'        # Originalwert belassen
```

---

#### A3: Preset-Globals `preset[1..5]_acc` — Werte umrechnen

Die Preset-acc-Werte waren Prozentwerte (Rampenanteil). Jetzt Motor-acc-Bytes.

| Preset | Alter Wert (%) | Neuer Wert (Motor-acc-Byte) | Sinn |
|---|---|---|---|
| 1 | 100 | 200 | sanfte Beschleunigung (~1.5s Rampe) |
| 2 | 100 | 200 | sanfte Beschleunigung |
| 3 | 100 | 180 | etwas direkter |
| 4 | 75  | 150 | mittel |
| 5 | 50  | 100 | schnell aber weich |

```yaml
  - id: preset1_acc
    type: int
    restore_value: yes
    initial_value: '200'   # war: 100 (%)

  - id: preset2_acc
    type: int
    restore_value: yes
    initial_value: '200'   # war: 100 (%)

  - id: preset3_acc
    type: int
    restore_value: yes
    initial_value: '180'   # war: 100 (%)

  - id: preset4_acc
    type: int
    restore_value: yes
    initial_value: '150'   # war: 75 (%)

  - id: preset5_acc
    type: int
    restore_value: yes
    initial_value: '100'   # war: 50 (%)
```

---

### B – `schwenker.yaml`: Scripts

---

#### B1: `script_schwenker_start` — MSTEP 64 → 128

**Zeile suchen:** `{ 0x84, 0x40 }` mit Kommentar `// MSTEP: 64 subdivisions`

```cpp
// ALT:
data = add_crc(${motor_can_id}, { 0x84, 0x40 });        // MSTEP: 64 subdivisions
// NEU:
data = add_crc(${motor_can_id}, { 0x84, 0x80 });        // MSTEP: 128 subdivisions (wie Drehen)
```

---

#### B2: `script_motor_reboot` — MSTEP 64 → 128

**Zeile suchen:** `{ 0x84, 0x40 }` in script_motor_reboot

```cpp
// ALT:
data = add_crc(${motor_can_id}, { 0x84, 0x40 });        // MSTEP: 64 subdivisions
// NEU:
data = add_crc(${motor_can_id}, { 0x84, 0x80 });        // MSTEP: 128 subdivisions
```

---

#### B3: `script_schwenker_goto_slot` — MSTEP 64 → 128, Kommentar korrigieren

Zwei Stellen in goto_slot:

**Stelle 1** — nach Drehen-Stop, vor Positionsfahrt:
```cpp
// ALT:
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x82, 0x04 }));
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x84, 0x40 }));  // 64 Sub zurücksetzen!
ESP_LOGI("schwenker", "goto_slot: Closed Loop + 64 Sub");
// NEU:
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x82, 0x04 }));
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x84, 0x80 }));  // 128 Sub
ESP_LOGI("schwenker", "goto_slot: Closed Loop + 128 Sub");
```

**Stelle 2** — innerhalb goto_slot Lambda (identischer Block):
```cpp
// ALT: (gleicher Block wie oben)
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x84, 0x40 }));  // 64 Sub zurücksetzen!
// NEU:
id(can_bus)->send_data(${motor_can_id}, false, add_crc(${motor_can_id}, { 0x84, 0x80 }));  // 128 Sub
```

**Hinweis:** `script_motor_goto_relative_degree` (in `motorcontrol_can-bus.yaml`) berechnet  
Schritte-Anzahl unabhängig von Subdivisions (nutzt 16384 Steps/Umdrehung aus Feedback).  
Kein Anpassungsbedarf dort.

---

#### B4: `script_schwenker_settings_open` — acc-Label-Format

**Suchen:** `snprintf(b3,sizeof(b3),"%d%%", a);`

```cpp
// ALT:
char b3[6]; snprintf(b3,sizeof(b3),"%d%%", a);
lv_label_set_text(id(lbl_arc_acc_val), b3);
// NEU:
char b3[6]; snprintf(b3,sizeof(b3),"%d", a);
lv_label_set_text(id(lbl_arc_acc_val), b3);
```

**Und den Trapez-Shape-Block entfernen** (die 4 Zeilen mit `lbl_trapez_shape`):
```cpp
// ENTFERNEN (war Sinus-Visualisierung, jetzt obsolet):
const char* sh;
if(a>=80)      sh="/\\";
else if(a>=55) sh="/_\\";
else if(a>=35) sh="/___\\";
else           sh="/_____\\";
lv_label_set_text(id(lbl_trapez_shape), sh);
```

---

### C – `schwenker.yaml`: 50ms-Interval — Hauptänderung

Dies ist die größte Änderung. Der Sinus-Kern des 50ms-Intervals wird durch direkte  
Konstantgeschwindigkeit ersetzt. Der Motor regelt die Rampen selbst per `sw_acc`.

#### C1: Neuer Schwenk-Kern (ersetzt alles ab `// ══ Speed-Trapez`)

**ALT (Sinus/Trapez — vollständiger Block für Rollback, Abschnitt C-BACKUP):**
```cpp
          uint8_t acc   = (uint8_t)std::max(1, std::min(254, id(sw_motor_ramp)));
          int32_t target;

          // ══ Speed-Trapez: acc% der Halbperiode für Rampen, Rest Plateau ══
          id(sw_phase_ms) += 50;
          float T        = (float)id(sw_halbperiode_ms);
          float pct      = (float)std::max(10, std::min(100, id(sw_acc))) / 100.0f;
          float ramp_ms  = pct * T * 0.5f;  // Dauer einer Flanke
          float phase_f  = (float)id(sw_phase_ms);
          float sinval;
          if (phase_f < ramp_ms) {
            sinval = sinf((M_PI * 0.5f) * phase_f / ramp_ms);       // Hochrampe 0→1
          } else if (phase_f >= T - ramp_ms) {
            sinval = sinf((M_PI * 0.5f) * (T - phase_f) / ramp_ms); // Abrampe 1→0
          } else {
            sinval = 1.0f;  // Plateau
          }
          if (sinval < 0.0f) sinval = 0.0f;
          int speed  = (int)(id(sw_max_speed_rpm) * sinval);
          ESP_LOGD("schwenker", "Dir:%+d spd:%3d RPM sin:%.3f phase:%u",
                   id(sw_richtung), speed, sinval, id(sw_phase_ms));

          // Flip-Trigger: sinval ist bereits seit halbperiode_ms = 0 (Abrampe beendet),
          // die extra 250 ms geben dem Motor Zeit (acc=200 ≈ 280 ms Auslaufzeit) wirklich
          // auf 0 abzubremsen, bevor die neue Richtung befohlen wird.
          if (id(sw_phase_ms) >= id(sw_halbperiode_ms) + 250U) {
            if (id(sw_stop_pending)) {
              // Zyklus beendet: jetzt wirklich stoppen
              id(sw_stop_pending) = false;
              id(sw_aktiv) = false;
              uint8_t sacc = (uint8_t)std::max(1, std::min(254, id(sw_motor_ramp)));
              int32_t stgt = (int32_t)(id(sw_richtung)) * 8000000;
              uint32_t sv  = (uint32_t)(stgt) & 0x00FFFFFFU;
              id(can_bus)->send_data(${motor_can_id}, false,
                  add_crc(${motor_can_id}, { 0xF5, 0x00, 0x00, sacc,
                      (uint8_t)((sv>>16)&0xFF), (uint8_t)((sv>>8)&0xFF), (uint8_t)(sv&0xFF) }));
              lv_obj_set_style_bg_color(id(btn_schwenker_main), lv_color_hex(0x666666), 0);
              lv_obj_set_style_bg_color(id(btn_schwenker_overlay_toggle), lv_color_hex(0x666666), 0);
              id(sw_motor_busy) = false;
              id(sw_motor_last_move_ms) = millis();
              // Richtung für nächsten Start vorbereiten (Gegenrichtung)
              id(sw_richtung) = -(id(sw_richtung));
              ESP_LOGI("schwenker", "STOP: Zyklus beendet, Motor bremst, nächste Richtung=%+d", id(sw_richtung));
              return;
            }
            id(sw_richtung) = -(id(sw_richtung));   // explizites Negieren (nicht *=)
            id(sw_phase_ms) = 0;
            ESP_LOGI("schwenker", "Richtungswechsel → %+d", id(sw_richtung));
          }

          // target NACH dem Flip berechnen → Flip-Tick schickt bereits neue Richtung
          // mit speed=0 (sinval=0 für phase > halbperiode_ms durch Clamp oben)
          target = (int32_t)(id(sw_richtung)) * 8000000;

          // ══ F5 CAN-Paket senden ════════════════════════════════════════
          uint8_t  spd_h = (uint8_t)(speed >> 8);
          uint8_t  spd_l = (uint8_t)(speed & 0xFF);
          uint32_t val   = (uint32_t)(target) & 0x00FFFFFFU;
          uint8_t  p_hh  = (uint8_t)((val >> 16) & 0xFF);
          uint8_t  p_h   = (uint8_t)((val >>  8) & 0xFF);
          uint8_t  p_l   = (uint8_t)( val         & 0xFF);
          id(can_bus)->send_data(${motor_can_id}, false,
              add_crc(${motor_can_id}, { 0xF5, spd_h, spd_l, acc, p_hh, p_h, p_l }));
```

**NEU (Motor-acc, 128 Sub):**
```cpp
          // ══ Motor-acc Modus: 128 Sub, Motor regelt Rampe intern via acc-Byte ══
          // sw_acc = Motor-acc-Byte 1–254 (1=schnell, 254=langsam, nie 0/255=Ruck!)
          // sw_max_speed_rpm = echte Ziel-UPM; F5-Speed = rpm × 8 (für 128 Sub)
          uint8_t acc    = (uint8_t)std::max(1, std::min(254, id(sw_acc)));
          int     speed  = std::max(1, std::min(3000, id(sw_max_speed_rpm) * 8));
          int32_t target;

          id(sw_phase_ms) += 50;

          // Flip-Trigger: Halbperiode abgelaufen → Richtung wechseln
          if (id(sw_phase_ms) >= id(sw_halbperiode_ms)) {
            if (id(sw_stop_pending)) {
              // Sanfter Stop: F5 speed=0, Motor bremst intern per acc-Byte aus
              id(sw_stop_pending) = false;
              id(sw_aktiv) = false;
              uint32_t sv = (uint32_t)((int32_t)(id(sw_richtung)) * 8000000) & 0x00FFFFFFU;
              id(can_bus)->send_data(${motor_can_id}, false,
                  add_crc(${motor_can_id}, { 0xF5, 0x00, 0x00, acc,
                      (uint8_t)((sv>>16)&0xFF), (uint8_t)((sv>>8)&0xFF), (uint8_t)(sv&0xFF) }));
              lv_obj_set_style_bg_color(id(btn_schwenker_main), lv_color_hex(0x666666), 0);
              lv_obj_set_style_bg_color(id(btn_schwenker_overlay_toggle), lv_color_hex(0x666666), 0);
              id(sw_motor_busy) = false;
              id(sw_motor_last_move_ms) = millis();
              id(sw_richtung) = -(id(sw_richtung));
              ESP_LOGI("schwenker", "STOP: Motor bremst (acc=%d), nächste Richtung=%+d",
                       (int)acc, id(sw_richtung));
              return;
            }
            id(sw_richtung) = -(id(sw_richtung));
            id(sw_phase_ms) = 0;
            ESP_LOGI("schwenker", "Richtungswechsel → %+d  speed=%d(F5)  acc=%d",
                     id(sw_richtung), speed, (int)acc);
          }

          // F5 senden: konstante Speed, Motor-internes Ramping per acc-Byte
          target = (int32_t)(id(sw_richtung)) * 8000000;
          uint8_t  spd_h = (uint8_t)(speed >> 8);
          uint8_t  spd_l = (uint8_t)(speed & 0xFF);
          uint32_t val   = (uint32_t)(target) & 0x00FFFFFFU;
          id(can_bus)->send_data(${motor_can_id}, false,
              add_crc(${motor_can_id}, { 0xF5, spd_h, spd_l, acc,
                  (uint8_t)((val>>16)&0xFF), (uint8_t)((val>>8)&0xFF), (uint8_t)(val&0xFF) }));
```

**Wichtige Unterschiede neu vs. alt:**
- Kein `sw_motor_ramp` mehr — `sw_acc` ist direkt das acc-Byte
- Kein `+250U` Auslaufzeit — Motor bremst selbst per acc-Byte
- Speed = `sw_max_speed_rpm × 8` (echte UPM → F5 bei 128 Sub)
- F5 wird weiter jede 50ms gesendet (Motor ignoriert redundante Befehle bei gleicher Speed)

---

### D – `lvgl_overlays/schwenker.yaml`

---

#### D1: `arc_acc` — Range und Format ändern

```yaml
# ALT:
                    - arc:
                        id: arc_acc
                        ...
                        min_value: 10
                        max_value: 100
                        value: 100
                        ...
                        on_value:
                          - lambda: |-
                              id(sw_acc) = (int)x;
                              char buf[8];
                              snprintf(buf, sizeof(buf), "%d%%", (int)x);
                              lv_label_set_text(id(lbl_arc_acc_val), buf);
                              const char* shape;
                              if (x >= 80)      shape = "/\\";
                              else if (x >= 55) shape = "/_\\";
                              else if (x >= 35) shape = "/___\\";
                              else              shape = "/_____\\";
                              lv_label_set_text(id(lbl_trapez_shape), shape);

# NEU:
                    - arc:
                        id: arc_acc
                        ...
                        min_value: 1
                        max_value: 254
                        value: 128
                        ...
                        on_value:
                          - lambda: |-
                              id(sw_acc) = (int)x;
                              char buf[6];
                              snprintf(buf, sizeof(buf), "%d", (int)x);
                              lv_label_set_text(id(lbl_arc_acc_val), buf);
```

---

#### D2: `lbl_arc_acc_val` — Initial-Text

```yaml
# ALT:
                    - label:
                        id: lbl_arc_acc_val
                        text: "100%"
# NEU:
                    - label:
                        id: lbl_arc_acc_val
                        text: "128"
```

---

#### D3: `lbl_trapez_shape` — ausblenden (nicht entfernen, für Rollback)

```yaml
# ALT:
                    - label:
                        id: lbl_trapez_shape
                        text: "/\\"
                        align: TOP_MID
                        y: 210
                        text_font: font_normal
                        text_color: 0xFF9933
                        text_align: CENTER

# NEU:
                    - label:
                        id: lbl_trapez_shape
                        text: ""           # [VERALTET] Trapez war Sinus-Visualisierung
                        hidden: true
                        align: TOP_MID
                        y: 210
                        text_font: font_normal
                        text_color: 0xFF9933
                        text_align: CENTER
```

---

#### D4: Preset-Buttons Schwenken — Label-Format acc

In allen 5 `on_short_click`- und `on_long_press`-Lambdas der Preset-Buttons:

```cpp
// ALT (in on_short_click):
char b3[8]; snprintf(b3,sizeof(b3),"%d%%",id(preset1_acc));
lv_label_set_text(id(lbl_arc_acc_val), b3);
int _a1=id(preset1_acc); const char* _sh1;
if(_a1>=80) _sh1="/\\";
else if(_a1>=55) _sh1="/_\\";
else if(_a1>=35) _sh1="/___\\";
else _sh1="/_____\\";
lv_label_set_text(id(lbl_trapez_shape), _sh1);

// NEU (in on_short_click):
char b3[6]; snprintf(b3,sizeof(b3),"%d",id(preset1_acc));
lv_label_set_text(id(lbl_arc_acc_val), b3);
```

```cpp
// ALT (in on_long_press):
snprintf(buf,sizeof(buf),"%.1fs \xc2\xb7 %d Upm \xc2\xb7 %d%%",
         id(preset1_hp_ms)/1000.0f, id(preset1_rpm), id(preset1_acc));
lv_label_set_text(id(lbl_preset1_vals), buf);

// NEU (in on_long_press):
snprintf(buf,sizeof(buf),"%.1fs \xc2\xb7 %d Upm \xc2\xb7 A:%d",
         id(preset1_hp_ms)/1000.0f, id(preset1_rpm), id(preset1_acc));
lv_label_set_text(id(lbl_preset1_vals), buf);
```

*(Gleiches Schema für Presets 2–5)*

---

#### D5: Preset-Label Initial-Texte (`lbl_presetN_vals`) — Werte referenzieren

Initiale Label-Texte für die Preset-Buttons:

| Preset | Alt | Neu |
|---|---|---|
| 1 | `"3.2s · 4 Upm · 100%"` | `"3.2s · 4 Upm · A:200"` |
| 2 | `"2.0s · 8 Upm · 100%"` | `"2.0s · 8 Upm · A:200"` |
| 3 | `"5.0s · 12 Upm · 100%"` | `"5.0s · 12 Upm · A:180"` |
| 4 | `"3.0s · 6 Upm · 75%"` | `"3.0s · 6 Upm · A:150"` |
| 5 | `"5.0s · 20 Upm · 50%"` | `"5.0s · 20 Upm · A:100"` |

---

## Sinus-Rollback (vollständige Wiederherstellung)

Falls der Motor-acc-Ansatz nicht zufriedenstellend ist, kann der Sinus-Modus  
anhand dieser Anleitung vollständig wiederhergestellt werden.

### Globals zurücksetzen

`sw_acc` → `initial_value: '100'`, Kommentar auf Rampenanteil % zurück.  
`sw_motor_ramp` → wieder aktiv als `initial_value: '200'`.  
Preset-acc-Globals → alte Prozentwerte: 100, 100, 100, 75, 50.

### `script_schwenker_start` + `script_motor_reboot` + `script_schwenker_goto_slot`

Alle `{ 0x84, 0x80 }` → `{ 0x84, 0x40 }` (128 → 64 Sub).

### 50ms-Interval — Sinus-Kern wiederherstellen

Den NEU-Block aus C1 ersetzen durch den ALT-Block (vollständig in C-BACKUP dokumentiert).

Kernunterschiede beim Rollback:
- `acc` aus `sw_motor_ramp` statt `sw_acc`
- Speed = `sw_max_speed_rpm * sinval` (kein × 8 Faktor)
- Flip-Trigger bei `phase_ms >= halbperiode_ms + 250U` (250ms Extra-Auslauf)

### `lvgl_overlays/schwenker.yaml` — arc_acc zurück

```
min_value: 10 / max_value: 100 / value: 100
on_value Lambda: "%d%%"
lbl_arc_acc_val text: "100%"
lbl_trapez_shape: hidden: false, on_value Lambda Trapez-Block wieder aktiv
```

---

## Reihenfolge der Durchführung

1. `schwenker.yaml` — A1 (`sw_acc` Global)
2. `schwenker.yaml` — A2 (`sw_motor_ramp` als veraltet markieren)
3. `schwenker.yaml` — A3 (Preset-acc-Globals)
4. `schwenker.yaml` — B1 (`script_schwenker_start`, MSTEP)
5. `schwenker.yaml` — B2 (`script_motor_reboot`, MSTEP)
6. `schwenker.yaml` — B3 (`script_schwenker_goto_slot`, MSTEP × 2)
7. `schwenker.yaml` — B4 (`script_schwenker_settings_open`, acc-Format)
8. `schwenker.yaml` — C1 (50ms-Interval Kern, Hauptänderung)
9. `lvgl_overlays/schwenker.yaml` — D1 (`arc_acc` Range)
10. `lvgl_overlays/schwenker.yaml` — D2 (lbl_arc_acc_val Initial-Text)
11. `lvgl_overlays/schwenker.yaml` — D3 (`lbl_trapez_shape` hidden)
12. `lvgl_overlays/schwenker.yaml` — D4 (alle 5 Preset-Buttons on_short_click + on_long_press)
13. `lvgl_overlays/schwenker.yaml` — D5 (Preset-Label Initial-Texte)
14. YAML validieren (Task „YAML validieren")
15. Flashen + testen

---

## Empfohlene erste Test-Parameter

| Parameter | Wert | Begründung |
|---|---|---|
| `sw_halbperiode_ms` | 3000 | 3s pro Richtung, wie bisher |
| `sw_max_speed_rpm` | 4 | 4 echte UPM → entspricht F5-Speed 32 |
| `sw_acc` | 200 | Sanfte Rampe, Motor braucht ~1.5s zum Hochlaufen |

Bei `acc=200` und 4 UPM:  
Rampendauer ≈ `(256-200) × 0,05ms × 128 Steps × 4UPM/60s × …` → Motor bremst locker aus  
vor Richtungswechsel bei T=3s.

Falls Richtungswechsel noch zu abrupt: `sw_halbperiode_ms` erhöhen (mehr Zeit zum Aus- und  
Anlaufen) oder `sw_acc` erhöhen (langsamere Rampe → mehr Zeit bis Vollgeschwindigkeit).
