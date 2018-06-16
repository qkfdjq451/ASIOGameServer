#pragma once

enum class MoveType : char
{
	Start,
	Moving,
	Stop,
	None
};

enum class EResult : char
{
	LOW = -1,
	COMPLETE = 0,
	OVER = 1,
	None = 2
};

enum class MapType : char
{
	None,
	Town,
	Dungeon
};

enum class ESessionState :char
{
	Login,
	InGame,
	
	None
};