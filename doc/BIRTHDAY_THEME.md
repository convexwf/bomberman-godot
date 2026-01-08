# Birthday Theme – Gift Mode Design

This document describes the **birthday theme** (gift mode) for the Bomberman project: config-driven personalization, BGM/sound, assets, and reuse workflow. It extends the main design (see [TECHNICAL_DESIGN.md](TECHNICAL_DESIGN.md)) without changing core game logic. Compatibility with the main design is ensured via **configuration and theme extension points** (see Section 2.3 and Section 5.3 of the main design).

---

## 1. Overview

### 1.1 Purpose
- Use the game as a **birthday gift** for a friend (casual friend; reusable for different friends).
- Keep the core Bomberman experience; add **personalization** (name, message, date) and **birthday-themed** visuals and audio.
- **Reuse**: one codebase, swap config (and optionally assets) per friend to produce a tailored build or link.

### 1.2 Design Principles
- **Config-driven**: All gift-specific content (name, message, BGM choice, theme id) comes from configuration so no code changes are needed per friend.
- **Pluggable**: Birthday theme plugs into the main design’s config and theme hooks (config source, theme id, BGM/skin paths).
- **Low scope**: A few clear touches (name on screen, one message, birthday BGM, optional candles/gifts) are enough for a strong gift feel.

---

## 2. Configuration (Gift Config)

### 2.1 Gift Config Schema
The main game should support a **gift/theme config** (see main design: Configuration and theme compatibility). For the birthday theme, the config can look like:

| Field | Type | Description |
|-------|------|-------------|
| `theme_id` | string | e.g. `"default"` or `"birthday"`. Drives which assets and BGM set to load. |
| `friend_name` | string | Name shown in title, victory, and dedication. |
| `birthday_date` | string | Optional; e.g. `"2025-03-15"` for display. |
| `message` | string | Short dedication or blessing (e.g. one line). |
| `sender` | string | Optional; e.g. your name for "From [sender]". |
| `bgm_track` | string | Resource path or id for main BGM (e.g. birthday song). |
| `bgm_menu` | string | Optional; BGM for menu (can be same as `bgm_track` or a calmer variant). |
| `show_dedication` | bool | Whether to show the dedication line (e.g. "To [name], from [sender]"). |

Example (JSON):

```json
{
  "theme_id": "birthday",
  "friend_name": "Alex",
  "birthday_date": "2025-03-15",
  "message": "Hope your day is full of explosions and cake!",
  "sender": "Me",
  "bgm_track": "res://audio/bgm/birthday_song.ogg",
  "bgm_menu": "res://audio/bgm/birthday_song_calm.ogg",
  "show_dedication": true
}
```

### 2.2 Where Config Is Used
- **Main menu / title**: e.g. "Happy Birthday, [friend_name]!" or "Bomberman for [friend_name]".
- **Victory / game over**: e.g. "Happy Birthday, [friend_name]! You won!" and optionally the short message.
- **Dedication line**: One line at start or end, e.g. "To [friend_name], from [sender] – [birthday_date]." Built from config; only shown if `show_dedication` is true.
- **Theme id**: Used to load birthday-themed assets (skins, tiles, particles) and BGM set (see below).
- **BGM**: Menu and gameplay use the paths/ids from `bgm_track` and `bgm_menu` when theme is birthday.

---

## 3. BGM and Sound

### 3.1 BGM Strategy
- **Default theme**: Use normal Bomberman-style BGM (e.g. action/arcade).
- **Birthday theme**: Switch to birthday-style BGM; main design should support **per-theme or per-config BGM selection** (see main design Section 5.3).

### 3.2 Birthday BGM Options
- **Main track (gameplay)**  
  - **Happy Birthday (traditional)**: Instrumental or chiptune version so it’s recognizable but not vocal-heavy.  
  - **Variations**:  
    - Upbeat / party (faster, more percussion).  
    - Calm / lounge (slower, for menu or post-victory).  
  - **Loop**: Edit the track so it loops seamlessly for long play sessions.

- **Menu / title**  
  - Same melody as main track but **calmer** (e.g. piano, soft synths) or shorter loop.  
  - Config: `bgm_menu` can point to this variant.

- **Victory / first win**  
  - **Short jingle**: 5–10 seconds of “Happy Birthday” climax or a fanfare derived from it.  
  - Play once when the player wins for the first time in a session (or once per “gift” run).  
  - Optional: after jingle, fade back to menu BGM or calm birthday track.

### 3.3 BGM Implementation Notes
- **Format**: Prefer OGG for Godot; keep file size reasonable for web export.
- **Layers**: If feasible, one base loop + optional layers (e.g. percussion) so the same theme can feel different in menu vs gameplay.
- **Volume**: Config or project setting for BGM volume so gift builds can default to a comfortable level.
- **Fallback**: If `bgm_track` or `bgm_menu` is missing, fall back to default theme BGM so the game never breaks.

### 3.4 SFX (Birthday Theme)
- **Explosion**: Optionally swap or mix with a **party popper / confetti** sound for birthday theme.
- **Power-up / collect**: Light **party** or **present** sound.
- **Candle collect** (if you add candles): Short **blow** or **spark** sound.
- **Victory**: **Applause** or **short birthday jingle** (can be the same as victory BGM jingle).
- All SFX should be selectable by `theme_id` (e.g. load from `res://audio/sfx/birthday/` when theme is birthday).

---

## 4. Visual and Asset Theming

### 4.1 Birthday-Themed Assets
- **Bombs**: Visual option as **birthday candles** or **gift boxes** (same logic, different sprite/animation).
- **Explosions**: **Confetti / sparkles** particle (or mixed with fire) when theme is birthday.
- **Destructible blocks**: Optionally **gift boxes**; when destroyed, drop power-up or show a short message (e.g. "For you, [friend_name]!" from config).
- **Background / tiles**: Optional birthday palette or subtle pattern (balloons, cake silhouette); keep grid readable.

### 4.2 Where to Show Name and Message
- **Title screen**: Large "Happy Birthday, [friend_name]!" or "Bomberman for [friend_name]".
- **Dedication line**: One line at bottom of title or after victory: "To [friend_name], from [sender] – [message] or [date]."
- **Victory screen**: "Happy Birthday, [friend_name]! You won!" plus optional `message`.
- **Optional**: In-game HUD or rare gift block tooltip showing `message` or name.

---

## 5. Optional Gameplay Touches

### 5.1 Candles as Collectibles
- **Idea**: A few **candle** pickups on the map (like optional collectibles). When the player collects all, show "Happy Birthday, [friend_name]!" or unlock a small reward.
- **Implementation**: Same as other collectibles (count, compare to total); only theme and text differ.

### 5.2 Gift Blocks
- **Idea**: Some destructible blocks look like **gift boxes**. When destroyed, drop power-up or display one line from config (e.g. `message` or "For you, [friend_name]!").
- **Implementation**: Destructible block type with optional “message on destroy” and theme-specific sprite.

---

## 6. Reuse Workflow (Different Friends)

### 6.1 Single Codebase
- One project; **no code changes** per friend.
- All personalization and theme selection via **gift config** (and theme id).

### 6.2 Ways to Ship
- **Option A – Config file**: Ship the game + a default `gift_config.json`. Friend (or you) edits `friend_name`, `message`, `sender`, `birthday_date`; theme_id set to `"birthday"` and BGM paths already point to birthday assets.
- **Option B – Build-time / pack-time**: When building for a specific friend, pass config (e.g. env var or file path); build script embeds this config into the project or packs it as the default. Output: e.g. `BombermanFor_Alex.zip` or `BombermanFor_Alex.pck`.
- **Option C – Web**: For web export, e.g. `?name=Alex&theme=birthday` or `?config=alex.json` so one URL per friend or one build that loads config from query string or external JSON.

### 6.3 Checklist per Friend
- [ ] Set `friend_name`, `message`, `sender`, `birthday_date` in gift config.
- [ ] Ensure `theme_id` is `"birthday"` and BGM paths are valid.
- [ ] (Optional) Replace or add a custom BGM file for this friend.
- [ ] Build or pack with that config; test title, victory, and dedication line.
- [ ] Ship the build or link.

---

## 7. Compatibility With Main Design

- **Config**: Main design (Section 5.3) defines a **single config source** (e.g. project config or resource) and optional **theme/gift config path**. Birthday theme uses this for all fields in Section 2.
- **Theme id**: Main design supports a **theme id** (e.g. `default` vs `birthday`) so the game can load the correct BGM set, SFX set, and asset paths; birthday theme sets `theme_id` to `"birthday"`.
- **BGM**: Main design reserves **BGM selection by config/theme** (menu vs gameplay track, fallback). Birthday theme only supplies the birthday tracks and references them in gift config.
- **UI strings**: Main design should **read title, victory, and dedication strings from config** (or from a resource populated from config) so birthday theme can override them without code changes.

This keeps the main design theme-agnostic and allows the birthday theme to be a pure content + config layer on top.
