#ifndef BOMBERMAN_POWER_UP_H
#define BOMBERMAN_POWER_UP_H

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

class Player;

/**
 * Collectible power-up. Emits "collected" when a Player body enters.
 * Type is exposed as integer (PowerUpType enum); GDScript spawns and places.
 */
class PowerUp : public Area2D {
	GDCLASS(PowerUp, Area2D)

public:
	enum PowerUpType {
		TYPE_FLAME_UP = 0,
		TYPE_BOMB_UP = 1,
		TYPE_SPEED_UP = 2,
		TYPE_KICK = 3,
		TYPE_REMOTE_DETONATOR = 4,
	};

private:
	int type = TYPE_FLAME_UP;
	int grid_x = 0;
	int grid_y = 0;

	void _on_body_entered(const Variant &body_v);

protected:
	static void _bind_methods();

public:
	PowerUp();
	~PowerUp();

	void _ready() override;

	void set_type(int p_type);
	int get_type() const;
	void set_grid_x(int x);
	int get_grid_x() const;
	void set_grid_y(int y);
	int get_grid_y() const;
	void set_grid_position(int x, int y);
};

} // namespace godot

#endif // BOMBERMAN_POWER_UP_H
