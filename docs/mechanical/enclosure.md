# Mechanical Assembly

This page walks you through preparing the enclosure, drilling the sensor holes, assembling the radiation shield, and setting up the pole mount.

---

## Tools you'll need

- Power drill + drill bits (6 mm, Ø0.375 in, Ø0.413 in, Ø0.97 in, 0.136 in for tapping)
- M4 × 0.7 mm hand tap + cutting lubricant
- Allen key set
- Hose clamp tool / large flat screwdriver
- Calipers
- Silicone sealant
- Glue gun + glue sticks (for mesh)
- 90° countersink bit (for deburring)

---

## Step 1 — Machine the backplate

The backplate is the structural spine of the whole assembly. Everything mounts to it.

!!! tip "Which backplate do you need?"
    Use the **14.250 in backplate** for the 5W solar panel and the **16.250 in backplate** for the 10W solar panel. All hole positions are otherwise identical.

**Engineering drawings:**

- [BACK_PLATE_5W.pdf](../assets/BackPlate1.pdf) — for 5W solar panel (14.250 in)
- [BACK_PLATE_10W.pdf](../assets/Backplate2.pdf) — for 10W solar panel (16.250 in)

Key specs from the drawing:

- Overall length: **14.250 in** (5W) or **16.250 in** (10W)
- Four Ø0.200 in holes spaced at 1.570 in and 2.000 in from reference edges
- Four **M4 × 0.7 mm tapped holes** — use a **0.136 in drill bit** before tapping (high tap resistance)
- Two additional holes at 1.130 in and 5.380 in from bottom

Instructions:

1. Mark all hole locations using the drawing dimensions.
2. Clamp the backplate securely to a drill press.
3. Drill the Ø0.200 in clearance holes first.
4. Drill the four M4 tap holes using the **0.136 in drill bit**.
5. Tap the four M4 × 0.7 mm threads using a hand tap with cutting lubricant. Go slow.
6. Deburr all holes and break sharp corners with a file or 90° countersink.

---

## Step 2 — Machine the clamping brackets

You need two clamping brackets. These are the cross members that the hose clamps thread through to grip the pole.

**Engineering drawing:** [CLAMPING_BRACKET.pdf](../assets/clampingbracket.pdf)

Key specs from the drawing:

- Overall height: **3.484 in**, width: **1.436 in**
- Two Ø0.223 in holes spaced 0.880 in apart
- Corner radius: R0.102 in
- Slot dimensions: 0.174 in × 1.175 in

Instructions:

1. Mark all hole and slot locations using the drawing.
2. Drill the Ø0.223 in holes.
3. Mill or file the slots to dimension.
4. Deburr all holes and edges using a 90° countersink.

---

## Step 3 — Drill the enclosure holes

Three holes go into the enclosure: one for the sound sensor, one for the cable gland, and two small mounting holes.

**Engineering drawing:** [HOLE_TEMPLATE.pdf](../assets/hole_template2.pdf)

Key specs from the drawing (all dimensions in inches):

- **Sound sensor hole:** Ø0.97 in — center wall of enclosure
- **Cable gland hole:** Ø0.375 in — clamp side wall
- **Two small mounting holes:** Ø0.413 in and Ø0.189 in as dimensioned
- Total wall span: 5.28 in reference

Instructions:

1. Print the hole template drawing at 1:1 scale and tape it to the enclosure wall as a drill guide.
2. Drill each hole with the correct bit size. Go slow on the plastic — high RPM melts ABS.
3. Deburr all holes and break sharp edges.
4. Cut a small piece of protective mesh to cover the sound sensor hole.
5. Apply hot glue around the inside edge of the hole and press the mesh firmly in place. Hold until set.
6. Install the cable gland into the Ø0.375 in hole and apply silicone sealant around the base on the outside.

!!! warning "Hole size matters"
    This was tested — the hole must be exactly **6 mm**. Do not go larger.

!!! tip
    Don't fully tighten the cable gland until you have routed the solar cable through it during final assembly.

---

## Step 4 — Assemble the radiation shield

The radiation shield protects the temperature sensor from direct sunlight, which would cause false high readings.

1. Take the off-the-shelf radiation shield (APRS2698) and remove the disks from the housing.
2. Re-stack **6 disks** on the mounting rod with equal gaps between each disk for airflow.
3. The disk stack must extend to more than **150% of the sensor probe length** to keep the sensing element shaded at all sun angles.
4. Mount the assembled shield to the **bottom face of the enclosure** using screws, with the sensor probe pointing downward through the center of the disks.

!!! note
    The radiation shield is required for the temperature and humidity sensor. Without it, direct sunlight will heat the sensor and give you falsely high temperature readings.

---

## Step 5 — Final assembly and pole mount

1. Attach the two clamping brackets to the backplate using M4 allen bolts into the tapped holes. Use shallow bolts — no through-holes into the enclosure.
2. Attach the solar panel bracket (Voltaic BK102 for 5W or BK103 for 10W) to the top of the backplate.
3. Mount the enclosure to the cross members.
4. Position the assembled unit on the pole at **10 feet to the bottom of the box**.
5. Make sure the solar panel faces south and is unobstructed.
6. Thread two hose clamps through the bracket slots.
7. Lightly tighten the first clamp to hold, then align and fully tighten both.
8. Clip excess hose clamp length with diagonal cutters.

!!! warning "No through-holes in the enclosure"
    All fasteners go into tapped holes in the backplate — never through the enclosure walls. Through-holes break the IP67 seal.

---

## Final check

Before moving to electronics assembly, verify:

- [ ] Backplate drilled and all four M4 holes tapped
- [ ] Clamping brackets drilled and slots milled
- [ ] Enclosure holes drilled — sound sensor (Ø0.97 in), cable gland (Ø0.375 in)
- [ ] Mesh hot-glued over sound sensor hole from inside
- [ ] Cable gland installed and silicone sealed
- [ ] Radiation shield assembled with 6 disks and mounted to enclosure base
- [ ] Solar bracket and clamping brackets bolted to backplate
- [ ] Enclosure mounted with no through-holes

---

## QA Checklist

Complete this checklist after mechanical assembly.

- [Mechanical & Electrical QA/QC Checklist](../assets/mechanicalandelectricalqaqclist.pdf)