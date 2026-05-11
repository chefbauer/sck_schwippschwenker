# MKS Servo42D – CAN-Bus Referenz

> Dieses Dokument sammelt alle verifizierten Kommandos und Konventionen für das Projekt.  
> Primärquelle: offizielle MKS Servo42D CAN-Bus Doku + empirische Tests.

---

## ⚠️ SPEED-FORMEL – IMMER LESEN VOR JEDEM RPM-EINSATZ

**mstep 16 / 32 / 64 → nativ: `speed_raw = rpm` (1:1, Firmware kompensiert intern)**  
**Außerhalb dieses Bereichs: `speed_raw = rpm × (mstep / 16)` – manuell rechnen, keine automatische Korrektur!**

| MSTEP (0x84) | Hex  | speed_raw für X RPM | Hinweis |
|---|---|---|---|
| 8            | 0x08 | X × 0,5 | manuell: außerhalb nativem Bereich |
| **16**       | **0x10** | **X** | **nativ 1:1** |
| **32**       | **0x20** | **X** | **nativ 1:1 – Firmware kompensiert** |
| **64**       | **0x40** | **X** | **nativ 1:1 – Firmware kompensiert** ← goto_slot |
| 128          | 0x80 | X × 8 | manuell: rpm × (128/16) ← Drehmodus |
| 256          | 0xFF | X × 16 | manuell: rpm × (256/16) ← Schwenkmodus |

Offizielles Doku-Zitat:
> *"The speed value is calibrated based on 16/32/64 subdivisions, and the speeds of other*  
> *subdivisions need to be calculated based on 16 subdivisions.*  
> *For example, setting speed=1200:*  
> *At 8 subdivisions, the speed is 2400 RPM*  
> *At 16/32/64 subdivisions, the speed is 1200 RPM*  
> *At 128 subdivisions, the speed is 150 RPM"*

---

## Beschleunigung (acc-Byte)

| Wert | Bedeutung |
|---|---|
| **0**   | ⚠️ KEIN Ramping – sofortiger Ruck! |
| 1–254   | Motor interpoliert Rampe intern. Kleiner = sanfter. |
| **255** | ⚠️ KEIN Ramping – sofortiger Ruck! |

**Formel für acc aus Prozent:** `acc_byte = round(pct / 100 × 254)`, clamp 1–254.

---

## Betriebsmodi (0x82)

| Byte | Modus | Verwendung |
|---|---|---|
| 0x04 | Closed Loop | Positionsfahrt (F4/F5), Schwenken (F6) |
| 0x05 | FOC (Open Loop) | Idle / Drehmodus-Option |

---

## Kommando-Tabelle

### Konfiguration

| Code | Bytes | Beschreibung |
|---|---|---|
| `0x80` | `{ 0x80, 0x00 }` | Kalibrierung |
| `0x82` | `{ 0x82, mode }` | Modus setzen (0x04=Closed Loop, 0x05=FOC) |
| `0x83` | `{ 0x83, cur_H, cur_L }` | Arbeitsstrom in mA (z.B. 1000 mA = `0x03, 0xE8`) |
| `0x84` | `{ 0x84, mstep }` | Subdivisions: 0x40=64, 0x80=128, 0xFF=256 |
| `0x89` | `{ 0x89, 0x01 }` | Interpolations-Multiplyer ON (0x00=off) |
| `0x8B` | `{ 0x8B, 0x00, can_id }` | CAN-ID ändern |
| `0x8C` | `{ 0x8C, 0x01, 0x01 }` | Respond ON / `{ 0x8C, 0, 0 }` = OFF |
| `0x92` | `{ 0x92 }` | Aktuelle Position als 0° (Zero) setzen |
| `0x9B` | `{ 0x9B, param }` | Idle-Strom: param = round(pct/10 − 1), 0=10%, 8=90% |
| `0x41` | `{ 0x41 }` | Soft-Reboot (Soll = Ist sync, ~2,5 s Hochlauf) |

### Abfragen

| Code | Bytes | Antwort |
|---|---|---|
| `0x30` | `{ 0x30 }` | Winkel: bytes[5]×256 + bytes[6], Bereich 0–16383 (= 0–360°) |
| `0x31` | `{ 0x31 }` | Position Absolute Axis (int48, 6 Bytes) |
| `0x32` | `{ 0x32 }` | RT-Speed: int16, bytes[1]<<8 \| bytes[2]; CCW>0, CW<0 |
| `0x39` | `{ 0x39 }` | Winkel-Error (int32 raw / 51200 × 360°) |
| `0xF1` | `{ 0xF1 }` | Status: 0=laufend, 1=gestoppt, 2=fertig |

### Bewegung

#### F4 – Relative Positionsfahrt (Encoder-Schritte)

```
{ 0xF4, spd_H, spd_L, acc, npr_HH, npr_H, npr_L }
```

- `steps = (16384 / 360) × degree_rel` → in int24 Two's Complement (0xFFFFFF - abs + 1 wenn negativ)
- **speed_raw gilt für mstep der aktuellen Einstellung** (0x84 vorher setzen!)
- Antwort: 0xF4 mit status=1 (start), status=2 (fertig)

#### F5 – Absolute Achsfahrt (absAxis)

```
{ 0xF5, spd_H, spd_L, acc, pos_HH, pos_H, pos_L }
```

- `absAxis`: int24_t, −8.388.607 … +8.388.607
- Motor erreicht das Ziel nie wenn |absAxis| = 8.000.000 → Endlos-Drehung (Schwenk-Trick alt)
- speed=0 = Sanftstop mit acc-Rampe

#### F6 – Motorgeschwindigkeit (Dauerlauf)

```
gen_data_can_motor_set_speed(can_id, dir, speed_raw, acc)
```

- `dir`: 0 = CCW, 1 = CW
- `speed_raw = rpm` für mstep 16/32/64 (nativ)
- `speed_raw = rpm × (mstep/16)` außerhalb — **manuell rechnen!**
- `speed_raw = 0` → Motor bremst per acc-Byte und stoppt
- **Einmalig senden** – Motor fährt selbst bis neuer Befehl kommt
- **Richtungswechsel ohne Stop = abrupter Ruck** (Firmware-Einschränkung). Immer erst stoppen, Stillstand abwarten, dann neue Richtung!

---

## Winkel-Sensor

- Abfrage: `0x30` (alle 10 ms im Projekt)
- Rohdaten: 0–16383 (immer, unabhängig von MSTEP!)
- Umrechnung: `grad = raw / 16384.0 × 360.0`
- Stillstandserkennung: Δ < 5 Raw ≈ 0,1° → Motor steht

---

## Projekt-spezifische Einstellungen

| Modus | MSTEP | speed_raw-Formel | acc-Quelle |
|---|---|---|---|
| Positionsfahrt (`goto_slot`) | 64 (0x40) | `rpm` (nativ 1:1) | 10 (fest) |
| Drehmodus | 128 (0x80) | `rpm × 8` | `dr_acc` (1–254) |
| Schwenkmodus | 256 (0xFF) | `rpm × 16` | `round(sw_acc/100 × 254)` |

**Arbeitsstrom:** 500–2000 mA (NVS: `sw_work_current_mA`)  
**Idle-Modus:** FOC (0x05) + 100 mA Arbeitsstrom + 10% Idle (~10 mA) nach 10 s Stillstand  
**CAN-Bitrate:** 500 kbps  
**CAN-ID Motor:** `motor_can_id` (Substitution in `schwippschwenker.yaml`)

---

## CRC

Alle Pakete werden mit `add_crc(can_id, { bytes... })` versehen (Implementierung in `amg8833_helper.h` oder Include).  
`gen_data_can_motor_set_speed` und `gen_data_can_motor_goto_abs_axis` sind ebenfalls dort definiert.
