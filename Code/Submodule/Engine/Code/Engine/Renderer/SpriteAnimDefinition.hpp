#pragma once

class SpriteSheet;
class SpriteDefinition;

enum SpriteAnimPlaybackType
{
	NO_PLAYBACK = -1,

	SPRITE_ANIM_PLAYBACK_ONCE,
	// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
	SPRITE_ANIM_PLAYBACK_LOOP,
	// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
	SPRITE_ANIM_PLAYBACK_PINGPONG,
	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...
	NUM_SPRITE_ANIM_PLAYBACK_TYPES
};

//------------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(const SpriteSheet& sheet, int start_sprite_index, int end_sprite_index,
	                     float duration_seconds, SpriteAnimPlaybackType playback_type = SPRITE_ANIM_PLAYBACK_LOOP);

	const SpriteDefinition& GetSpriteDefAtTime(float seconds) const;

private:
	const SpriteSheet& m_spriteSheet;
	int m_startSpriteIndex = -1;
	int m_endSpriteIndex = -1;
	float m_durationSeconds = 1.f;
	SpriteAnimPlaybackType m_playbackType = NO_PLAYBACK;
};
