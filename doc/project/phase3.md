# Phase 3 实现记录（Polish）

记录项目在阶段 3 **做了哪些事**，以及本地如何验证（仅 Web）。

## Phase 3 做了哪些事

### 已实现

- **地图视觉（TileMap）**：与 GridManager 同位置、同格子的 TileMap；运行时生成 3 色 TileSet（地板 / 墙 / 可破坏砖），加载地图后 `_refresh_map_from_grid()` 同步显示；`tile_destroyed` 时将该格设为地板，可破坏砖被炸后**可见**清除。
- **HUD**：CanvasLayer 下 BombsLabel（`Bombs: 可用/容量`）、FlameLabel（`Flame: 范围`）；C++ `Player.get_active_bombs()` 用于显示已放置炸弹数，每帧更新。
- **Game Over**：玩家死亡后显示全屏 Game Over 层（Panel + Label + Restart 按钮），点击 Restart 重载当前场景。
- **音效**：未接资源，仅预留逻辑位；可在 `_on_bomb_exploded`、`_on_player_died`、`_on_power_up_collected` 中接入 AudioStreamPlayer。

### 阶段 1/2 回顾与遗漏

- **Phase 1 遗漏**  
  - **Map rendering**：设计中的「Map loading and rendering (TileMap same offset/tile size as C++)」在阶段 1 只做了 `load_map_from_string`，没有 TileMap 显示；**阶段 3 已补**：TileMap + `tile_destroyed` 同步。  
  - **load_map_from_file**：设计中有 `load_map_from_file`，当前未在 C++ 实现；可用 GDScript 读文件后调用 `grid_manager.load_map_from_string(FileAccess.get_file_as_string(path))` 等效实现。
- **Phase 2**：无遗漏；爆炸、格子破坏、玩家死亡、道具均已实现。

---

## 本地验证汇总（Phase 3，待验证时执行）

### Phase 3 检查项

- **TileMap**：运行后能看到地图（墙 / 地板 / 可破坏砖）；炸掉可破坏砖后，该格变为地板颜色。
- **HUD**：左上角显示 `Bombs: X/Y`、`Flame: Z`；放置炸弹后 X 减少，爆炸后恢复；收集道具后 Flame 或 Y 增加。
- **Game Over**：玩家被炸死后出现 Game Over 层，点击 Restart 重新开始本局。

### 检查清单（可勾选）

- [ ] Phase 3：地图以 TileMap 正确显示
- [ ] Phase 3：可破坏砖被炸后 TileMap 该格变地板
- [ ] Phase 3：HUD 炸弹数、火焰范围正确
- [ ] Phase 3：死亡后 Game Over 出现，Restart 可重载场景

与 Phase 1、2 的完整验证步骤仍见 [phase1-phase2.md](phase1-phase2.md)。
