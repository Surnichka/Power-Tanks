#pragma once
#include <string>

namespace Signal
{
    namespace View
	{
	    static const std::string RequestLevelUp = "request_level_up_view";
		static const std::string RequestGamePlay = "request_game_play_view";
	}

	namespace Enemy
	{
	    static const std::string Died = "enemy_died";
		static const std::string GotHit = "enemy_got_hit";
	}

	namespace Bullet
	{
	    static const std::string Damage = "bullet_damage";
		static const std::string Speed = "bullet_speed";
		static const std::string FireRate = "fire_rate";
	}

	namespace Player
	{
	    static const std::string MoveUp = "player_move_up";
		static const std::string MoveDown = "player_move_down";
		static const std::string MoveLeft = "player_move_left";
		static const std::string MoveRight = "player_move_right";
		static const std::string Shoot = "player_shoot";
		static const std::string ShootUltimate = "player_shoot_supernova";
		static const std::string MoveSpeed = "move_speed";
		static const std::string LevelUp = "level_up";
	}
	static const std::string Pause = "pause_game";
}
