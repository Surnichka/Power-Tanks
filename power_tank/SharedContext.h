#pragma once
#include <string>
#include "libs/GenericMsg/Msg.h"

enum class Property
{
	Damage,
	Health,
	AttackSpeed,
	MovementSpeed,
	BulletFireRate,
	BulletSpeed,
	BulletDamage,
	CriticalChance,
	CriticalDamage,
	PlayerLevel,
	AbilityPoint,
	Highscore,
	UltimateCooldown
};

class SharedContext
{
public:
	template<typename T>
	void Add(Property name, T value)
	{
		size_t nameInt = static_cast<size_t>(name);
		std::string nameStr = std::to_string(nameInt);
		player.AddValue(nameStr, value);
	}

	int Get(Property name)
	{
		return Get<int>(name);
	}

	template<typename T>
	T Get(Property name)
	{
		size_t nameInt = static_cast<size_t>(name);
		std::string nameStr = std::to_string(nameInt);
		T result;
		player.GetValue(nameStr, result);
		return result;
	}

private:
	msg::UniMsg player;
};

inline SharedContext& GetSharedContext()
{
	static SharedContext shr;
	return shr;
}
