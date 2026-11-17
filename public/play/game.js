/* Escape Room: Infinite Rooms — browser edition.
 * A faithful HTML5 Canvas port of the C++/SFML game in this repository:
 * same rooms, room graph, doors, puzzles, traps, items, HUD, BFS mini-map,
 * procedural audio and checksum-validated quick save / quick load.
 */
(() => {
  "use strict";

  // ------------------------------------------------------------------
  // Constants & world data (mirrors src/main.cpp + src/GameRenderer.cpp)
  // ------------------------------------------------------------------
  const VIEW_W = 1200, VIEW_H = 800;
  const ROOM_W = 860, ROOM_H = 800;
  const WALL = 40;
  const BOUNDS = { minX: WALL + 26, maxX: ROOM_W - WALL - 26, minY: WALL + 34, maxY: ROOM_H - WALL - 34 };
  const PLAYER_R = 15;
  const SPEED = 230;
  const INTERACT_RANGE = 70;
  const SAVE_KEY = "escape-room-infinite.quicksave";
  const FONT = '"GameFont", ui-monospace, Menlo, Consolas, monospace';

  const ROOMS = {
    0: { name: "Entrance Hall", desc: "You have entered a mysterious building. Find a way to escape.", enter: "Two doors lead to different parts of the building.", theme: "entrance" },
    1: { name: "Library", desc: "An old library filled with dusty books and hidden clues.", enter: "Ancient books may contain clues to your escape.", theme: "library" },
    2: { name: "Laboratory", desc: "A strange laboratory containing mysterious equipment.", enter: "Machines hum quietly. Something is hidden here.", theme: "lab" },
    3: { name: "Secret Archive", desc: "A hidden archive containing the building's secrets.", enter: "A locked passage may lead to the final chamber.", theme: "archive" },
    4: { name: "Escape Chamber", desc: "The final chamber. Find a way to unlock the exit.", enter: "You have reached the final room. Can you escape?", theme: "escape" },
  };
  const EDGES = [[0, 1], [0, 2], [1, 3], [2, 3], [3, 4]];
  const EXIT_ROOM = 4;

  const PUZZLE_DEFS = {
    101: { kind: "Math Puzzle", title: "Library Calculation", q: "Solve: 5 + 3 = ?", answer: "8", points: 10 },
    102: { kind: "Logic Puzzle", title: "Laboratory Code", q: "What comes next: 2, 4, 6, ?", answer: "8", points: 10 },
    103: { kind: "Cipher Puzzle", title: "Archive Entry", q: "Decrypt: HELLO (shift 0)", answer: "HELLO", points: 10 },
    104: { kind: "Sequence Puzzle", title: "Escape Sequence", q: "Complete: 1, 2, 3, ?", answer: "4", points: 10 },
  };

  // Entities per room. Doors gated by a puzzle id (0 = always open).
  const ENTITY_DEFS = {
    0: [
      { t: "door", id: 1, label: "Library", dest: 1, puzzle: 101, x: 430, y: 40, side: "N" },
      { t: "door", id: 2, label: "Laboratory", dest: 2, puzzle: 102, x: 820, y: 400, side: "E" },
      { t: "trap", key: "t0", name: "Spike Trap", dmg: 20, x: 430, y: 380, w: 160, h: 70, kind: "spike" },
      { t: "item", key: "i0", name: "Health Potion", desc: "Restores 25 Health Points", heal: 25, x: 240, y: 520 },
    ],
    1: [
      { t: "door", id: 3, label: "Secret Archive", dest: 3, puzzle: 103, x: 430, y: 40, side: "N" },
      { t: "door", id: 10, label: "Entrance Hall", dest: 0, puzzle: 0, x: 430, y: 760, side: "S" },
      { t: "pedestal", puzzle: 101, name: "Library Puzzle #101", x: 430, y: 320 },
      { t: "item", key: "i1", name: "Clue Scroll", desc: "Ancient writings detailing facility secrets", heal: 0, x: 580, y: 360 },
    ],
    2: [
      { t: "door", id: 3, label: "Secret Archive", dest: 3, puzzle: 103, x: 430, y: 40, side: "N" },
      { t: "door", id: 20, label: "Entrance Hall", dest: 0, puzzle: 0, x: 40, y: 400, side: "W" },
      { t: "pedestal", puzzle: 102, name: "Laboratory Puzzle #102", x: 430, y: 360 },
      { t: "trap", key: "t2", name: "Poison Gas Vent", dmg: 15, x: 430, y: 520, w: 160, h: 70, kind: "poison" },
      { t: "item", key: "i2", name: "Medkit", desc: "Standard facility first-aid kit (+35 HP)", heal: 35, x: 280, y: 560 },
    ],
    3: [
      { t: "door", id: 4, label: "Escape Chamber", dest: 4, puzzle: 104, x: 430, y: 40, side: "N" },
      { t: "door", id: 30, label: "Library", dest: 1, puzzle: 0, x: 280, y: 760, side: "S" },
      { t: "door", id: 31, label: "Laboratory", dest: 2, puzzle: 0, x: 580, y: 760, side: "S" },
      { t: "pedestal", puzzle: 103, name: "Archive Cipher #103", x: 430, y: 400 },
      { t: "trap", key: "t3", name: "Electric Arc Coil", dmg: 25, x: 430, y: 260, w: 160, h: 70, kind: "electric" },
      { t: "item", key: "i3", name: "Archive Keycard", desc: "Level-4 Security access card", heal: 0, x: 600, y: 560 },
    ],
    4: [
      { t: "door", id: 40, label: "Secret Archive", dest: 3, puzzle: 0, x: 430, y: 760, side: "S" },
      { t: "pedestal", puzzle: 104, name: "Final Escape Sequence #104", x: 430, y: 320 },
    ],
  };

  const THEMES = {
    entrance: { tileA: "#373a46", tileB: "#2d303a", grout: "#23262f", wall: "#1b1c22", wallTop: "#2c2e38", trim: "#8a7a5a", mote: [255, 230, 180] },
    library: { tileA: "#412b1e", tileB: "#372318", grout: "#281a12", wall: "#1d120b", wallTop: "#2f1f14", trim: "#b48a3c", mote: [255, 220, 130] },
    lab: { tileA: "#202c38", tileB: "#18222e", grout: "#123c4b", wall: "#0d151c", wallTop: "#18242f", trim: "#2ad6c4", mote: [80, 255, 180] },
    archive: { tileA: "#201a2a", tileB: "#181422", grout: "#2d203c", wall: "#110c18", wallTop: "#1e1629", trim: "#a45af0", mote: [200, 140, 255] },
    escape: { tileA: "#30291e", tileB: "#262016", grout: "#7a6430", wall: "#17120a", wallTop: "#2a2112", trim: "#e8c25a", mote: [255, 235, 100] },
  };

  // ------------------------------------------------------------------
  // Helpers
  // ------------------------------------------------------------------
  const $ = (id) => document.getElementById(id);
  const clamp = (v, a, b) => Math.max(a, Math.min(b, v));
  const normalize = (s) => String(s).trim().toLowerCase();
  const rand = (a, b) => a + Math.random() * (b - a);
  const esc = (s) => String(s).replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));

  function neighbors(id) {
    const out = [];
    for (const [a, b] of EDGES) {
      if (a === id) out.push(b);
      else if (b === id) out.push(a);
    }
    return out.sort((x, y) => x - y);
  }

  // Breadth-first search shortest path (RoomGraph::findPath).
  function bfsPath(from, to) {
    if (from === to) return [from];
    const prev = { [from]: -1 };
    const queue = [from];
    while (queue.length) {
      const cur = queue.shift();
      for (const n of neighbors(cur)) {
        if (n in prev) continue;
        prev[n] = cur;
        if (n === to) {
          const path = [to];
          let p = cur;
          while (p !== -1) { path.unshift(p); p = prev[p]; }
          return path;
        }
        queue.push(n);
      }
    }
    return [];
  }

  // djb2-style checksum used to detect tampered or corrupt saves.
  function checksum(str) {
    let h = 5381;
    for (let i = 0; i < str.length; i++) h = ((h << 5) + h + str.charCodeAt(i)) >>> 0;
    return h.toString(16).padStart(8, "0");
  }

  // ------------------------------------------------------------------
  // Procedural sound synthesizer (11 effects, no audio files)
  // ------------------------------------------------------------------
  const Sound = {
    ctx: null,
    muted: false,
    ensure() {
      if (!this.ctx) {
        const AC = window.AudioContext || window.webkitAudioContext;
        if (!AC) return null;
        this.ctx = new AC();
        this.master = this.ctx.createGain();
        this.master.gain.value = 0.35;
        this.master.connect(this.ctx.destination);
      }
      if (this.ctx.state === "suspended") this.ctx.resume();
      return this.ctx;
    },
    tone(freq, dur, { type = "sine", vol = 0.5, at = 0, slide = 0 } = {}) {
      const ctx = this.ctx;
      const t = ctx.currentTime + at;
      const o = ctx.createOscillator();
      const g = ctx.createGain();
      o.type = type;
      o.frequency.setValueAtTime(freq, t);
      if (slide) o.frequency.exponentialRampToValueAtTime(Math.max(20, freq + slide), t + dur);
      g.gain.setValueAtTime(0.0001, t);
      g.gain.exponentialRampToValueAtTime(vol, t + 0.01);
      g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
      o.connect(g).connect(this.master);
      o.start(t);
      o.stop(t + dur + 0.02);
    },
    noise(dur, { vol = 0.4, at = 0, freq = 1200 } = {}) {
      const ctx = this.ctx;
      const t = ctx.currentTime + at;
      const len = Math.floor(ctx.sampleRate * dur);
      const buf = ctx.createBuffer(1, len, ctx.sampleRate);
      const data = buf.getChannelData(0);
      for (let i = 0; i < len; i++) data[i] = (Math.random() * 2 - 1) * (1 - i / len);
      const src = ctx.createBufferSource();
      src.buffer = buf;
      const f = ctx.createBiquadFilter();
      f.type = "bandpass";
      f.frequency.value = freq;
      const g = ctx.createGain();
      g.gain.value = vol;
      src.connect(f).connect(g).connect(this.master);
      src.start(t);
    },
    play(name) {
      if (this.muted || !this.ensure()) return;
      switch (name) {
        case "footstep": this.noise(0.06, { vol: 0.12, freq: rand(300, 500) }); break;
        case "pickup": this.tone(660, 0.1, { type: "square", vol: 0.15 }); this.tone(990, 0.16, { type: "square", vol: 0.15, at: 0.08 }); break;
        case "doorUnlock": this.noise(0.35, { vol: 0.3, freq: 300 }); this.tone(220, 0.3, { type: "triangle", vol: 0.25, slide: 220 }); break;
        case "doorLocked": this.tone(140, 0.12, { type: "square", vol: 0.2 }); this.tone(110, 0.18, { type: "square", vol: 0.2, at: 0.12 }); break;
        case "interact": this.tone(880, 0.08, { type: "sine", vol: 0.2 }); break;
        case "correct": [523, 659, 784].forEach((f, i) => this.tone(f, 0.18, { type: "triangle", vol: 0.25, at: i * 0.09 })); break;
        case "wrong": this.tone(200, 0.35, { type: "sawtooth", vol: 0.2, slide: -80 }); break;
        case "trap": this.noise(0.4, { vol: 0.5, freq: 900 }); this.tone(90, 0.4, { type: "sawtooth", vol: 0.3, slide: -40 }); break;
        case "heal": [392, 523, 659, 784].forEach((f, i) => this.tone(f, 0.25, { type: "sine", vol: 0.2, at: i * 0.07 })); break;
        case "victory": [523, 659, 784, 1046, 784, 1046].forEach((f, i) => this.tone(f, i === 5 ? 0.7 : 0.2, { type: "triangle", vol: 0.28, at: i * 0.14 })); break;
        case "gameover": [392, 330, 262, 196].forEach((f, i) => this.tone(f, 0.4, { type: "sawtooth", vol: 0.18, at: i * 0.25 })); break;
      }
    },
  };

  // ------------------------------------------------------------------
  // Game state
  // ------------------------------------------------------------------
  let S; // serialisable state
  let fx; // transient visual state
  let mode = "title"; // title | play | puzzle | paused | over | won

  function freshState() {
    const puzzles = {};
    for (const id of Object.keys(PUZZLE_DEFS)) puzzles[id] = { solved: false, attempts: 0 };
    return {
      room: 0,
      x: 430, y: 620, dir: "up",
      hp: 100,
      score: 10,
      moves: 0,
      visited: [0],
      puzzles,
      traps: {},       // trap key -> true once triggered
      collected: {},   // item key -> true once picked up
      inventory: [],   // [{name, desc, heal}]
      showPath: true,
      startedAt: Date.now(),
      elapsed: 0,
    };
  }

  function freshFx() {
    return { t: 0, walk: 0, stepTimer: 0, floats: [], log: [], motes: makeMotes(S.room), shake: 0, flash: 0, doorAnim: {} };
  }

  function makeMotes(room) {
    const arr = [];
    for (let i = 0; i < 46; i++) {
      arr.push({ x: rand(WALL, ROOM_W - WALL), y: rand(WALL, ROOM_H - WALL), vx: rand(-8, 8), vy: rand(-18, -4), r: rand(1, 2.6), a: rand(0.2, 0.8), p: rand(0, 6.28) });
    }
    return arr;
  }

  function log(msg, color = "#d8ccb4") {
    fx.log.push({ msg, color });
    if (fx.log.length > 7) fx.log.shift();
  }

  function floatText(text, color, x = S.x, y = S.y - 30) {
    fx.floats.push({ text, color, x, y, t: 0 });
  }

  const isSolved = (id) => !!(S.puzzles[id] && S.puzzles[id].solved);
  const allSolved = () => Object.keys(PUZZLE_DEFS).every(isSolved);
  const doorOpen = (d) => d.puzzle === 0 || isSolved(d.puzzle);

  function entities() {
    return ENTITY_DEFS[S.room] || [];
  }

  function healthStatus(hp) {
    if (hp >= 80) return ["Excellent", "#6dffa8"];
    if (hp >= 50) return ["Good", "#e8d44a"];
    if (hp > 30) return ["Hurt", "#ff9d3c"];
    return ["Critical", "#ff5a4e"];
  }

  // ------------------------------------------------------------------
  // Input
  // ------------------------------------------------------------------
  const keys = new Set();
  const touchDirs = new Set();

  window.addEventListener("keydown", (e) => {
    if (mode === "puzzle") {
      if (e.key === "Escape") { e.preventDefault(); closePuzzle(); }
      return;
    }
    const k = e.key.toLowerCase();
    if (["arrowup", "arrowdown", "arrowleft", "arrowright", " ", "f5", "f9"].includes(k)) e.preventDefault();

    if (mode === "play") {
      keys.add(k);
      if (e.repeat) return;
      if (k === "e" || k === " " || k === "enter") interact();
      else if (k === "escape" || k === "p") openPause();
      else if (k === "m") { S.showPath = !S.showPath; Sound.play("interact"); log(S.showPath ? "Escape route overlay ON" : "Escape route overlay OFF"); }
      else if (["1", "2", "3", "4"].includes(k)) useItem(Number(k) - 1);
      else if (k === "f5") quickSave();
      else if (k === "f9") quickLoad();
    } else if (mode === "paused" && (k === "escape" || k === "p")) {
      resume();
    }
  });
  window.addEventListener("keyup", (e) => keys.delete(e.key.toLowerCase()));
  window.addEventListener("blur", () => { keys.clear(); touchDirs.clear(); });

  // Touch controls
  document.querySelectorAll("#touch [data-dir]").forEach((btn) => {
    const dir = btn.dataset.dir;
    const on = (e) => { e.preventDefault(); touchDirs.add(dir); btn.classList.add("active"); };
    const off = (e) => { e.preventDefault(); touchDirs.delete(dir); btn.classList.remove("active"); };
    btn.addEventListener("pointerdown", on);
    btn.addEventListener("pointerup", off);
    btn.addEventListener("pointercancel", off);
    btn.addEventListener("pointerleave", off);
  });
  $("t-act").addEventListener("pointerdown", (e) => { e.preventDefault(); if (mode === "play") interact(); });
  $("t-menu").addEventListener("pointerdown", (e) => { e.preventDefault(); if (mode === "play") openPause(); else if (mode === "paused") resume(); });

  // Clicking inventory slots on the HUD
  const canvas = $("game");
  canvas.addEventListener("pointerdown", (e) => {
    if (mode !== "play") return;
    const r = canvas.getBoundingClientRect();
    const x = ((e.clientX - r.left) / r.width) * VIEW_W;
    const y = ((e.clientY - r.top) / r.height) * VIEW_H;
    for (let i = 0; i < 4; i++) {
      const s = slotRect(i);
      if (x >= s.x && x <= s.x + s.w && y >= s.y && y <= s.y + s.h) { useItem(i); return; }
    }
  });

  // ------------------------------------------------------------------
  // Gameplay actions
  // ------------------------------------------------------------------
  function nearest() {
    let best = null, bestD = Infinity;
    for (const e of entities()) {
      if (e.t === "item" && S.collected[e.key]) continue;
      if (e.t === "trap") continue;
      const d = Math.hypot(S.x - e.x, S.y - e.y);
      if (d < INTERACT_RANGE + (e.t === "door" ? 12 : 0) && d < bestD) { best = e; bestD = d; }
    }
    return best;
  }

  function interact() {
    const e = nearest();
    if (!e) return;
    if (e.t === "door") {
      if (doorOpen(e)) {
        moveToRoom(e.dest, e.side);
      } else {
        Sound.play("doorLocked");
        log("Door locked! Requires puzzle resolution.", "#ff8a7e");
        floatText("LOCKED!", "#ff5a4e");
        openPuzzle(e.puzzle);
      }
    } else if (e.t === "pedestal") {
      Sound.play("interact");
      log("Inspecting " + e.name);
      openPuzzle(e.puzzle);
    } else if (e.t === "item") {
      S.collected[e.key] = true;
      S.inventory.push({ name: e.name, desc: e.desc, heal: e.heal });
      Sound.play("pickup");
      log("Collected: " + e.name, "#7ff0ff");
      floatText("+" + e.name, "#50f0ff");
    }
  }

  function moveToRoom(dest, side) {
    S.room = dest;
    S.moves++;
    S.visited.push(dest);
    Sound.play("doorUnlock");
    fx.motes = makeMotes(dest);
    fx.flash = 1;
    // Reposition the player at the matching entrance of the new room.
    if (side === "N") { S.x = 430; S.y = 680; S.dir = "up"; }
    else if (side === "S") { S.x = 430; S.y = 110; S.dir = "down"; }
    else if (side === "E") { S.x = 110; S.y = 400; S.dir = "right"; }
    else { S.x = 750; S.y = 400; S.dir = "left"; }
    // Find the door we came through so we don't spawn on top of a trap or wall.
    const back = entities().find((d) => d.t === "door" && d.dest !== undefined && sideOpposite(side) === d.side && d.side === "S");
    if (back && side === "N") { S.x = back.x; }
    log("Entered " + ROOMS[dest].name + ". " + ROOMS[dest].enter, "#f3e0b0");
  }
  const sideOpposite = (s) => ({ N: "S", S: "N", E: "W", W: "E" }[s]);

  function useItem(i) {
    const it = S.inventory[i];
    if (!it) return;
    if (it.heal > 0) {
      if (S.hp >= 100) { log("Health already full.", "#a89c86"); return; }
      const before = S.hp;
      S.hp = Math.min(100, S.hp + it.heal);
      S.inventory.splice(i, 1);
      Sound.play("heal");
      log(`Used ${it.name}: +${S.hp - before} HP`, "#6dffa8");
      floatText(`+${S.hp - before} HP`, "#6dffa8");
    } else {
      Sound.play("interact");
      if (it.name === "Clue Scroll") log("Scroll: \"The answers are simpler than they seem.\"", "#e8c78a");
      else log(it.name + ": " + it.desc, "#e8c78a");
    }
  }

  function updateTraps() {
    for (const e of entities()) {
      if (e.t !== "trap" || S.traps[e.key]) continue;
      const hx = e.w / 2, hy = e.h / 2;
      if (Math.abs(S.x - e.x) < hx + PLAYER_R * 0.6 && Math.abs(S.y - e.y) < hy + PLAYER_R * 0.6) {
        S.traps[e.key] = true;
        if (Math.random() < 0.5) {
          S.score += 2;
          Sound.play("interact");
          log(`Safe from ${e.name}! (+2 pts)`, "#ffd23c");
          floatText("TRAP EVADED! +2 pts", "#ffd23c");
        } else {
          S.hp = Math.max(0, S.hp - e.dmg);
          S.score = Math.max(0, S.score - 5);
          Sound.play("trap");
          fx.shake = 0.4;
          log(`${e.name} struck! (-${e.dmg} HP, -5 pts)`, "#ff6a5e");
          floatText(`-${e.dmg} HP!`, "#ff3c32");
          if (S.hp > 0 && S.hp <= 30) log("CRITICAL HEALTH! Use a healing item!", "#ff5a4e");
        }
      }
    }
  }

  // ------------------------------------------------------------------
  // Puzzle terminal
  // ------------------------------------------------------------------
  let activePuzzle = null;
  const pz = { root: $("puzzle"), form: $("pz-form"), input: $("pz-input"), status: $("pz-status") };

  function openPuzzle(id) {
    const def = PUZZLE_DEFS[id];
    if (!def) return;
    activePuzzle = id;
    mode = "puzzle";
    keys.clear();
    touchDirs.clear();
    const st = S.puzzles[id];
    $("pz-id").textContent = "TERMINAL #" + id;
    $("pz-kind").textContent = def.kind;
    $("pz-title").textContent = def.title;
    $("pz-q").textContent = def.q;
    $("pz-reward").textContent = "Reward: +" + def.points + " pts";
    $("pz-attempts").textContent = "Attempts: " + st.attempts;
    pz.input.value = "";
    pz.status.className = "term-status";
    pz.status.textContent = st.solved ? "Already solved. Access granted." : "Awaiting input…";
    if (st.solved) pz.status.classList.add("ok");
    pz.input.disabled = st.solved;
    $("pz-submit").disabled = st.solved;
    pz.root.classList.remove("hidden");
    setTimeout(() => { if (!st.solved) pz.input.focus(); else $("pz-close").focus(); }, 30);
  }

  function closePuzzle() {
    pz.root.classList.add("hidden");
    pz.input.blur();
    activePuzzle = null;
    mode = "play";
  }

  pz.form.addEventListener("submit", (e) => {
    e.preventDefault();
    const id = activePuzzle;
    if (!id) return;
    const def = PUZZLE_DEFS[id];
    const st = S.puzzles[id];
    if (st.solved) return;
    const guess = pz.input.value;
    if (!guess.trim()) return;
    st.attempts++;
    $("pz-attempts").textContent = "Attempts: " + st.attempts;
    if (normalize(guess) === normalize(def.answer)) {
      st.solved = true;
      // Fewer points for more attempts, never below half (Puzzle::submitAnswer).
      const pts = Math.max(def.points - (st.attempts - 1), Math.floor(def.points / 2));
      S.score += pts;
      Sound.play("correct");
      pz.status.className = "term-status ok";
      pz.status.textContent = `ACCESS GRANTED! Correct answer. (+${pts} pts)`;
      pz.input.disabled = true;
      $("pz-submit").disabled = true;
      log(`Solved "${def.title}" (+${pts} pts)`, "#6dffa8");
      floatText(`ACCESS GRANTED +${pts}`, "#6dffa8");
      setTimeout(() => { if (activePuzzle === id) closePuzzle(); }, 1100);
    } else {
      Sound.play("wrong");
      pz.status.className = "term-status bad";
      pz.status.textContent = "ACCESS DENIED. Incorrect answer, try again.";
      pz.form.classList.remove("shake");
      void pz.form.offsetWidth;
      pz.form.classList.add("shake");
      pz.input.select();
    }
  });
  $("pz-close").addEventListener("click", closePuzzle);

  // ------------------------------------------------------------------
  // Save / load (localStorage, checksum validated)
  // ------------------------------------------------------------------
  function quickSave() {
    try {
      const body = JSON.stringify({ version: 1, savedAt: new Date().toISOString(), state: S });
      localStorage.setItem(SAVE_KEY, JSON.stringify({ body, checksum: checksum(body) }));
      Sound.play("interact");
      log("QuickSave written to browser storage.", "#6dffa8");
      floatText("GAME SAVED!", "#50ff8c");
      return true;
    } catch (err) {
      log("Save error: " + err.message, "#ff6a5e");
      return false;
    }
  }

  function readSave() {
    const raw = localStorage.getItem(SAVE_KEY);
    if (!raw) throw new Error("No save file found.");
    let wrapper;
    try { wrapper = JSON.parse(raw); } catch { throw new Error("Save file is corrupt."); }
    if (!wrapper || typeof wrapper.body !== "string" || checksum(wrapper.body) !== wrapper.checksum) {
      throw new Error("Checksum mismatch: save is corrupt or was tampered with.");
    }
    const data = JSON.parse(wrapper.body);
    const st = data.state;
    if (!st || !(st.room in ROOMS) || typeof st.hp !== "number") throw new Error("Save data is invalid.");
    return st;
  }

  function hasSave() {
    try { readSave(); return true; } catch { return false; }
  }

  function quickLoad() {
    try {
      const st = readSave();
      S = Object.assign(freshState(), st);
      const keepLog = fx ? fx.log : [];
      fx = freshFx();
      fx.log = keepLog;
      Sound.play("interact");
      log("QuickSave restored.", "#6dffa8");
      floatText("GAME LOADED!", "#50ff8c");
      if (mode !== "play") { hideOverlay(); mode = "play"; }
      return true;
    } catch (err) {
      Sound.play("doorLocked");
      log("Load error: " + err.message, "#ff6a5e");
      return false;
    }
  }

  // ------------------------------------------------------------------
  // Overlay screens
  // ------------------------------------------------------------------
  const overlay = $("overlay"), panel = $("panel");
  function showOverlay(html) { panel.innerHTML = html; overlay.classList.remove("hidden"); const b = panel.querySelector("button:not(:disabled)"); if (b) setTimeout(() => b.focus(), 30); }
  function hideOverlay() { overlay.classList.add("hidden"); }

  const CONTROLS = `
    <div class="keys">
      <kbd>WASD / Arrows</kbd><span>Move</span>
      <kbd>E / Space</kbd><span>Interact: doors, terminals, items</span>
      <kbd>1 – 4</kbd><span>Use inventory item</span>
      <kbd>M</kbd><span>Toggle escape-route overlay</span>
      <kbd>F5 / F9</kbd><span>Quick save / quick load</span>
      <kbd>Esc / P</kbd><span>Pause menu</span>
    </div>`;

  function showTitle() {
    mode = "title";
    showOverlay(`
      <h1>ESCAPE ROOM</h1>
      <p class="sub">Infinite Rooms · You wake inside a sealed building.<br/>Solve every terminal, survive the traps, reach the Escape Chamber.</p>
      ${CONTROLS}
      <div class="menu">
        <button class="btn" data-act="new">New Game</button>
        <button class="btn ghost" data-act="load" ${hasSave() ? "" : "disabled"}>Continue Saved Game</button>
      </div>
      <p class="note"><a href="/">← Back to project page</a></p>`);
  }

  function openPause() {
    mode = "paused";
    keys.clear();
    showOverlay(`
      <h1>PAUSED</h1>
      <p class="sub">${esc(ROOMS[S.room].name)} · ${Object.values(S.puzzles).filter((p) => p.solved).length}/4 puzzles solved</p>
      ${CONTROLS}
      <div class="menu">
        <button class="btn" data-act="resume">Resume</button>
        <button class="btn ghost" data-act="save">Quick Save</button>
        <button class="btn ghost" data-act="load" ${hasSave() ? "" : "disabled"}>Quick Load</button>
        <button class="btn ghost" data-act="mute">${Sound.muted ? "Unmute Sound" : "Mute Sound"}</button>
        <button class="btn ghost" data-act="new">Restart</button>
      </div>`);
  }

  function resume() { hideOverlay(); mode = "play"; }

  function fmtTime(ms) {
    const s = Math.floor(ms / 1000);
    return `${Math.floor(s / 60)}:${String(s % 60).padStart(2, "0")}`;
  }

  function showEnd(won) {
    mode = won ? "won" : "over";
    keys.clear();
    Sound.play(won ? "victory" : "gameover");
    showOverlay(`
      <h1 class="${won ? "" : "bad"}">${won ? "YOU ESCAPED!" : "GAME OVER"}</h1>
      <p class="sub">${won ? "Congratulations! You solved every puzzle and escaped the mysterious rooms." : "Your health ran out. The building claims another explorer."}</p>
      <div class="stats">
        <div><b>${S.score}</b>score</div>
        <div><b>${S.moves}</b>moves</div>
        <div><b>${fmtTime(S.elapsed)}</b>time</div>
        <div><b>${S.hp}</b>health</div>
      </div>
      <div class="menu">
        <button class="btn" data-act="new">${won ? "Play Again" : "Try Again"}</button>
        ${won ? "" : `<button class="btn ghost" data-act="load" ${hasSave() ? "" : "disabled"}>Load Quick Save</button>`}
        <a class="btn ghost" href="/" style="text-decoration:none">Project Page</a>
      </div>`);
  }

  panel.addEventListener("click", (e) => {
    const btn = e.target.closest("button[data-act]");
    if (!btn) return;
    Sound.ensure();
    const act = btn.dataset.act;
    if (act === "new") newGame();
    else if (act === "resume") resume();
    else if (act === "save") { quickSave(); openPause(); }
    else if (act === "load") { if (quickLoad()) resume(); }
    else if (act === "mute") { Sound.muted = !Sound.muted; openPause(); }
  });

  function newGame() {
    S = freshState();
    fx = freshFx();
    log("You have entered a mysterious building. Find a way to escape.", "#f3e0b0");
    log("Walk to a door and press E. Locked doors open a puzzle terminal.");
    hideOverlay();
    mode = "play";
  }

  // ------------------------------------------------------------------
  // Update
  // ------------------------------------------------------------------
  function update(dt) {
    fx.t += dt;
    for (const m of fx.motes) {
      m.x += m.vx * dt; m.y += m.vy * dt; m.p += dt * 2;
      if (m.y < WALL) { m.y = ROOM_H - WALL; m.x = rand(WALL, ROOM_W - WALL); }
      if (m.x < WALL) m.x = ROOM_W - WALL; else if (m.x > ROOM_W - WALL) m.x = WALL;
    }
    fx.floats.forEach((f) => (f.t += dt));
    fx.floats = fx.floats.filter((f) => f.t < 1.6);
    fx.shake = Math.max(0, fx.shake - dt);
    fx.flash = Math.max(0, fx.flash - dt * 2.5);
    for (const e of entities()) {
      if (e.t !== "door") continue;
      const k = S.room + ":" + e.id;
      const target = doorOpen(e) ? 1 : 0;
      const cur = fx.doorAnim[k] ?? target;
      fx.doorAnim[k] = cur + (target - cur) * Math.min(1, dt * 4);
    }

    if (mode !== "play") return;
    S.elapsed += dt * 1000;

    let dx = 0, dy = 0;
    if (keys.has("w") || keys.has("arrowup") || touchDirs.has("up")) dy -= 1;
    if (keys.has("s") || keys.has("arrowdown") || touchDirs.has("down")) dy += 1;
    if (keys.has("a") || keys.has("arrowleft") || touchDirs.has("left")) dx -= 1;
    if (keys.has("d") || keys.has("arrowright") || touchDirs.has("right")) dx += 1;
    const moving = dx || dy;
    if (moving) {
      const len = Math.hypot(dx, dy);
      let nx = clamp(S.x + (dx / len) * SPEED * dt, BOUNDS.minX, BOUNDS.maxX);
      let ny = clamp(S.y + (dy / len) * SPEED * dt, BOUNDS.minY, BOUNDS.maxY);
      // Pedestals are solid.
      for (const e of entities()) {
        if (e.t !== "pedestal") continue;
        const ddx = nx - e.x, ddy = ny - e.y, d = Math.hypot(ddx, ddy), min = 30 + PLAYER_R;
        if (d < min && d > 0.001) { nx = e.x + (ddx / d) * min; ny = e.y + (ddy / d) * min; }
      }
      S.x = nx; S.y = ny;
      if (Math.abs(dx) > Math.abs(dy)) S.dir = dx > 0 ? "right" : "left";
      else S.dir = dy > 0 ? "down" : "up";
      fx.walk += dt * 10;
      fx.stepTimer -= dt;
      if (fx.stepTimer <= 0) { Sound.play("footstep"); fx.stepTimer = 0.32; }
    } else {
      fx.walk = 0;
    }

    updateTraps();

    if (S.hp <= 0) { showEnd(false); return; }
    if (S.room === EXIT_ROOM && allSolved()) showEnd(true);
  }

  // ------------------------------------------------------------------
  // Rendering
  // ------------------------------------------------------------------
  const ctx = canvas.getContext("2d");
  const light = document.createElement("canvas");
  light.width = ROOM_W; light.height = ROOM_H;
  const lctx = light.getContext("2d");

  function rr(c, x, y, w, h, r) {
    c.beginPath();
    c.moveTo(x + r, y);
    c.arcTo(x + w, y, x + w, y + h, r);
    c.arcTo(x + w, y + h, x, y + h, r);
    c.arcTo(x, y + h, x, y, r);
    c.arcTo(x, y, x + w, y, r);
    c.closePath();
  }

  function torchPositions() {
    return [
      [200, WALL - 6], [660, WALL - 6],
      [WALL - 6, 220], [WALL - 6, 600], [ROOM_W - WALL + 6, 220], [ROOM_W - WALL + 6, 600],
    ];
  }

  function drawFloor(th, theme) {
    const size = 52;
    for (let y = WALL; y < ROOM_H - WALL; y += size) {
      for (let x = WALL; x < ROOM_W - WALL; x += size) {
        const alt = ((x / size) | 0) % 2 === ((y / size) | 0) % 2;
        ctx.fillStyle = alt ? th.tileA : th.tileB;
        ctx.fillRect(x, y, size, size);
        ctx.strokeStyle = th.grout;
        ctx.lineWidth = 1;
        ctx.strokeRect(x + 0.5, y + 0.5, size - 1, size - 1);
      }
    }
    // Theme props
    ctx.save();
    if (theme === "entrance") {
      ctx.fillStyle = "rgba(140,25,35,0.8)";
      ctx.fillRect(370, WALL, 120, ROOM_H - WALL * 2);
      ctx.strokeStyle = "rgba(200,160,60,0.8)"; ctx.lineWidth = 3;
      ctx.strokeRect(376, WALL, 108, ROOM_H - WALL * 2);
      // stone pillars
      for (const [px, py] of [[150, 150], [710, 150], [150, 650], [710, 650]]) {
        ctx.fillStyle = "#4a4d5a"; ctx.beginPath(); ctx.arc(px, py, 24, 0, 7); ctx.fill();
        ctx.strokeStyle = "#23252e"; ctx.lineWidth = 4; ctx.stroke();
      }
    } else if (theme === "library") {
      rr(ctx, 230, 450, 400, 240, 6);
      ctx.fillStyle = "rgba(24,70,50,0.82)"; ctx.fill();
      ctx.strokeStyle = "rgba(190,145,50,0.9)"; ctx.lineWidth = 4; ctx.stroke();
      rr(ctx, 270, 485, 320, 170, 4);
      ctx.fillStyle = "rgba(80,30,35,0.7)"; ctx.fill();
      ctx.strokeStyle = "rgba(210,180,90,0.6)"; ctx.lineWidth = 2; ctx.stroke();
      // bookshelves
      const shelf = (x, y, w, h) => {
        ctx.fillStyle = "#2a1810"; ctx.fillRect(x, y, w, h);
        const cols = ["#7a2e2a", "#2f5a3c", "#3b4c7a", "#8a6a2a", "#5a2f5a", "#6b3d1f"];
        const horiz = w > h;
        const n = Math.floor((horiz ? w : h) / 11);
        for (let i = 0; i < n; i++) {
          ctx.fillStyle = cols[(i * 7 + x) % cols.length];
          if (horiz) ctx.fillRect(x + 4 + i * 11, y + 4 + (i % 3), 8, h - 8 - (i % 3));
          else ctx.fillRect(x + 4 + (i % 3), y + 4 + i * 11, w - 8 - (i % 3), 8);
        }
        ctx.strokeStyle = "#140c07"; ctx.lineWidth = 3; ctx.strokeRect(x, y, w, h);
      };
      shelf(WALL, 120, 34, 220); shelf(WALL, 460, 34, 220);
      shelf(ROOM_W - WALL - 34, 120, 34, 220); shelf(ROOM_W - WALL - 34, 460, 34, 220);
      shelf(120, WALL, 180, 30); shelf(560, WALL, 180, 30);
    } else if (theme === "lab") {
      ctx.strokeStyle = "rgba(0,220,200,0.35)"; ctx.lineWidth = 2;
      ctx.beginPath(); ctx.moveTo(100, 400); ctx.lineTo(760, 400); ctx.moveTo(430, 100); ctx.lineTo(430, 700); ctx.stroke();
      ctx.beginPath(); ctx.arc(430, 360, 90, 0, 7);
      ctx.fillStyle = "rgba(20,50,65,0.6)"; ctx.fill();
      ctx.strokeStyle = "rgba(0,230,210,0.6)"; ctx.stroke();
      // benches with flasks
      for (const [bx, by] of [[90, 120], [640, 120], [640, 620]]) {
        ctx.fillStyle = "#2b3a46"; ctx.fillRect(bx, by, 130, 54);
        ctx.strokeStyle = "#0c1419"; ctx.lineWidth = 3; ctx.strokeRect(bx, by, 130, 54);
        for (let i = 0; i < 3; i++) {
          const fxp = bx + 25 + i * 40, glow = 0.5 + 0.5 * Math.sin(fx.t * 3 + i + bx);
          ctx.fillStyle = `rgba(${i === 1 ? "255,90,200" : "80,255,180"},${0.5 + glow * 0.4})`;
          ctx.beginPath(); ctx.arc(fxp, by + 30, 9, 0, 7); ctx.fill();
          ctx.fillStyle = "#9ab"; ctx.fillRect(fxp - 3, by + 12, 6, 10);
        }
      }
    } else if (theme === "archive") {
      ctx.save(); ctx.translate(430, 400); ctx.rotate(fx.t * 0.15);
      ctx.strokeStyle = "rgba(170,90,250,0.55)"; ctx.lineWidth = 3;
      ctx.beginPath(); ctx.arc(0, 0, 130, 0, 7); ctx.stroke();
      ctx.beginPath(); ctx.arc(0, 0, 100, 0, 7); ctx.stroke();
      ctx.font = `18px ${FONT}`; ctx.fillStyle = "rgba(200,150,255,0.7)"; ctx.textAlign = "center"; ctx.textBaseline = "middle";
      const runes = "ᚠᚢᚦᚨᚱᚲᚷᚹᚺᚾᛁᛃ";
      for (let i = 0; i < runes.length; i++) {
        const a = (i / runes.length) * Math.PI * 2;
        ctx.fillText(runes[i], Math.cos(a) * 115, Math.sin(a) * 115);
      }
      ctx.restore();
      // filing cabinets
      for (const [cx, cy] of [[80, 120], [80, 560], [720, 120], [720, 560]]) {
        ctx.fillStyle = "#2a2236"; ctx.fillRect(cx, cy, 60, 110);
        ctx.strokeStyle = "#0e0a14"; ctx.lineWidth = 3; ctx.strokeRect(cx, cy, 60, 110);
        for (let i = 0; i < 3; i++) { ctx.fillStyle = "#8a6ab0"; ctx.fillRect(cx + 24, cy + 18 + i * 34, 12, 4); }
      }
    } else if (theme === "escape") {
      const g = ctx.createRadialGradient(430, 120, 10, 430, 120, 360);
      g.addColorStop(0, "rgba(255,220,120,0.35)"); g.addColorStop(1, "rgba(255,220,120,0)");
      ctx.fillStyle = g; ctx.fillRect(WALL, WALL, ROOM_W - WALL * 2, ROOM_H - WALL * 2);
      ctx.strokeStyle = "rgba(232,194,90,0.6)"; ctx.lineWidth = 3;
      for (let i = 0; i < 3; i++) { ctx.beginPath(); ctx.arc(430, 320, 70 + i * 40, 0, 7); ctx.stroke(); }
      // Exit gate
      const open = allSolved();
      ctx.fillStyle = open ? "#f6dc8a" : "#3a2e18";
      rr(ctx, 360, 4, 140, 40, 4); ctx.fill();
      ctx.font = `bold 15px ${FONT}`; ctx.textAlign = "center"; ctx.textBaseline = "middle";
      ctx.fillStyle = open ? "#2a1c05" : "#e8c25a";
      ctx.fillText(open ? "EXIT OPEN" : "EXIT SEALED", 430, 25);
    }
    ctx.restore();
  }

  function drawWalls(th) {
    ctx.fillStyle = th.wall;
    ctx.fillRect(0, 0, ROOM_W, WALL);
    ctx.fillRect(0, ROOM_H - WALL, ROOM_W, WALL);
    ctx.fillRect(0, 0, WALL, ROOM_H);
    ctx.fillRect(ROOM_W - WALL, 0, WALL, ROOM_H);
    ctx.fillStyle = th.wallTop;
    for (let x = 0; x < ROOM_W; x += 44) { ctx.fillRect(x + 2, 4, 40, 14); ctx.fillRect(x + 24, 22, 40, 12); ctx.fillRect(x + 2, ROOM_H - 36, 40, 14); ctx.fillRect(x + 24, ROOM_H - 18, 40, 12); }
    for (let y = 0; y < ROOM_H; y += 44) { ctx.fillRect(4, y + 2, 14, 40); ctx.fillRect(22, y + 24, 12, 40); ctx.fillRect(ROOM_W - 36, y + 2, 14, 40); ctx.fillRect(ROOM_W - 18, y + 24, 12, 40); }
    ctx.strokeStyle = th.trim; ctx.lineWidth = 2;
    ctx.strokeRect(WALL - 1, WALL - 1, ROOM_W - WALL * 2 + 2, ROOM_H - WALL * 2 + 2);
    // torches
    for (const [tx, ty] of torchPositions()) {
      const f = 0.7 + 0.3 * Math.sin(fx.t * 13 + tx) * Math.sin(fx.t * 7 + ty);
      ctx.fillStyle = "#3a2a1a"; ctx.fillRect(tx - 4, ty - 4, 8, 8);
      ctx.fillStyle = `rgba(255,${150 + f * 60},60,${0.85})`;
      ctx.beginPath(); ctx.ellipse(tx, ty - 6, 5 * f + 2, 9 * f + 2, 0, 0, 7); ctx.fill();
      ctx.fillStyle = "rgba(255,250,200,0.9)";
      ctx.beginPath(); ctx.arc(tx, ty - 4, 2.5, 0, 7); ctx.fill();
    }
  }

  function drawDoor(e) {
    const open = doorOpen(e);
    const k = S.room + ":" + e.id;
    const a = fx.doorAnim[k] ?? (open ? 1 : 0);
    const horiz = e.side === "N" || e.side === "S";
    const w = horiz ? 110 : 40, h = horiz ? 40 : 110;
    const x = e.x - w / 2, y = e.y - h / 2;
    ctx.save();
    // frame
    ctx.fillStyle = "#0a0806"; ctx.fillRect(x - 6, y - 6, w + 12, h + 12);
    // opening (dark passage)
    ctx.fillStyle = open ? "#050403" : "#120d08"; ctx.fillRect(x, y, w, h);
    // sliding panels
    ctx.fillStyle = open ? "#3f4a3a" : "#5a3d26";
    const slide = a * (horiz ? w / 2 - 6 : h / 2 - 6);
    if (horiz) {
      ctx.fillRect(x, y, w / 2 - slide, h); ctx.fillRect(x + w / 2 + slide, y, w / 2 - slide, h);
      ctx.strokeStyle = "#1a120b"; ctx.lineWidth = 2;
      ctx.strokeRect(x + 1, y + 1, w / 2 - slide - 2, h - 2); ctx.strokeRect(x + w / 2 + slide + 1, y + 1, w / 2 - slide - 2, h - 2);
    } else {
      ctx.fillRect(x, y, w, h / 2 - slide); ctx.fillRect(x, y + h / 2 + slide, w, h / 2 - slide);
      ctx.strokeStyle = "#1a120b"; ctx.lineWidth = 2;
      ctx.strokeRect(x + 1, y + 1, w - 2, h / 2 - slide - 2); ctx.strokeRect(x + 1, y + h / 2 + slide + 1, w - 2, h / 2 - slide - 2);
    }
    // beacon
    const pulse = 0.6 + 0.4 * Math.sin(fx.t * 4);
    const bx = horiz ? e.x : (e.side === "E" ? x - 14 : x + w + 14);
    const by = horiz ? (e.side === "N" ? y + h + 14 : y - 14) : e.y - h / 2 - 4;
    ctx.fillStyle = open ? `rgba(80,255,140,${pulse})` : `rgba(255,60,60,${pulse})`;
    ctx.shadowColor = open ? "#50ff8c" : "#ff3c3c"; ctx.shadowBlur = 14;
    ctx.beginPath(); ctx.arc(bx, by, 5, 0, 7); ctx.fill();
    ctx.shadowBlur = 0;
    ctx.restore();
  }

  function labelAt(text, x, y, color = "#f3e9d2", size = 13) {
    ctx.font = `${size}px ${FONT}`;
    ctx.textAlign = "center"; ctx.textBaseline = "middle";
    const w = ctx.measureText(text).width + 14;
    ctx.fillStyle = "rgba(8,6,4,0.78)";
    rr(ctx, x - w / 2, y - size / 2 - 5, w, size + 10, 4); ctx.fill();
    ctx.fillStyle = color; ctx.fillText(text, x, y + 1);
  }

  function drawDoorLabel(e) {
    const open = doorOpen(e);
    let lx = e.x, ly = e.y;
    if (e.side === "N") ly = e.y + 48; else if (e.side === "S") ly = e.y - 48;
    else if (e.side === "E") { lx = e.x - 70; ly = e.y - 74; } else { lx = e.x + 70; ly = e.y - 74; }
    labelAt((open ? "" : "🔒 ") + e.label, lx, ly, open ? "#bff5cf" : "#ffb3aa", 12);
  }

  function drawPedestal(e) {
    const solved = isSolved(e.puzzle);
    const p = 0.5 + 0.5 * Math.sin(fx.t * 3);
    const col = solved ? "80,255,140" : "63,224,208";
    ctx.save();
    const g = ctx.createRadialGradient(e.x, e.y, 4, e.x, e.y, 70);
    g.addColorStop(0, `rgba(${col},${0.35 + p * 0.2})`); g.addColorStop(1, `rgba(${col},0)`);
    ctx.fillStyle = g; ctx.beginPath(); ctx.arc(e.x, e.y, 70, 0, 7); ctx.fill();
    ctx.fillStyle = "#2a2e36"; ctx.beginPath(); ctx.arc(e.x, e.y + 6, 30, 0, 7); ctx.fill();
    ctx.strokeStyle = "#11141a"; ctx.lineWidth = 4; ctx.stroke();
    ctx.fillStyle = "#3c424e"; ctx.beginPath(); ctx.arc(e.x, e.y, 22, 0, 7); ctx.fill();
    // hologram
    ctx.fillStyle = `rgba(${col},${0.5 + p * 0.4})`;
    ctx.shadowColor = `rgb(${col})`; ctx.shadowBlur = 18;
    const hy = e.y - 30 - p * 6;
    ctx.beginPath(); ctx.moveTo(e.x, hy - 16); ctx.lineTo(e.x + 12, hy); ctx.lineTo(e.x, hy + 16); ctx.lineTo(e.x - 12, hy); ctx.closePath(); ctx.fill();
    ctx.shadowBlur = 0;
    ctx.font = `bold 14px ${FONT}`; ctx.textAlign = "center"; ctx.textBaseline = "middle";
    ctx.fillStyle = "#0a1a1a"; ctx.fillText(solved ? "✓" : "?", e.x, hy + 1);
    ctx.restore();
  }

  function drawTrap(e) {
    const done = !!S.traps[e.key];
    const x = e.x - e.w / 2, y = e.y - e.h / 2;
    ctx.save();
    ctx.fillStyle = done ? "rgba(40,40,40,0.6)" : "rgba(20,16,12,0.85)";
    ctx.fillRect(x, y, e.w, e.h);
    ctx.strokeStyle = done ? "#555" : "#c9a030"; ctx.lineWidth = 3;
    ctx.setLineDash([10, 8]); ctx.strokeRect(x, y, e.w, e.h); ctx.setLineDash([]);
    if (e.kind === "spike") {
      ctx.fillStyle = done ? "#555" : "#a7adb8";
      for (let i = 0; i < 8; i++) for (let j = 0; j < 3; j++) {
        const sx = x + 12 + i * 19, sy = y + 14 + j * 20, up = done ? 0.3 : 0.6 + 0.4 * Math.sin(fx.t * 2 + i);
        ctx.beginPath(); ctx.moveTo(sx - 5, sy + 6); ctx.lineTo(sx, sy + 6 - 12 * up); ctx.lineTo(sx + 5, sy + 6); ctx.fill();
      }
    } else if (e.kind === "poison") {
      for (let i = 0; i < 6; i++) { ctx.fillStyle = "#2a2a22"; ctx.fillRect(x + 10 + i * 25, y + 10, 16, e.h - 20); }
      if (!done) for (let i = 0; i < 10; i++) {
        const ph = (fx.t * 0.6 + i * 0.37) % 1;
        ctx.fillStyle = `rgba(120,255,80,${0.5 * (1 - ph)})`;
        ctx.beginPath(); ctx.arc(x + 15 + ((i * 53) % (e.w - 30)), y + e.h - ph * 90, 8 + ph * 14, 0, 7); ctx.fill();
      }
    } else if (e.kind === "electric") {
      ctx.fillStyle = "#3a3a46"; ctx.beginPath(); ctx.arc(x + 18, e.y, 12, 0, 7); ctx.arc(x + e.w - 18, e.y, 12, 0, 7); ctx.fill();
      if (!done) {
        ctx.strokeStyle = `rgba(150,200,255,${0.6 + Math.random() * 0.4})`; ctx.lineWidth = 2;
        ctx.shadowColor = "#8cf"; ctx.shadowBlur = 12;
        for (let b = 0; b < 2; b++) {
          ctx.beginPath(); ctx.moveTo(x + 18, e.y);
          for (let i = 1; i < 10; i++) ctx.lineTo(x + 18 + ((e.w - 36) * i) / 10, e.y + rand(-22, 22));
          ctx.lineTo(x + e.w - 18, e.y); ctx.stroke();
        }
        ctx.shadowBlur = 0;
      }
    }
    if (!done) { ctx.font = `bold 11px ${FONT}`; ctx.fillStyle = "#e8c04a"; ctx.textAlign = "center"; ctx.fillText("⚠ HAZARD", e.x, y - 9); }
    ctx.restore();
  }

  function drawItem(e) {
    if (S.collected[e.key]) return;
    const bob = Math.sin(fx.t * 3 + e.x) * 4;
    const y = e.y + bob;
    ctx.save();
    const g = ctx.createRadialGradient(e.x, y, 2, e.x, y, 34);
    g.addColorStop(0, "rgba(120,240,255,0.45)"); g.addColorStop(1, "rgba(120,240,255,0)");
    ctx.fillStyle = g; ctx.beginPath(); ctx.arc(e.x, y, 34, 0, 7); ctx.fill();
    ctx.fillStyle = "rgba(0,0,0,0.35)"; ctx.beginPath(); ctx.ellipse(e.x, e.y + 18, 12, 4, 0, 0, 7); ctx.fill();
    drawItemIcon(e.name, e.x, y, 1);
    ctx.restore();
  }

  function drawItemIcon(name, x, y, s) {
    ctx.save(); ctx.translate(x, y); ctx.scale(s, s);
    if (name === "Health Potion") {
      ctx.fillStyle = "#e23a4a"; ctx.beginPath(); ctx.arc(0, 4, 10, 0, 7); ctx.fill();
      ctx.fillStyle = "#cfd6e0"; ctx.fillRect(-4, -12, 8, 8);
      ctx.fillStyle = "#8a5a2a"; ctx.fillRect(-5, -15, 10, 4);
      ctx.fillStyle = "rgba(255,255,255,0.6)"; ctx.beginPath(); ctx.arc(-4, 1, 3, 0, 7); ctx.fill();
    } else if (name === "Medkit") {
      ctx.fillStyle = "#f2f2f2"; rr(ctx, -13, -10, 26, 20, 3); ctx.fill();
      ctx.fillStyle = "#e23a3a"; ctx.fillRect(-3, -7, 6, 14); ctx.fillRect(-7, -3, 14, 6);
    } else if (name === "Clue Scroll") {
      ctx.fillStyle = "#e8d3a0"; ctx.fillRect(-10, -12, 20, 24);
      ctx.fillStyle = "#b8955a"; ctx.fillRect(-12, -14, 24, 5); ctx.fillRect(-12, 9, 24, 5);
      ctx.strokeStyle = "#7a5a30"; ctx.lineWidth = 1.5;
      for (let i = 0; i < 3; i++) { ctx.beginPath(); ctx.moveTo(-6, -4 + i * 5); ctx.lineTo(6, -4 + i * 5); ctx.stroke(); }
    } else {
      ctx.fillStyle = "#3a8ad8"; rr(ctx, -13, -9, 26, 18, 3); ctx.fill();
      ctx.fillStyle = "#e8c25a"; ctx.fillRect(-9, -4, 8, 6);
      ctx.fillStyle = "#cfe6ff"; ctx.fillRect(2, -4, 8, 2); ctx.fillRect(2, 1, 6, 2);
    }
    ctx.restore();
  }

  function drawPlayer() {
    const bob = Math.sin(fx.walk) * 2.5;
    const x = S.x, y = S.y + bob;
    ctx.save();
    ctx.fillStyle = "rgba(0,0,0,0.4)"; ctx.beginPath(); ctx.ellipse(S.x, S.y + 16, 14, 5, 0, 0, 7); ctx.fill();
    // legs
    const leg = Math.sin(fx.walk) * 5;
    ctx.fillStyle = "#2a2f3a";
    ctx.fillRect(x - 7, y + 6, 5, 10 + leg * 0.4); ctx.fillRect(x + 2, y + 6, 5, 10 - leg * 0.4);
    // body
    ctx.fillStyle = "#c8742a"; rr(ctx, x - 12, y - 10, 24, 20, 6); ctx.fill();
    ctx.strokeStyle = "#5a3010"; ctx.lineWidth = 2; ctx.stroke();
    // head
    ctx.fillStyle = "#f0c8a0"; ctx.beginPath(); ctx.arc(x, y - 17, 9, 0, 7); ctx.fill();
    ctx.fillStyle = "#3a2410"; ctx.beginPath(); ctx.arc(x, y - 20, 9, Math.PI, 0); ctx.fill();
    // eyes by facing
    ctx.fillStyle = "#1a1a1a";
    const ex = S.dir === "left" ? -3 : S.dir === "right" ? 3 : 0;
    if (S.dir !== "up") { ctx.fillRect(x - 4 + ex, y - 17, 2, 3); ctx.fillRect(x + 2 + ex, y - 17, 2, 3); }
    // flashlight
    ctx.fillStyle = "#ddd";
    const fl = { up: [8, -12], down: [-10, 6], left: [-14, 0], right: [12, 0] }[S.dir];
    ctx.fillRect(x + fl[0] - 2, y + fl[1] - 2, 5, 5);
    ctx.restore();
  }

  function drawLighting(th) {
    lctx.globalCompositeOperation = "source-over";
    lctx.clearRect(0, 0, ROOM_W, ROOM_H);
    lctx.fillStyle = S.room === EXIT_ROOM ? "rgba(4,3,2,0.55)" : "rgba(4,3,2,0.8)";
    lctx.fillRect(0, 0, ROOM_W, ROOM_H);
    lctx.globalCompositeOperation = "destination-out";
    const hole = (x, y, r, a) => {
      const g = lctx.createRadialGradient(x, y, 0, x, y, r);
      g.addColorStop(0, `rgba(0,0,0,${a})`); g.addColorStop(1, "rgba(0,0,0,0)");
      lctx.fillStyle = g; lctx.beginPath(); lctx.arc(x, y, r, 0, 7); lctx.fill();
    };
    hole(S.x, S.y, 170, 1);
    // flashlight cone
    const ang = { up: -Math.PI / 2, down: Math.PI / 2, left: Math.PI, right: 0 }[S.dir];
    const g = lctx.createRadialGradient(S.x, S.y, 20, S.x, S.y, 380);
    g.addColorStop(0, "rgba(0,0,0,0.95)"); g.addColorStop(1, "rgba(0,0,0,0)");
    lctx.fillStyle = g;
    lctx.beginPath(); lctx.moveTo(S.x, S.y); lctx.arc(S.x, S.y, 380, ang - 0.42, ang + 0.42); lctx.closePath(); lctx.fill();
    for (const [tx, ty] of torchPositions()) hole(tx, ty, 120 + Math.sin(fx.t * 9 + tx) * 6, 0.75);
    for (const e of entities()) {
      if (e.t === "pedestal") hole(e.x, e.y, 110, 0.7);
      else if (e.t === "door") hole(e.x, e.y, 90, 0.6);
      else if (e.t === "item" && !S.collected[e.key]) hole(e.x, e.y, 55, 0.6);
      else if (e.t === "trap" && !S.traps[e.key]) hole(e.x, e.y, 90, 0.35);
    }
    ctx.drawImage(light, 0, 0);
    // warm torch tint
    ctx.save(); ctx.globalCompositeOperation = "lighter";
    for (const [tx, ty] of torchPositions()) {
      const tg = ctx.createRadialGradient(tx, ty, 0, tx, ty, 110);
      tg.addColorStop(0, "rgba(255,150,60,0.16)"); tg.addColorStop(1, "rgba(255,150,60,0)");
      ctx.fillStyle = tg; ctx.beginPath(); ctx.arc(tx, ty, 110, 0, 7); ctx.fill();
    }
    ctx.restore();
  }

  function drawMotes(th) {
    const [r, g, b] = th.mote;
    for (const m of fx.motes) {
      ctx.fillStyle = `rgba(${r},${g},${b},${m.a * (0.6 + 0.4 * Math.sin(m.p))})`;
      ctx.beginPath(); ctx.arc(m.x, m.y, m.r, 0, 7); ctx.fill();
    }
  }

  function drawRoom() {
    const th = THEMES[ROOMS[S.room].theme];
    ctx.save();
    ctx.beginPath(); ctx.rect(0, 0, ROOM_W, ROOM_H); ctx.clip();
    if (fx.shake > 0) ctx.translate(rand(-6, 6) * fx.shake * 2.5, rand(-6, 6) * fx.shake * 2.5);
    drawFloor(th, ROOMS[S.room].theme);
    const ents = entities();
    ents.filter((e) => e.t === "trap").forEach(drawTrap);
    drawWalls(th);
    ents.filter((e) => e.t === "door").forEach(drawDoor);
    ents.filter((e) => e.t === "item").forEach(drawItem);
    // Depth-sort pedestals and player.
    const drawables = ents.filter((e) => e.t === "pedestal").map((e) => ({ y: e.y, d: () => drawPedestal(e) }));
    drawables.push({ y: S.y, d: drawPlayer });
    drawables.sort((a, b) => a.y - b.y).forEach((o) => o.d());
    drawMotes(th);
    drawLighting(th);
    ents.filter((e) => e.t === "door").forEach(drawDoorLabel);

    // Room title card
    ctx.font = `bold 15px ${FONT}`;
    ctx.textAlign = "left"; ctx.textBaseline = "middle";
    labelAt(`${ROOMS[S.room].name.toUpperCase()}  ·  ROOM ${S.room}`, 150, 70, "#f3d58a", 13);

    // Interaction prompt
    if (mode === "play") {
      const n = nearest();
      if (n) {
        let txt;
        if (n.t === "door") txt = doorOpen(n) ? `[E] Enter ${n.label}` : `[E] Locked: solve puzzle #${n.puzzle}`;
        else if (n.t === "pedestal") txt = isSolved(n.puzzle) ? `[E] ${n.name} (solved)` : `[E] Use terminal: ${n.name}`;
        else txt = `[E] Pick up ${n.name}`;
        labelAt(txt, S.x, S.y - 52, "#fff3c4", 14);
      }
    }

    // floating texts
    for (const f of fx.floats) {
      const a = 1 - f.t / 1.6;
      ctx.globalAlpha = Math.max(0, a);
      ctx.font = `bold 18px ${FONT}`; ctx.textAlign = "center";
      ctx.lineWidth = 4; ctx.strokeStyle = "rgba(0,0,0,0.8)";
      ctx.strokeText(f.text, f.x, f.y - f.t * 40);
      ctx.fillStyle = f.color; ctx.fillText(f.text, f.x, f.y - f.t * 40);
      ctx.globalAlpha = 1;
    }
    if (fx.flash > 0) { ctx.fillStyle = `rgba(0,0,0,${fx.flash})`; ctx.fillRect(0, 0, ROOM_W, ROOM_H); }
    ctx.restore();
  }

  // ----- HUD -----
  const HUD_X = ROOM_W;
  const HUD_W = VIEW_W - ROOM_W;
  function slotRect(i) { return { x: HUD_X + 24 + i * 76, y: 470, w: 66, h: 66 }; }

  const MAP_POS = { 0: [0.5, 0.9], 1: [0.2, 0.6], 2: [0.8, 0.6], 3: [0.5, 0.32], 4: [0.5, 0.06] };

  function drawHUD() {
    const x0 = HUD_X;
    const g = ctx.createLinearGradient(x0, 0, VIEW_W, 0);
    g.addColorStop(0, "#110d09"); g.addColorStop(1, "#0b0906");
    ctx.fillStyle = g; ctx.fillRect(x0, 0, HUD_W, VIEW_H);
    ctx.fillStyle = "#3a2c18"; ctx.fillRect(x0, 0, 2, VIEW_H);
    const pad = 24, L = x0 + pad, R = VIEW_W - pad, W = R - L;
    ctx.textBaseline = "alphabetic"; ctx.textAlign = "left";

    const section = (title, y) => {
      ctx.font = `11px ${FONT}`; ctx.fillStyle = "#8a7a5c";
      ctx.fillText(title.toUpperCase(), L, y);
      ctx.fillStyle = "#2a2015"; ctx.fillRect(L + ctx.measureText(title.toUpperCase()).width + 8, y - 4, W - ctx.measureText(title.toUpperCase()).width - 8, 1);
    };

    // Title
    ctx.font = `bold 20px ${FONT}`; ctx.fillStyle = "#e8b54a";
    ctx.fillText("ESCAPE ROOM", L, 42);
    ctx.font = `11px ${FONT}`; ctx.fillStyle = "#8a7a5c";
    ctx.fillText("INFINITE ROOMS · " + fmtTime(S.elapsed), L, 60);

    // Health
    section("Vitals", 96);
    const [stName, stCol] = healthStatus(S.hp);
    ctx.font = `14px ${FONT}`; ctx.fillStyle = "#f3e9d2"; ctx.fillText(`HP ${S.hp}/100`, L, 120);
    ctx.textAlign = "right"; ctx.fillStyle = stCol; ctx.fillText(`[${stName}]`, R, 120); ctx.textAlign = "left";
    ctx.fillStyle = "#1e1812"; rr(ctx, L, 130, W, 14, 4); ctx.fill();
    const pct = S.hp / 100;
    if (pct > 0) {
      const hg = ctx.createLinearGradient(L, 0, L + W, 0);
      hg.addColorStop(0, stCol); hg.addColorStop(1, "#ffffff");
      ctx.fillStyle = stCol; rr(ctx, L, 130, Math.max(8, W * pct), 14, 4); ctx.fill();
      ctx.fillStyle = "rgba(255,255,255,0.18)"; ctx.fillRect(L + 3, 132, Math.max(2, W * pct - 6), 4);
    }
    // score / moves / puzzles
    const solvedCount = Object.values(S.puzzles).filter((p) => p.solved).length;
    const stats = [["SCORE", S.score], ["MOVES", S.moves], ["PUZZLES", `${solvedCount}/4`]];
    stats.forEach(([k, v], i) => {
      const bx = L + i * (W / 3);
      ctx.fillStyle = "#17120c"; rr(ctx, bx, 158, W / 3 - 8, 50, 4); ctx.fill();
      ctx.font = `10px ${FONT}`; ctx.fillStyle = "#8a7a5c"; ctx.fillText(k, bx + 10, 175);
      ctx.font = `bold 20px ${FONT}`; ctx.fillStyle = "#f3e9d2"; ctx.fillText(String(v), bx + 10, 199);
    });

    // Mini-map
    section("Facility map · BFS route [M]", 238);
    const mx = L, my = 252, mw = W, mh = 180;
    ctx.fillStyle = "#0e0b08"; rr(ctx, mx, my, mw, mh, 6); ctx.fill();
    ctx.strokeStyle = "#2a2015"; ctx.lineWidth = 1; ctx.stroke();
    const pos = (id) => [mx + 30 + MAP_POS[id][0] * (mw - 60), my + 18 + MAP_POS[id][1] * (mh - 36)];
    const path = S.showPath ? bfsPath(S.room, EXIT_ROOM) : [];
    const onPath = (a, b) => { for (let i = 0; i < path.length - 1; i++) if ((path[i] === a && path[i + 1] === b) || (path[i] === b && path[i + 1] === a)) return true; return false; };
    for (const [a, b] of EDGES) {
      const [ax, ay] = pos(a), [bx, by] = pos(b);
      const hot = onPath(a, b);
      ctx.strokeStyle = hot ? `rgba(255,210,80,${0.6 + 0.4 * Math.sin(fx.t * 5)})` : "#3a3024";
      ctx.lineWidth = hot ? 4 : 2;
      if (hot) { ctx.shadowColor = "#ffd250"; ctx.shadowBlur = 10; }
      ctx.beginPath(); ctx.moveTo(ax, ay); ctx.lineTo(bx, by); ctx.stroke();
      ctx.shadowBlur = 0;
    }
    for (const id of Object.keys(ROOMS).map(Number)) {
      const [px, py] = pos(id);
      const visited = S.visited.includes(id);
      const cur = id === S.room;
      const door = Object.values(ENTITY_DEFS).flat().find((e) => e.t === "door" && e.dest === id && e.puzzle);
      const locked = door && !isSolved(door.puzzle);
      ctx.fillStyle = cur ? "#e8b54a" : visited ? "#5a4a30" : "#221b12";
      ctx.strokeStyle = locked ? "#ff5a4e" : "#6dffa8"; ctx.lineWidth = 2;
      rr(ctx, px - 13, py - 10, 26, 20, 4); ctx.fill(); ctx.stroke();
      ctx.font = `bold 11px ${FONT}`; ctx.textAlign = "center"; ctx.textBaseline = "middle";
      ctx.fillStyle = cur ? "#1a1206" : "#f3e9d2"; ctx.fillText(String(id), px, py + 1);
      ctx.font = `10px ${FONT}`; ctx.fillStyle = cur ? "#e8b54a" : "#8a7a5c";
      const short = ROOMS[id].name.replace("Secret ", "").replace(" Chamber", "").replace(" Hall", "");
      const lx = id === 1 ? px - 2 : id === 2 ? px + 2 : px + 18;
      ctx.textAlign = id === 1 ? "center" : id === 2 ? "center" : "left";
      ctx.fillText(short, id === 1 || id === 2 ? px : lx, id === 1 || id === 2 ? py + 20 : py + 1);
      ctx.textBaseline = "alphabetic"; ctx.textAlign = "left";
    }
    if (S.showPath && path.length) {
      ctx.font = `10px ${FONT}`; ctx.fillStyle = "#ffd250";
      ctx.fillText("Route: " + path.join(" → "), mx + 8, my + mh - 8);
    }

    // Inventory
    section("Inventory [1-4]", 458);
    for (let i = 0; i < 4; i++) {
      const s = slotRect(i);
      const it = S.inventory[i];
      ctx.fillStyle = it ? "#1c160e" : "#120e09"; rr(ctx, s.x, s.y, s.w, s.h, 6); ctx.fill();
      ctx.strokeStyle = it ? "#6a5430" : "#2a2015"; ctx.lineWidth = 1.5; ctx.stroke();
      ctx.font = `10px ${FONT}`; ctx.fillStyle = "#6a5c44"; ctx.fillText(String(i + 1), s.x + 6, s.y + 14);
      if (it) {
        drawItemIcon(it.name, s.x + s.w / 2, s.y + s.h / 2 - 2, 1.1);
        if (it.heal) { ctx.font = `bold 10px ${FONT}`; ctx.fillStyle = "#6dffa8"; ctx.textAlign = "right"; ctx.fillText("+" + it.heal, s.x + s.w - 5, s.y + s.h - 6); ctx.textAlign = "left"; }
      }
    }
    if (S.inventory.length > 4) { ctx.font = `10px ${FONT}`; ctx.fillStyle = "#8a7a5c"; ctx.fillText(`+${S.inventory.length - 4} more`, L, 552); }

    // Log
    section("Mission feed", 580);
    ctx.font = `12px ${FONT}`;
    let ly = 602;
    const lines = [];
    for (const entry of fx.log) {
      // word wrap
      const words = entry.msg.split(" ");
      let line = "";
      const wrapped = [];
      for (const w of words) {
        const test = line ? line + " " + w : w;
        if (ctx.measureText(test).width > W - 12) { wrapped.push(line); line = w; } else line = test;
      }
      if (line) wrapped.push(line);
      wrapped.forEach((t, i) => lines.push({ t: (i === 0 ? "› " : "  ") + t, c: entry.color }));
    }
    const visible = lines.slice(-11);
    visible.forEach((l, i) => {
      ctx.globalAlpha = 0.45 + 0.55 * ((i + 1) / visible.length);
      ctx.fillStyle = l.c; ctx.fillText(l.t, L, ly + i * 16);
    });
    ctx.globalAlpha = 1;

    ctx.font = `10px ${FONT}`; ctx.fillStyle = "#5a4e3a";
    ctx.fillText("E interact · 1-4 items · F5/F9 save/load · Esc pause", L, VIEW_H - 14);
  }

  function render() {
    ctx.clearRect(0, 0, VIEW_W, VIEW_H);
    drawRoom();
    drawHUD();
  }

  // ------------------------------------------------------------------
  // Responsive scaling
  // ------------------------------------------------------------------
  const stage = $("stage");
  function fit() {
    const margin = matchMedia("(pointer: coarse)").matches ? 8 : 24;
    const s = Math.min((window.innerWidth - margin * 2) / VIEW_W, (window.innerHeight - margin * 2) / VIEW_H);
    stage.style.transform = `scale(${s}) translate(-50%, -50%)`;
    stage.style.transformOrigin = "0 0";
    stage.style.left = "50%"; stage.style.top = "50%";
    stage.style.transform = `translate(-50%, -50%) scale(${s})`;
    stage.style.transformOrigin = "center center";
  }
  window.addEventListener("resize", fit);
  fit();

  // ------------------------------------------------------------------
  // Main loop
  // ------------------------------------------------------------------
  S = freshState();
  fx = freshFx();
  showTitle();
  let last = performance.now();
  function frame(now) {
    const dt = Math.min(0.05, (now - last) / 1000);
    last = now;
    update(dt);
    render();
    requestAnimationFrame(frame);
  }
  requestAnimationFrame(frame);
})();
