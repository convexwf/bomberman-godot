#ifndef BOMBERMAN_BOMB_H
#define BOMBERMAN_BOMB_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/**
 * Bomb placed by a player. Counts down then emits exploded signal.
 * Phase 1: no explosion logic; GDScript handles removal and effects.
 */
class Bomb : public Node2D {
	GDCLASS(Bomb, Node2D)

private:
	int grid_x = 0;
	int grid_y = 0;
	double timer = 0.0;
	double explosion_time = 2.0;
	int flame_range = 1;
	NodePath owner_path;
	bool has_exploded = false;

protected:
	static void _bind_methods();

public:
	Bomb();
	~Bomb();

	void _ready() override;
	void _process(double delta) override;

	void explode();

	void set_grid_x(int x);
	int get_grid_x() const;
	void set_grid_y(int y);
	int get_grid_y() const;
	void set_explosion_time(double p_time);
	double get_explosion_time() const;
	void set_flame_range(int p_range);
	int get_flame_range() const;
	void set_owner_path(const NodePath &p_path);
	NodePath get_owner_path() const;
	bool get_has_exploded() const;
};

} // namespace godot

#endif // BOMBERMAN_BOMB_H
