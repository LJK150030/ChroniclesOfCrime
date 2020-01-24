#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(const SpriteSheet& sheet, const int start_sprite_index,
                                           const int end_sprite_index,
                                           const float duration_seconds, const SpriteAnimPlaybackType playback_type):
	m_spriteSheet(sheet), m_startSpriteIndex(start_sprite_index),
	m_endSpriteIndex(end_sprite_index), m_durationSeconds(duration_seconds), m_playbackType(playback_type)
{
}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	//	SPRITE_ANIM_PLAYBACK_ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
	//	SPRITE_ANIM_PLAYBACK_LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
	//	SPRITE_ANIM_PLAYBACK_PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...
	// need to be able to calculate at time t, what frame would it be at?

	switch (m_playbackType)
	{
	case SPRITE_ANIM_PLAYBACK_ONCE:
		{
			const int base_frames = m_endSpriteIndex - m_startSpriteIndex + 1;
			const float fraction_completed = seconds / m_durationSeconds;
			const float current_frame_fraction = fraction_completed * static_cast<float>(base_frames);
			int current_frame = RoundDownToNearestInt(current_frame_fraction) + m_startSpriteIndex;
			current_frame = ClampInt(current_frame, m_startSpriteIndex, m_endSpriteIndex);
			return m_spriteSheet.GetSpriteDef(current_frame);
		}

	case SPRITE_ANIM_PLAYBACK_LOOP:
		{
			const int base_frames = m_endSpriteIndex - m_startSpriteIndex + 1;
			const float fraction_completed = seconds / m_durationSeconds;
			const float current_frame_fraction = fraction_completed * static_cast<float>(base_frames);
			int current_frame = RoundDownToNearestInt(current_frame_fraction);
			current_frame = ModPositive(current_frame, base_frames) + m_startSpriteIndex;
			return m_spriteSheet.GetSpriteDef(current_frame);
		}
	case SPRITE_ANIM_PLAYBACK_PINGPONG:
		{
			const int base_frames = m_endSpriteIndex - m_startSpriteIndex + 1;
			const int reverse_frame_count = (base_frames - 2);
			const int total_frames = base_frames + reverse_frame_count;

			const float fraction_completed = seconds / m_durationSeconds;
			const float fraction_first_half = static_cast<float>(base_frames) / static_cast<float>(total_frames);
			const float fraction_second_half = 1.0f - fraction_first_half;

			const float current_frame_fraction = fraction_completed * static_cast<float>(total_frames);
			int current_frame = RoundDownToNearestInt(current_frame_fraction);
			current_frame = ModPositive(current_frame, total_frames);

			if (current_frame < base_frames)
				current_frame += m_startSpriteIndex;
			else
				current_frame = m_endSpriteIndex - (current_frame - m_endSpriteIndex);

			return m_spriteSheet.GetSpriteDef(current_frame);
		}
	default:
		ERROR_AND_DIE( Stringf("Unsupported Playback Type in SpriteAnimDefinition"));
	}
}
