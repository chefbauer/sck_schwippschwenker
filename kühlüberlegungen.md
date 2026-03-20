# Kühlüberlegungen – Optimaler Wärmetransfer

## Ausgangssituation

| Parameter | Wert |
|---|---|
| Wassertemperatur | ~2 °C |
| Getränketemperatur | ~22 °C |
| Temperaturdifferenz ΔT | **20 K** |
| Schwenker-Drehzahl | 60–80 RPM (aktuell) |

---

## Wirkmechanismen der Rotation

Es gibt **zwei verschiedene Effekte** der Rotation – und die wirken auf komplett unterschiedliche Bereiche:

### 1. Außen: Erzwungene Konvektion im Wasser (dominant!)

Die rotierende Dose/Flasche erzeugt **Turbulenz im Wasser** – das ist bei weitem der stärkste Kühleffekt.

| Zustand | konvektiver Wärmeübergangskoeffizient h |
|---|---|
| Statisch (natürliche Konvektion) | ~100–300 W/(m²·K) |
| Rotation 20–50 RPM | ~400–800 W/(m²·K) |
| Rotation 60–100 RPM | ~800–1500 W/(m²·K) |
| Rotation >150 RPM | ~1500–2500 W/(m²·K) |

→ Rotation steigert den Wärmeübergang **5–10× gegenüber statischem Eintauchen**.

### 2. Innen: Zentrifugalwirkung auf das Getränk

Ob die Rotation auch das **Getränkeinnere** durchmischt, hängt von der Zentrifugalbeschleunigung ab:

```
a_c = ω² × r
```

Für eine Standard-Dose (Radius r = 33 mm):

| RPM | ω [rad/s] | a_c [m/s²] | a_c / g | Wirkung innen |
|---|---|---|---|---|
| 20 | 2,09 | 0,14 | **0,015 g** | vernachlässigbar |
| 40 | 4,19 | 0,58 | **0,059 g** | kaum |
| 60 | 6,28 | 1,31 | **0,13 g** | gering |
| 80 | 8,38 | 2,32 | **0,24 g** | gering |
| 120 | 12,6 | 5,22 | **0,53 g** | merklich |
| **164** | **17,2** | **9,81** | **≈ 1,0 g** | Zentrifugalkraft = Schwerkraft |
| 200 | 20,9 | 14,5 | **1,5 g** | deutliche Innenzirkulation |

**Fazit:** Bei 60–80 RPM ist die Zentrifugalbeschleunigung innen nur 0,13–0,24 g.  
Das reicht **nicht** für eine aktive Durchmischung des Getränkeinneren.  
Ab ~150–160 RPM wird die Zentrifugalkraft vergleichbar mit der Schwerkraft → erst dann beginnt echte Innenzirkulation.

**Trotzdem kühlt es gut** – der Haupt效应 ist die Außenturbulenz, nicht das Innen.

---

## Abschätzung der Kühlzeit

Vereinfachtes Modell (Newton'sche Abkühlung, ΔT₀ = 20 K):

```
τ = (m × c) / (h × A)
T(t) = T_Wasser + ΔT₀ × e^(−t/τ)
```

Für eine 0,33 L Dose (Aluminiumdose):
- Masse Getränk: 0,33 kg
- Wärmekapazität: c ≈ 4000 J/(kg·K)
- Oberfläche: A ≈ 0,031 m²

| Szenario | h | τ | Zeit bis 4°C | Zeit bis 6°C |
|---|---|---|---|---|
| Statisch | 200 W/(m²·K) | ~213 s | ~6,5 min | ~5,0 min |
| 30 RPM | 500 W/(m²·K) | ~85 s | ~2,6 min | ~2,0 min |
| **60 RPM** | **1000 W/(m²·K)** | **~43 s** | **~1,3 min** | **~1,0 min** |
| **80 RPM** | **1200 W/(m²·K)** | **~35 s** | **~1,1 min** | **~0,8 min** |
| 150 RPM | 2000 W/(m²·K) | ~21 s | ~40 s | ~31 s |

> **Hinweis:** Das sind theoretische Mindestwerte ohne Wärmewiderstand der Dosenwand (Alu: ~200 W/m·K, vernachlässigbar dünne Wand). In der Praxis kommen Füllunabhängigkeiten, Wärmerückgewinnung im Wasser etc. dazu – echte Zeiten liegen 30–50 % höher.

---

## Glasflaschen – Rotation vs. Wandwiderstand

### Das Grundproblem: Glas isoliert

Bei Aludosen ist die Wand kein Engpass (λ_Alu = 200 W/m·K, Wandstärke 0,1 mm → R_Wand ≈ 0).  
Bei Glasflaschen ist die Wand ein echter Widerstand:

```
R_Wand = d / λ = 0,003 m / 1,0 W/(m·K) = 0,003 m²·K/W
```

Das entspricht einem zusätzlichen Wärmeübergangswiderstand von:

```
h_äquivalent = 1 / R_Wand = 333 W/(m²·K)
```

Das heißt: Selbst wenn außen ein perfekter Wärmeübergang wäre, limitiert das Glas auf **maximal ~333 W/(m²·K)** Gesamtwirkung.

### Gesamtwiderstand (außen + Wand in Reihe)

```
1/h_gesamt = 1/h_außen + R_Wand
```

| Szenario | h_außen | R_gesamt | h_gesamt effektiv |
|---|---|---|---|
| Statisch, kein Durchfluss | 200 | 0,003 + 0,005 | **125 W/(m²·K)** |
| 15 L/min, kein Drehen | 350 | 0,003 + 0,0029 | **168 W/(m²·K)** |
| **30 RPM** + Durchfluss | 750 | 0,003 + 0,00133 | **230 W/(m²·K)** |
| **60 RPM** + Durchfluss | 1200 | 0,003 + 0,00083 | **263 W/(m²·K)** |
| 100 RPM + Durchfluss | 1700 | 0,003 + 0,00059 | **278 W/(m²·K)** |
| ∞ RPM (theoretisch ideal) | ∞ | 0,003 | **333 W/(m²·K)** |

→ Das Glas bildet eine **harte Obergrenze** bei ~333 W/(m²·K).  
→ Ab ~60 RPM ist das Glas der dominierende Widerstand – **mehr Drehen bringt kaum noch was**.

### Kühlzeiten Glasflasche (0,5 L, Wasser 2 °C → Getränk 22 °C auf 6 °C)

Glasflasche: Masse ~0,5 kg Getränk, Oberfläche ~0,038 m², c ≈ 4000 J/(kg·K)

| Szenario | h_gesamt | τ | Kühlzeit bis 6 °C |
|---|---|---|---|
| Statisch, kein Drehen | 125 | ~420 s | **~8 min** |
| Nur Durchfluss, kein Drehen | 168 | ~313 s | **~6 min** |
| **30 RPM** + Durchfluss | **230** | **~229 s** | **~4,5 min** |
| **60 RPM** + Durchfluss | **263** | **~200 s** | **~4 min** |
| 100 RPM + Durchfluss | 278 | ~189 s | **~3,7 min** |

### Direkter Vergleich: Dose vs. Glasflasche

| | Aludose 0,33 L | Glasflasche 0,5 L |
|---|---|---|
| 60 RPM + Durchfluss | **~65 s** | **~4 min** |
| Grenzwert bei mehr RPM | ~40 s erreichbar | ~3,5 min (Glas limitiert) |
| Bottleneck | Wassergrenzschicht | **Glaswand** |

### Fazit für Glasflaschen

- Rotation hilft **deutlich** gegenüber statischem Eintauchen (8 min → 4 min bei 60 RPM)
- Aber: **Ab 30–60 RPM ist das Glas der Flaschenhals** – mehr RPM bringt kaum Gewinn
- Bei Glasflaschen ist die **Kühlzeit strukturell ~3–4× länger** als bei Aludosen
- Willst du Glasflaschen schneller kühlen → mehr Durchfluss, kälteres Wasser oder länger warten. Mehr RPM hilft hier weniger als bei Dosen.

---

## Dünne Edelstahl-Trinkflasche

### Wandwiderstand Edelstahl

Typische Edelstahl-Trinkflasche (einfachw andig): λ ≈ 16 W/(m·K), Wandstärke ~0,5–0,8 mm

```
R_Wand = 0,0006 m / 16 W/(m·K) = 0,0000375 m²·K/W
h_äquivalent = 1 / R_Wand ≈ 26 700 W/(m²·K)
```

→ Praktisch **vernachlässigbar** – wie Aluminium. Die Edelstahlwand ist **kein Engpass**.

### Rangfolge Wandwiderstand

| Material | h_äquivalent Wand | Limitierend? |
|---|---|---|
| Aluminium (Dose) | ~2 000 000 W/(m²·K) | Nein |
| **Edelstahl dünn** | **~26 700 W/(m²·K)** | **Nein** |
| Glas | ~333 W/(m²·K) | **Ja, ab 30 RPM** |
| PET | ~667 W/(m²·K) | Ja, ab 60 RPM |

### Kühlverhalten Edelstahlflasche (0,5 L, einfachwandig)

Verhält sich fast identisch zur Aludose – die Rotation hilft voll:

| Szenario | h_gesamt | Kühlzeit bis 6 °C |
|---|---|---|
| Statisch, kein Durchfluss | ~200 W/(m²·K) | ~8 min |
| Nur Durchfluss | ~350 W/(m²·K) | ~4,5 min |
| **30 RPM** + Durchfluss | **~750 W/(m²·K)** | **~2,1 min** |
| **60 RPM** + Durchfluss | **~1200 W/(m²·K)** | **~1,3 min** |

(Länger als Dose hauptsächlich wegen größerem Volumen 0,5 L vs. 0,33 L)

### ⚠️ Kritischer Unterschied: Doppelwandig / Vakuum-isoliert?

| Typ | Kühlbar? |
|---|---|
| Einfachwandig (dünn, klingt metallisch) | ✅ Gut kühlbar, wie oben |
| Doppelwandig mit Luftspalt | ⚠️ Sehr schlecht – Luftschicht isoliert stark |
| **Vakuum-isoliert** (z. B. Thermosflasche) | ❌ **Praktisch nicht kühlbar** – Vakuum = perfekter Isolator |

**Erkennungszeichen:** Thermoflaschen halten Kaffee 8 h warm → sie halten auch Wärme draußen – das Wasser kommt nicht durch.  
Eine echte Thermos-Trinkflasche würde im Becken kaum kühlen, egal wie schnell gedreht wird.

### Fazit

Dünne einfachwandige Edelstahlflasche → **wie Aludose**, Rotation hilft voll, ~1–2 min bei 60 RPM.  
Doppelwandige/Thermoflasche → **Finger weg**, das System kann da nichts ausrichten.

---

## Dosen vs. Flaschen – Materialeffekt

Die **Wandwärmeleitfähigkeit** spielt bei Kunststoffflaschen eine wichtige Rolle:

| Material | λ [W/(m·K)] | Wandstärke | Wärmewiderstand R_Wand |
|---|---|---|---|
| Aluminium (Dose) | ~200 | ~0,1 mm | **~0,0000005 m²·K/W** → vernachlässigbar |
| Glas (Flasche) | ~1,0 | ~3 mm | **~0,003 m²·K/W** → spürbar |
| PET (Plastikflasche) | ~0,2 | ~0,3 mm | **~0,0015 m²·K/W** → vergleichbar mit Glas |

→ **Aluminiumdosen kühlen deutlich schneller** als Glasflaschen oder PET-Flaschen, weil der Wandwiderstand dort nicht limitierend ist.  
→ Bei Glasflaschen bringt mehr Rotation weniger Zusatznutzen, da die Wand selbst der Flaschenhals ist.

---

## Empfehlungen & Optimierungspotenzial

### Aktuelle 60–80 RPM: schon sehr gut für den Außeneffekt

Der Sprung von 0 → 60 RPM ist riesig (3–5× schneller).  
Der Sprung von 60 → 150 RPM bringt noch ~40 % Verbesserung.  
Der Sprung von 150 → 300 RPM bringt noch ~20 % mehr.

**Diminishing returns** setzen stark ab ~100 RPM ein.

### Ab wann lohnen sich höhere Drehzahlen?

| Ziel | Empfohlene RPM |
|---|---|
| Maximaler Außeneffekt ohne Innenmix | 60–100 RPM |
| Merkliche Innenzirkulation (Getränk) | ab ~120 RPM |
| Echte Zentrifugalwirkung innen | ab ~160 RPM |
| Physikalisches Maximum (Turbulenzplateau) | ~200–300 RPM |

### Wassertemperatur ist der größte Hebel

ΔT hat **linearen Einfluss** auf die Kühlleistung:

| Wassertemp. | ΔT | rel. Kühlleistung |
|---|---|---|
| 10 °C | 12 K | 60 % |
| 5 °C | 17 K | 85 % |
| **2 °C** | **20 K** | **100 %** |
| 0 °C (Eiswasser) | 22 K | 110 % |

→ Mit Eiswürfeln oder Sole (Salzwasser, Gefrierpunkt ~−2 °C) ließe sich die Kühlzeit nochmals um ~10–15 % verkürzen.

### Sinus-Pendelbewegung vs. kontinuierliche Rotation

Das aktuelle Pendelschema (Hin und Her) ist **nahezu gleichwertig** zur kontinuierlichen Rotation:
- Turbulenz entsteht durch die Relativbewegung Dose↔Wasser
- Richtungswechsel erzeugt kurze Turbulenzmaxima
- Ggf. sogar leichter Vorteil: kein "mitgedrehtes" Wasservolumen, das sich mit der Dose dreht

---

## Präzise Geometrie-Analyse (reale Maße)

### Becken-Layout

```
Mittlerer Kreisdurchmesser:    155 mm  (r_mittel = 77,5 mm)
Getränkedurchmesser:           67 mm   (r_Dose = 33,5 mm)
Abstand radial nach innen:     10 mm
Abstand radial nach außen:     5 mm
Abstand tangential (Spalt):    ~10 mm  (≤1 cm)

Innenring-Radius:   77,5 − 33,5 − 10 = 34 mm  → ⌀ 68 mm
Außenring-Radius:   77,5 + 33,5 +  5 = 116 mm → ⌀ 232 mm
Kanalbreite radial: 116 − 34 = 82 mm
```

Umfang auf Mittelkreis: 2π × 77,5 = 487 mm  
6 Dosen belegen: 6 × 67 = 402 mm  
Restumfang: 85 mm / 6 = **~14 mm Spalt** (tangential gemessen auf Mittelkreis)  
→ User-Angabe 10 mm ist der engste Punkt (Außenseite der Dosen, größerem r)

### Geschwindigkeit an der Dosenoberfläche

```
v_Dose = ω × r_mittel

30 RPM → ω = 3,14 rad/s → v = 3,14 × 0,0775 = 0,243 m/s
60 RPM → ω = 6,28 rad/s → v = 6,28 × 0,0775 = 0,487 m/s
80 RPM → ω = 8,38 rad/s → v = 8,38 × 0,0775 = 0,650 m/s
```

### Spalt-Reynolds-Zahl (kritisch für Turbulenz)

Der Spalt zwischen zwei Dosen beträgt **10 mm**. Re im Spalt:

```
Re = v_Dose × d_Spalt / ν     (ν = 10⁻⁶ m²/s)
```

| RPM | v_Dose | Re_Spalt | Regime |
|---|---|---|---|
| 30 RPM | 0,243 m/s | **2 430** | laminar ⚠️ |
| **60 RPM** | **0,487 m/s** | **4 870** | **Transition → turbulent ✓** |
| **80 RPM** | **0,650 m/s** | **6 500** | **turbulent ✓** |
| 100 RPM | 0,812 m/s | 8 120 | klar turbulent |

> **Kritische Erkenntnis:** Deine 10 mm-Spalten setzen turbulente Strömung erst ab ~55 RPM ein.  
> Bei 30 RPM ist der Spalt noch **laminar** – kaum Wärmeübergang zwischen den Dosen!  
> **60 RPM ist nicht zufällig gut – es ist genau der Übergang zur Turbulenz.**

### Stokes-Schicht vs. Spaltbreite

```
δ = √(2ν / ω_Pendel) = √(2×10⁻⁶ / (π/T_half))

T_half = 3 s → δ = 1,38 mm   (beidseitig: 2 × 1,38 = 2,76 mm)
T_half = 1 s → δ = 0,80 mm
```

Spalt 10 mm, mitdrehende Schicht: 2 × 1,38 = 2,76 mm  
**→ 7,2 mm des Spalts sind quasi-statisches Wasser**  
→ Sehr gute Voraussetzung: Das Bulk-Wasser dreht kaum mit.

### Wasservolumen-Analyse

```
Gesamtvolumen Ring (h = 15 cm):
V_Ring    = π × (0,116² − 0,034²) × 0,15 = 5,94 × 0,15 ≈ 5,5 L

Volumen 6 Dosen:
V_Dosen   = 6 × π × 0,0335² × 0,15 ≈ 0,317 L

Effektives Wasservolumen: ~5,2 L
```

Das Verhältnis Wasservolumen/Dosenoberfläche ist mit **~5,2 L auf 6 × 0,031 m² = 0,19 m²** sehr gut.  
Jeder Liter Wasser hat nur ~36 cm² Dose zum Wärmetransfer → sehr kompakt, sehr effizient.

### Fazit: Deine Geometrie ist fast optimal

| Eigenschaft | Bewertung |
|---|---|
| 10 mm Spalt | ⚠️ Nur ab ~55 RPM turbulent – unter diesem Wert kaum Effekt |
| Enges Becken | ✅ Wenig Wasser = wenig thermische Masse = Wasser erwärmt sich schnell |
| Durchfluss 15 L/min | ✅ Erneuert Wasser schneller als es sich erwärmt |
| Radiale Freiräume (5/10 mm) | ✅ Zusätzliche laminare Grenzschichten, aber klein |
| 6 Dosen als Paddel | ✅ Füllen fast den ganzen Umfang → wenig Bypass-Wasser |

**Praktische Empfehlung für deine Geometrie:**  
- Unter 55 RPM → kaum Turbulenz im Spalt, Rotation bringt wenig  
- **60–80 RPM → Spalt turbulent, optimale Zone**  
- Kürzere Halbperiode (1,5 s statt 3 s) → weniger Co-Rotation, mehr Richtungswechsel  
- Über 100 RPM → Co-Rotation baut sich auf, Spalt-Re steigt kaum weiter (diminishing returns)

---

## Strategie: Aktive Turbulenz durch frühen Richtungswechsel

### Die richtige Erkenntnis

Co-rotierendes Wasser = nutzlos. Das Wasser überlauft ruhig – schön, aber thermisch wertlos.  
**Ziel: Das Wasser muss ständig gegen die Dosenbewegung stehen.**

Das Mittel: **Richtung wechseln bevor das Wasser mitschwimmt.**

### Ekman-Zeit als Taktgeber

```
τ_Ekman ≈ 26 s   (80 RPM, h=15cm)

Co-Rotation nach Zeit t: C(t) = 1 − e^(−t / τ_Ekman)

Nach  5 s: C ≈ 17 %   → v_relativ noch ~83 % ✓
Nach 10 s: C ≈ 32 %   → v_relativ noch ~68 % ⚠️
Nach 15 s: C ≈ 44 %   → v_relativ noch ~56 % ✗
Nach 26 s: C ≈ 63 %   → fast sinnlos
```

**Optimales Umschaltzeitfenster: 5–7 Sekunden nach jedem Start.**

### Was beim Richtungswechsel passiert (das ist das Gute)

Beim Wechsel dreht das Wasser noch in Richtung A, die Dose dreht schon Richtung B:

```
v_relativ_Wechsel = v_Dose + v_Wasser_noch_alt
                  = v_Dose × (1 + C(t_half))

Bei t_half = 15s, C ≈ 44 %:
v_relativ_Wechsel = 1,44 × v_Dose → kurz 44% mehr als Nennwert!

Bei t_half = 5s, C ≈ 17 %:
v_relativ_Wechsel = 1,17 × v_Dose, aber 3× öfter!
```

5s-Halbperiode erzeugt **3× mehr Turbulenzereignisse** pro Minute, mit höherer Summen-Turbulenz.

### Optimale Profileform: Trapez statt Sinus

Sinus-Profil: sanfte Beschleunigung → verbringt viel Zeit bei niedriger Geschwindigkeit.  
**Trapez-Profil:** sofort auf Maximum → halten → hart umschalten.

```
Sinus:    0 → max → 0 → max → 0   (langsam auf/ab)
Trapez:   0 → MAX → MAX → 0 → -MAX → -MAX → 0
```

Das Trapez-Profil hält **konstant maximale Relativgeschwindigkeit** und dreht hart um.  
Der Motor-acc-Parameter macht dabei die eigentliche Rampe – kurz, aber kontrolliert.

### Konkrete Empfehlung für dein System

| Parameter | Bisher (Sinus) | Optimiert |
|---|---|---|
| `sw_halbperiode_ms` | 15 000 ms | **4 000–6 000 ms** |
| `sw_max_speed_rpm` | 80 RPM | 60–80 RPM (gleich) |
| `sw_acc` | 200 | **200–250** (harter Anlauf) |
| Sinus-Profil | ja | idealerweise Trapez/Rechteck |
| Richtungswechsel/min | 4 | **10–15** |

**Damit bleibt Co-Rotation immer unter ~17 %** – der Zulauf-Reset tut dann den Rest.

### Waschmaschinen-Vergleich

Das ist exakt das Prinzip moderner Waschmaschinentrommeln:
- Kurze Schwenks (3–5 s), nicht lange Umdrehungen
- Maximale Beschleunigung beim Wechsel
- Wäsche (hier: Dose) bewegt sich relativ zum Wasser (hier: Lauge)

Industrielle Schnellkühlsysteme (z. B. Beverage Cooler für Messeküchen) arbeiten nach demselben Prinzip – typisch 3–8 s Zyklen.

### Im YAML ändern

Nur `sw_halbperiode_ms` anpassen – das Profil läuft ohne weitere Änderung:

```yaml
# Empfohlene Testsequenz (im Laufen per Slider einstellbar):
# 15000 ms → Baseline (aktuell)
#  6000 ms → wenig Co-Rotation, gut
#  4000 ms → optimale Turbulenz
#  3000 ms → sehr aggressiv, Motor prüfen
```

Wenn der Slider für `sw_halbperiode_ms` noch nicht vorhanden ist → einfach im Tab "Schwenker" ergänzen.

---

## 1g Co-Rotation: Die Falle

### Die Intuition: „Zentrifugalkraft im Wasser = gut"

Wenn das gesamte Wasser mit 1g Zentrifugalkraft rotiert, klingt das nach maximaler Turbulenz.  
Tatsächlich ist es das **Gegenteil** – zumindest für dieses System.

### Das Grundprinzip des Wärmeübergangs

Wärmeübergang an der Dosenoberfläche hängt **ausschließlich von der Relativgeschwindigkeit** zwischen Dose und angrenzendem Wasser ab:

```
h ~ (v_Dose − v_Wasser)^0,7
```

Wenn Wasser und Dose **gleich schnell drehen**:
```
v_relativ = 0  →  h → natürliche Konvektion ≈ 100–200 W/(m²·K)
```

Das ist **schlechter als statisch eintauchen mit Durchfluss** (350 W/(m²·K)).

### 1g Co-Rotation wäre das schlechteste Szenario

| Zustand | v_relativ | Wärmeübergang |
|---|---|---|
| Statisch, kein Durchfluss | 0 | ~200 W/(m²·K) |
| Statisch + 15 L/min | 4 mm/s | ~350 W/(m²·K) |
| 80 RPM, 22% Co-Rotation | ~78% v_Dose | **~1200 W/(m²·K)** |
| **80 RPM, 100% Co-Rotation** | **0** | **~200 W/(m²·K)** ← wie statisch! |

→ Selbst mit 1g Zentrifugalkraft im Wasser: wenn Wasser = Dose, kein Wärmeübergang.

### Wann wäre 1g Zentrifugation sinnvoll?

Nur in einem **echten Zentrifugal-Kühlsystem** – da dreht die Dose **innerhalb** des Behälters,  
das Kühlwasser ist **getrennt** außen und wird von der Zentrifugalkraft entlang der Wand gepresst:

```
Zentrifuge-Konzept:
   [Kaltes Wasser, außen, rotiert mit]
   [Dosenoberfläche – Grenze]
   [Getränk innen – dreht mit]
```

Dort drückt die 1g-Kraft das kalte Wasser gegen die Außenwand der Dose → erzwungene  
Konvektion durch Dichteunterschied + radiale Druckdifferenz.

**In deinem offenen Donut-Becken** funktioniert das nicht – Dose und Wasser sind direkt kontakt,  
keine Trennebene, alles dreht gemeinsam → keine Relativströmung.

### Fazit

Das Ziel ist immer **maximale Relativgeschwindigkeit** zwischen Dose und Wasser –  
nicht maximale Absolutgeschwindigkeit des Wassers.

**1g im Wasser bei Co-Rotation = maximaler Misserfolg.**  
**1g nur an der Dosenoberfläche bei stehendem Wasser = maximaler Erfolg** – das wären ~164 RPM.

Dein aktuelles System mit ~78 % Relativgeschwindigkeit und 15 L/min ist physikalisch nah am  
Optimum für diesen Aufbau – du bist auf dem richtigen Weg.

---

## Kontinuierliche Rotation vs. Pendel – direkter Vergleich

### Ausgangslage: Beide unterhalb des Turbulenz-Schwellwerts

Am Spalt (10 mm) wird Turbulenz erst ab ~55 RPM erreicht.  
Bei < 30 RPM sind **beide Betriebsarten im laminaren Bereich** des Spalts.

Die Frage ist daher: Welcher Modus erzeugt mehr **effektive Relativgeschwindigkeit** über die Zeit?

### Kontinuierliche Rotation (< 30 RPM)

```
Phase 1 (0–10 s): Wasser steht, Dose dreht → v_relativ = 100% → gut
Phase 2 (10–26 s): Wasser dreht mit → v_relativ sinkt auf ~20% → schlecht
Dauerbetrieb:     Co-Rotation quasi-stationär, nur Zulauf bremst sie
```

Ohne Durchfluss: Nach ~26 s (τ_Ekman) dreht das Wasser fast mit – Rotation ist nutzlos.  
Mit 15 L/min: Co-Rotation stabilisiert sich bei ~22 % → v_relativ dauerhaft ~78 %  
Mit 30 L/min: Austausch τ = 5,2 / 0,5 = 10,4 s → Co-Rotation ~10 % → v_relativ ~90 %

### Pendelbewegung (< 30 RPM peak, 15 s Halbperiode)

```
Phase 0–5 s:   Dose beschleunigt, Wasser beginnt mitzudrehen
Phase 5–10 s:  Nähe RPM-Maximum, Wasser ≈ 30–40% Co-Rotation
Phase 10–15 s: Dose bremst ab, dreht um → Wasser dreht noch vorwärts
Richtungswechsel: v_relativ kurz = v_Dose + v_Wasser (addiert!) → bis 140% ✓
```

Besonderer Vorteil: Der Richtungswechsel **addiert Dosen- und Wassergeschwindigkeit** für kurze Zeit.

### Gegenüberstellung (< 30 RPM, mit 15 L/min)

| | Kontinuierliche Rotation | Pendel 15 s |
|---|---|---|
| Durchschnittliche v_relativ | ~78 % dauernd | ~60 % normal + kurze ~140 %-Bursts |
| Turbulenz-Charakter | schwach-laminar, konstant | laminar + kurze turbulente Bursts |
| Strömung im Spalt | Re ~1900, laminar | Re ~1900, aber an Wechseln kurz ×2 |
| Wärmeübergang Mittelwert | niedrig aber konstant | etwas ungleichmäßiger, Spitzen besser |
| **Gesamteindruck** | **leicht besser** im Mittel | **Spitzenwerte höher**, Bursts wichtig |

**Unter 55 RPM sind beide Betriebsarten ähnlich effektiv – der Durchfluss dominiert.**

### Was wirklich den Unterschied macht: 30 L/min

```
15 L/min: Austausch pro 15s-Zyklus ≈ 51 % → Co-Rotation ~22 %
30 L/min: Austausch pro 15s-Zyklus ≈ 76 % → Co-Rotation ~10 %
```

| | 15 L/min | 30 L/min | Verbesserung |
|---|---|---|---|
| Co-Rotation | ~22 % | ~10 % | −55 % |
| v_relativ effektiv | ~78 % | ~90 % | +15 % |
| Wärmeübergang (laminar) | Basis | **+20–25 %** | spürbar |
| Kühlzeit Dose auf 4 °C | ~65 s | **~52 s** | −13 s |

**30 L/min bringt bei gleicher Drehzahl mehr als der Unterschied zwischen Pendel und Dauerrotation.**

### Empfehlung

| Frage | Antwort |
|---|---|
| Pendel vs. Dauerrotation bei < 30 RPM? | Nahezu gleich, leichter Vorteil Dauerrotation im Langzeitmittel |
| Was bringt am meisten unter 55 RPM? | **Mehr Durchfluss** – das ist der dominante Hebel |
| Lohnt sich 30 L/min? | **Ja** – ~10–15 % schnellere Kühlung, günstiger als mehr RPM |
| Ab wann lohnt wieder mehr RPM? | Ab 55 RPM (Turbulenz-Schwelle) – dann wäre 60+ RPM + 30 L/min das Optimum |

---

## Reale Betriebsparameter: 80 RPM / 15 s Halbperiode

### Was bedeutet 15 s Halbperiode für Co-Rotation?

Bei 15 s hat das Wasser lang Zeit mitzudrehen. Die Stokes-Schicht-Analyse (δ ≈ 1,4 mm) gilt nur für schnelle Oszillationen – bei 15 s arbeiten wir im quasi-stationären Bereich.

Relevante Kennzahl ist die **Ekman-Anlaufzeit** (wie schnell dreht sich ein Wasservolumen in einem rotierenden System hoch):

```
τ_Ekman = h / (2 × √(ν × ω))

h = 0,15 m (Wassertiefe), ν = 10⁻⁶ m²/s
80 RPM → ω = 8,38 rad/s

τ_Ekman = 0,15 / (2 × √(10⁻⁶ × 8,38)) ≈ 26 s
```

→ Das Wasser würde ~26 s brauchen um vollständig mitzudrehen.  
→ Bei 15 s Halbperiode: Das Wasser erreicht **~40–50 % der vollen Co-Rotation** bevor der Richtungswechsel kommt.  
→ Das ist der Strudel den du siehst – echte, signifikante Mitrotation.

### Das rettendes Gegengewicht: Der statische Zulauf

Hier kommt der entscheidende Faktor ins Spiel:

```
Zulaufrate:         15 L/min = 0,25 L/s
Wasservolumen:      ~5,2 L
Zeitkonstante Austausch: V/Q = 5,2 / 0,25 = 20,8 s
```

In **einer Halbperiode (15 s)** werden ausgetauscht:
```
Anteil neu = 1 − e^(−15 / 20,8) = 1 − e^(−0,72) ≈ 51 %
```

→ **In jedem 15s-Halbzyklus wird ~die Hälfte des Beckenwassers durch frisches, nicht-rotierendes Wasser ersetzt.**

Das frisch einströmende Wasser hat **null Drehimpuls** (kommt aus einem fixen Rohr unten).  
Es verdünnt die Mitrotation ständig:

```
Effektive Co-Rotation ≈ 45% (Ekman) × (1 − 51% Austausch) ≈ ~22 % der Dosengeschwindigkeit
```

| | Ohne Durchfluss | Mit 15 L/min |
|---|---|---|
| Co-Rotation nach 15 s | ~45 % | **~22 %** |
| Effektive v_relativ | ~55 % von v_Dose | **~78 % von v_Dose** |
| Entspricht bei 80 RPM | wie ~44 RPM | **wie ~62 RPM effektiv** |

**Der Durchfluss rettet den Großteil der Rotationswirkung** – ohne ihn wäre 15 s Halbperiode bei 80 RPM deutlich ineffizienter.

### Ideale Halbperiode für dein Setup

```
Sweet Spot ohne Durchfluss: T_half ≈ τ_Ekman / 2 = ~13 s
Mit Durchfluss-Korrektur:   10–15 s → bereits gut
```

→ Deine **15 s sind sehr gut getroffen** – du bist genau im Sweet Spot.

### Die drei Effekte im Zusammenspiel

```
1. Spalt-Turbulenz:  80 RPM × 10mm Spalt → Re ~6500 → turbulent ✓
2. Zulauf-Reset:     51% Wasseraustausch pro Zyklus → Co-Rotation gekappt ✓
3. Richtungswechsel: Restliche ~22% Mitrotation kehren um → kurzer Turbulenzburst ✓
```

Die **15 L/min sind der heimliche Held** – sie ersetzen nicht nur warmes mit kaltem Wasser,  
sie bremsen auch aktiv die Mitrotation und erlauben erst, dass 15 s Halbperiode so gut funktioniert.

---

## Sinus-Pendel vs. Co-Rotation: Wie viel hilft es wirklich?

### Die konkrete Geometrie

```
Innenring:      r_i = 7 cm
Außenring:      r_a = 23 cm
Kanalbreite:    16 cm
Mittlerer r:    15 cm

6 Dosen à 67 mm Durchmesser
Umfang bei r=15cm: 2π × 15 = 94,2 cm
6 × 6,7 cm = 40,2 cm belegt
Lücke tangential: (94,2 − 40,2) / 6 ≈ 9 cm zwischen je zwei Dosen
Lücke radial: (16 − 6,7) / 2 ≈ 4,65 cm je Seite
```

### Das Stokes-Schicht-Argument: Bulk-Wasser dreht kaum mit

Bei **oszillierender** (nicht stationärer) Rotation gilt: Wie tief dringt die Bewegung überhaupt ins Wasser ein?

```
Stokes-Schichtdicke: δ = √(2ν / ω_Pendel)

ν = 10⁻⁶ m²/s (Wasser bei 2°C)
ω_Pendel = π / T_Halbperiode = π / 3 s ≈ 1,05 rad/s

δ = √(2 × 10⁻⁶ / 1,05) ≈ 1,4 mm
```

**Das Bulk-Wasser das mehr als ~2 mm von der Dosenoberfläche entfernt ist, „sieht" die Pendelbewegung kaum.**  
Es ist zu träge für die Änderungsrate der Oszillation – es bleibt annähernd stehen.

Das klingt schlimm, ist aber **gut**: Die Relativgeschwindigkeit zwischen Dose (bewegt) und Wasser (fast stationär) bleibt hoch.

### Das Spalt-Jet-Argument: Turbulenz zwischen den Dosen

9 cm Lücke zwischen je zwei Dosen. Wenn die Dosen sich bewegen, wird Wasser durch diese Engstellen gepresst → **Spaltströmung mit hoher Geschwindigkeit**.

```
Bei 60 RPM, v_Dose = 207 mm/s an r=15cm:
Der 9cm-Spalt verengt die Strömung relativ zur Dosen-Anordnung.
Re im Spalt ≈ v × Spalt / ν = 0,207 × 0,09 / 10⁻⁶ ≈ 18 600 → turbulent
```

Hinter jeder Dose entsteht ein **turbulentes Kielwasser** (wake), das auf die nächste Dose trifft. Das ist ähnlich wie bei einem Pin-Fin-Wärmetauscher – eine sehr effektive Geometrie.

### Rotor-Impuls-Argument: Kann die Sinus-Pendelbewegung die 22 L Wasser überhaupt beschleunigen?

Schätze das Wasservolumen im Ring (h ≈ 15 cm):
```
V = π × (0,23² − 0,07²) × 0,15 ≈ 22 L = 22 kg
Massenträgheitsmoment: I ≈ 22 × 0,15² ≈ 0,5 kg·m²
```

Bei Sinus-Profil, T_half = 3 s, ω_peak = 7,33 rad/s:
```
α_peak = π × ω_peak / T_half ≈ 7,7 rad/s²
Benötigtes Drehmoment für Gesamtwasser: τ = I × α ≈ 0,5 × 7,7 ≈ 3,9 N·m
```

Die 6 Dosen als „Paddel" im Ring können dieses Drehmoment bei 9 cm Spalt vermutlich **nicht vollständig** übertragen → das Bulk-Wasser bleibt weiter zurück → **mehr Relativgeschwindigkeit** → gut.

### Zusammenfassung: Ja, es wirkt – aber anders als gedacht

| Intuition | Realität |
|---|---|
| „Wasser dreht mit → nutzlos" | Stokes-Schicht: Bulk dreht bei 3s-Periode kaum mit |
| „Sinus ist weich → wenig Turbulenz" | Spalt-Jets zwischen Dosen erzeugen Re ~18 000 → turbulent |
| „Richtungswechsel bringt Turbulenz" | Ja – aber auch der gesamte Zyklus dazwischen ist effektiv |
| „Mehr RPM immer besser" | Ab ~70 RPM baut sich Co-Rotation auf; Strudel ist Zeichen dafür |

### Was wirklich optimieren würde: Kürzere Halbperiode

Die Stokes-Schicht hängt von **ω_Pendel** ab (der Pendelfrequenz, nicht der Drehgeschwindigkeit):

```
T_half = 1 s → δ = √(2×10⁻⁶ / π×1) ≈ 0,8 mm  → noch weniger Mitrotation
T_half = 3 s → δ = 1,4 mm
T_half = 5 s → δ = 1,8 mm
```

Kürzere Halbperiode = weniger Zeit fürs Wasser mitzudrehen + mehr Richtungswechsel pro Minute = mehr turbulente Ereignisse.

**Empfehlung:** Statt 3 s Halbperiode → 1,5–2 s testen, gleiche RPM. Weniger Strudel, mehr effektive Turbulenz.

Bei deiner Geometrie (enger Donut, 6 Dosen als Paddel) ist mittlere Drehzahl + **hohe Pendelfrequenz** wahrscheinlich besser als hohe Drehzahl + langsames Pendeln.

---

## Strudel-Effekt ab 70–80 RPM (Donut-Becken, Wasser sinkt innen)

### Was passiert physikalisch?

Der sinkende Innenspiegel ist ein **Freiflächenwirbel** (Rankine-Vortex):  
Die Rotation der Flaschen/Dosen überträgt Impuls ans Wasser → das Wasser beginnt mitzudrehen → Zentrifugalkraft drückt Wasser nach außen → Spiegel sinkt innen, steigt außen.

```
Δh = ω² × r² / (2g)

Bei 75 RPM und Donut-Innenradius 8 cm:
ω = 7,85 rad/s → Δh ≈ 2,5 cm Höhenunterschied
```

### Ist das schlecht für die Kühlung?

**Jein – es hat zwei gegenläufige Effekte:**

#### Negativer Effekt: Co-Rotation reduziert Relativgeschwindigkeit

Das entscheidende für die Wärmeübertragung ist **nicht** die absolute Drehzahl der Dose,  
sondern die **Relativgeschwindigkeit zwischen Dose und Wasser**:

```
v_relativ = v_Dose − v_Wasser
```

Wenn das Wasser mitdreht, reduziert sich v_relativ → weniger Turbulenz an der Oberfläche → schlechterer Wärmeübergang.

| Szenario | v_Dose | v_Wasser dreht mit | v_relativ | Wirkung |
|---|---|---|---|---|
| 60 RPM, kein Mitdrehen | 207 mm/s | 0 mm/s | 207 mm/s | optimal |
| 75 RPM, 50% Mitrotation | 262 mm/s | 131 mm/s | **131 mm/s** | ~wie 40 RPM! |
| 75 RPM, 80% Mitrotation | 262 mm/s | 210 mm/s | **52 mm/s** | ~wie 15 RPM! |

→ **Starke Mitrotation kann den Effektivwert auf das Niveau von 20–40 RPM drücken** – trotz höherer Sollgeschwindigkeit.

#### Positiver Effekt: Toroidale Sekundärströmung

Das Wasserspiegelgefälle erzeugt gleichzeitig eine **toroidale Zirkulationsströmung**:
- Außen: Wasser steigt hoch (dort kommt auch das kalte Einlasswasser rein → gut)
- Innen: Wasser sinkt ab
- Boden: Wasser strömt von innen nach außen

Diese Zirkulation erneuert ständig das Wasser an den Dosen – ein positiver Effekt.

### Pendel-System vs. Dauerrotation

Du hast ein **Pendel** (Hin- und Herbewegung mit Sinus-Profil), kein Dauerrotation.  
Das hilft erheblich!

| | Dauerrotation | Pendel (dein System) |
|---|---|---|
| Mitrotation | baut sich kontinuierlich auf | wird bei **jedem Richtungswechsel aufgebrochen** |
| Strudel | stabil und stark | instabil, kollabiert an Richtungswechsel |
| Effektive v_relativ | stark reduziert | viel besser erhalten |

**Dein Pendelsystem schlägt kontinuierliche Rotation bei gleicher Spitzen-RPM.**  
Der Richtungswechsel zerstört den Strudel regelmäßig – das Wasser bremst, die Dose dreht schon rückwärts, maximaler Schlupf genau beim Wechsel.

### Optimaler RPM-Bereich für dein Donut-Becken

| < 50 RPM | kein nennenswerter Strudel, volle Relativgeschwindigkeit, weniger Turbulenz |
| **50–80 RPM** | **leichter Strudel, aber Pendel bricht ihn auf → Sweet Spot** |
| > 100 RPM | starke Mitrotation, Wasserring dreht fast mit → Relativvorteil sinkt |

### Empfehlung

**60–80 RPM ist tatsächlich dein optimaler Bereich** – nicht trotz des Strudels, sondern weil:
1. Das Pendel bricht die Mitrotation bei jedem Umkehrpunkt auf
2. Die toroidale Zirkulation bringt frisches Kaltwasser aus dem Einlass nach oben
3. Über 90–100 RPM frisst die Mitrotation den Vorteil wieder auf

Beobachtung als einfachen Test: Wenn der Strudel nach dem Richtungswechsel **schnell zusammenbricht** (< 1–2 s), ist das Pendel effektiv. Bleibt er lange stehen, ist die Halbperiode zu lang → `sw_halbperiode_ms` verkürzen.

---

## 30 RPM vs. 60 RPM – konkreter Vergleich

### Oberflächengeschwindigkeit & Turbulenz

| | 30 RPM | 60 RPM | Faktor |
|---|---|---|---|
| v_Oberfläche (r=33mm) | 104 mm/s | 207 mm/s | 2× |
| Reynolds-Zahl | ~6 900 | ~13 600 | 2× |

Beide sind turbulent (Re > 4000), aber unterschiedlich stark.

### Wärmeübergangskoeffizient skaliert mit Re^0,7

Für turbulente Umströmung rotierender Zylinder gilt näherungsweise:

$$h \propto \text{Re}^{0{,}7}$$

$$\frac{h_{60}}{h_{30}} = \left(\frac{13600}{6900}\right)^{0{,}7} = 1{,}98^{0{,}7} \approx \mathbf{1{,}62}$$

→ 60 RPM überträgt Wärme **~62 % effizienter** als 30 RPM.

### Kühlzeit (mit Durchfluss 15 L/min)

| Szenario | h ca. | Kühlzeit 0,33 L auf 4 °C |
|---|---|---|
| Kein Drehen, nur Durchfluss | ~350 W/(m²·K) | ~3,5 min |
| **30 RPM** + Durchfluss | **~750 W/(m²·K)** | **~1,7 min (~100 s)** |
| **60 RPM** + Durchfluss | **~1200 W/(m²·K)** | **~1,1 min (~65 s)** |
| 100 RPM + Durchfluss | ~1700 W/(m²·K) | ~45 s |

→ **30 RPM spart gegenüber kein Drehen ~55 %** der Zeit.  
→ **60 RPM spart gegenüber 30 RPM nochmals ~35 s** (~35 %).  
→ Der Sprung von 60 → 100 RPM bringt nochmals ~20 s.

### Fazit: Lohnt sich der Unterschied?

| Vergleich | Zeitersparnis | Aufwand für Motor |
|---|---|---|
| 0 → 30 RPM | −2 min | gering |
| 30 → 60 RPM | −35 s | moderat |
| 60 → 100 RPM | −20 s | höher (Wasserwiderstand!) |

**30 RPM ist bereits sehr gut.** 60 RPM bringt spürbar mehr – gut 30 Sekunden weniger pro Dose.  
Für Alltagsbetrieb ist **60–80 RPM der Sweet Spot**: deutlicher Vorteil gegenüber 30 RPM, ohne den Motor übermäßig zu belasten.

---

## Einfluss des Durchflusssystems (Überlaufbecken, 15 L/min)

### Wie schnell fließt das Wasser vorbei?

Das Überlaufbecken hat kaltes Wasser unten rein, warmes oben raus.  
Angenommene Beckengrundfläche: ~30 × 20 cm = 0,06 m²

```
v_Wasser = Q / A = (15 L/min ÷ 60) / 0,06 m² = 0,00025 m³/s / 0,06 m² ≈ 4 mm/s
```

Das Wasser steigt also mit **~4 mm/s** nach oben.

### Wie schnell dreht sich die Dosenoberfläche?

Bei 60 RPM, Dosenradius r = 33 mm:

```
v_Dose = ω × r = (60 × 2π / 60) × 0,033 m ≈ 207 mm/s
```

→ Die Dosenoberfläche bewegt sich **~50× schneller** als das durchströmende Wasser.

### Reynolds-Zahl im Vergleich

Die Reynolds-Zahl bestimmt, ob die Strömung laminar (glatt, schlecht für Wärmeübertragung) oder turbulent (gut) ist:

```
Re = v × D / ν     (ν_Wasser ≈ 1×10⁻⁶ m²/s bei 2°C, D = 66 mm)
```

| Strömungsquelle | Geschwindigkeit | Re | Regime |
|---|---|---|---|
| Wasserfluss (15 L/min) | 4 mm/s | **~264** | laminar – kaum Turbulenz |
| Rotation 60 RPM | 207 mm/s | **~13 600** | turbulent – sehr gut |
| Rotation 100 RPM | 346 mm/s | **~22 800** | stark turbulent |

**Der Wasserfluss allein erzeugt praktisch keine Turbulenz an der Dosenoberfläche.**  
Die Rotation dominiert die Wärmeübertragung vollständig.

### Bringt Drehen also noch was? Ja – eindeutig!

Beide Effekte sind **komplementär**, nicht konkurrierend:

| Mechanismus | Was es tut |
|---|---|
| **Durchfluss 15 L/min** | Hält die Bulk-Wassertemperatur konstant auf 2 °C – kein Aufwärmen des Badwassers |
| **Rotation 60 RPM** | Erzeugt Turbulenz an der Dosenoberfläche – bricht die thermische Grenzschicht auf |

Ohne Rotation: Das Wasser direkt an der Dosenoberfläche erwärmt sich und bleibt als isolierende Schicht (thermische Grenzschicht). Der Durchfluss erneuert das Wasser zwar, aber langsam – und laminar.

Mit Rotation: Die Grenzschicht wird ständig aufgerissen. Frisches 2 °C-Wasser kommt direkt an die Dose.

### Kühlzeitvergleich mit deinem System

| Szenario | h ca. | Kühlzeit 0,33 L Dose auf 4 °C |
|---|---|---|
| Statisch, stehendes Wasser | 200 W/(m²·K) | ~6,5 min |
| Durchfluss 15 L/min, keine Rotation | ~350 W/(m²·K) | ~3,5 min |
| **Durchfluss + 60 RPM** | **~1200–1500 W/(m²·K)** | **~55–70 s** |
| Durchfluss + 100 RPM | ~1500–1800 W/(m²·K) | ~45–55 s |

→ Der Durchfluss allein halbiert die Kühlzeit grob.  
→ Die **Kombination Durchfluss + Rotation** ist ~5–7× besser als statisches Eintauchen.

### Fazit: Ja, 60 RPM lohnt sich extrem!

Der Durchfluss löst ein anderes Problem als die Rotation:
- Durchfluss verhindert **Wassererwärmung** (wichtig wenn viele Dosen gleichzeitig)
- Rotation verhindert **Grenzschichtbildung** an der Dose (wichtig für schnellen Wärmeübergang)

Beide zusammen ergeben das bestmögliche System. **Ohne Rotation wäre dein Durchfluss verschenkt**, weil das Wasser zwar kalt ankommt, aber laminar an der Dose vorbeifließt, ohne die Grenzschicht aufzubrechen.

---

## Zusammenfassung

> **60–80 RPM** in 2 °C kaltem Wasser reduziert die Kühlzeit einer 0,33-L-Dose von ~6 min auf **~1 bis 1,5 min** – das ist ein Faktor 4–5 gegenüber statischem Eintauchen.

> Der **Zentrifugaleffekt innen** ist bei 60–80 RPM mit ~0,2 g vernachlässigbar. Ab ~160 RPM beginnt er, die Durchmischung des Getränkeinneren zu fördern.

> Der größte Kühlhebel ist **Rotation überhaupt** (erzwungene Außenkonvektion), nicht die genaue Drehzahl. Wassertemperatur ist der zweitwichtigste Faktor.
