# Phase 1、Phase 2 实现记录

记录项目在阶段 1、阶段 2 **做了哪些事**，以及本地如何验证（仅 Web）。

**阶段 3** 见 [phase3.md](phase3.md)（TileMap、HUD、Game Over、阶段 1/2 遗漏说明）。

## 构建

```bash
./scripts/build/build.sh web template_debug
```

产物：`bomberman/gdextension/libbomberman.web.template_debug.wasm32.wasm`。

---

## Phase 1 做了哪些事

### 已实现

- GDExtension 信号/绑定验证：`Player.grid_position_changed`、`move_direction`、`place_bomb`、`on_bomb_exploded` 等。
- GridManager：`grid_to_world` / `world_to_grid`、格子状态、`load_map_from_string`。
- 玩家：格子对齐移动、炸弹放置（GDScript 创建 Bomb 节点）。
- 地图：字符串加载、默认地图。
- 炸弹：放置、计时、`exploded` 信号（Phase 1 仅通知，无爆炸范围逻辑）。

### 本地验证要点

- **信号**：方向键移动玩家，控制台应出现 `[Phase 1] grid_position_changed: (x, y)`。
- **绑定方法**：用空格放置炸弹，等待约 2 秒爆炸，无报错；炸弹数量/冷却表现正常。

---

## Phase 2 做了哪些事

### 已实现

- **炸弹爆炸**：`Bomb.explode()` 计算 `get_explosion_tiles()`，沿四向扩散，遇墙停止，遇可破坏砖则破坏并停止；通过 `GridManager.destroy_tile` 更新格子。
- **格子破坏**：`GridManager` 在 `destroy_tile` 时发出 `tile_destroyed(x, y)`，供 GDScript 更新 TileMap 或生成道具。
- **玩家受伤/死亡**：`Player.die()` / `take_damage()`，发出 `died`；GDScript 在炸弹 `exploded(grid_x, grid_y, tiles)` 时若玩家格子在 `tiles` 内则调用 `player.die()`。
- **道具系统**：`PowerUp`（C++ Area2D），类型枚举（FLAME_UP、BOMB_UP 等），`tile_destroyed` 时概率生成，玩家进入区域触发 `collected(player)`，GDScript 增加火焰范围或炸弹数量。

### 本地验证要点

- **爆炸与格子破坏**：放置炸弹后站在爆炸范围外，等待爆炸；可破坏砖（地图中的 `x`）应被清除（若已用 TileMap 接 `tile_destroyed` 更新显示，则能看到格子变空）。
- **玩家死亡**：放置炸弹后站在爆炸格子上或走入爆炸范围，等待爆炸；控制台应出现 `[Phase 2] player died`，玩家不再能移动。
- **道具生成与收集**：破坏可破坏砖后，有概率在该格生成 PowerUp；玩家移动到该格，应触发收集（火焰范围或炸弹数量增加），控制台无报错。

---

## 本地验证汇总（待验证时执行）

构建与代码已就绪后，以下步骤需在本地 Godot + 浏览器中执行（暂时无法验证时可先跳过，之后按此清单一次性补验）。

### 1. 环境与构建

1. **构建 Web 扩展**  
   ```bash
   ./scripts/build/build.sh web template_debug
   ```  
   确认产物存在：`bomberman/gdextension/libbomberman.web.template_debug.wasm32.wasm`。

2. **打开项目**  
   用 Godot 4.5+ 打开仓库中的 `bomberman/project.godot`。

3. **配置 Web 导出**  
   - 菜单 **Project → Export…**  
   - 若无 Web 预设：**Add… → Web**，保存（如保存到 `export_presets.cfg`）。  
   - 导出路径可按习惯设置（例如 `bomberman/export/web/`）。

4. **验证 Web 扩展加载**  
   - 导出 → 选择 **Web** → 导出项目（或导出并运行）。  
   - 在浏览器中打开导出的页面。  
   - 打开浏览器开发者工具控制台，确认无 GDExtension 加载错误（无报红、无 “failed to load” 等）。

### 2. Phase 1 检查项

- **信号**：方向键移动玩家，控制台应出现 `[Phase 1] grid_position_changed: (x, y)`。
- **绑定方法**：用空格放置炸弹，等待约 2 秒爆炸，无报错；炸弹数量/冷却表现正常。

### 3. Phase 2 检查项

- **爆炸与格子破坏**：放置炸弹后站在爆炸范围外，等待爆炸；可破坏砖应被清除（或 `tile_destroyed` 正常）。
- **玩家死亡**：放置炸弹后站在爆炸格子上或走入爆炸范围，等待爆炸；控制台应出现 `[Phase 2] player died`，玩家不再能移动。
- **道具生成与收集**：破坏可破坏砖后，有概率生成 PowerUp；玩家移动到该格应触发收集，能力提升，控制台无报错。

### 4. 检查清单（可勾选）

- [ ] `./scripts/build/build.sh web template_debug` 构建成功
- [ ] `bomberman/gdextension/libbomberman.web.template_debug.wasm32.wasm` 存在
- [ ] Godot 导出 Web 并在浏览器中运行，无 GDExtension 加载错误
- [ ] Phase 1：移动时 `grid_position_changed` 有输出
- [ ] Phase 1：放置/爆炸炸弹无报错
- [ ] Phase 2：爆炸清除可破坏砖（或 `tile_destroyed` 正常）
- [ ] Phase 2：玩家在爆炸范围内会死亡并打印 `[Phase 2] player died`
- [ ] Phase 2：破坏砖后有机会出现道具，收集后能力提升

全部通过后，可在上述清单中勾选对应项。

---

## 阶段 1/2 回顾与遗漏

- **Phase 1**  
  - **Map rendering**：设计中的「Map loading and rendering (TileMap same offset/tile size)」在阶段 1 只做了 `load_map_from_string`，无 TileMap 显示；**阶段 3 已补**（见 [phase3.md](phase3.md)）。  
  - **load_map_from_file**：未在 C++ 实现；可用 GDScript 读文件后调用 `load_map_from_string` 等效实现。  
- **Phase 2**：无遗漏。
