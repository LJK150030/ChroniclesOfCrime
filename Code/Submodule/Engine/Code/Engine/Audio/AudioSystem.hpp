#pragma once


//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


struct Vec3;
//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = static_cast<size_t>(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
class AudioSystem;


/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

public:
	virtual void BeginFrame();
	virtual void EndFrame();

	virtual SoundID CreateOrGetSound(const std::string& sound_file_path, bool is_3D = false, 
		bool is_looping = false, bool is_streaming = false);
	virtual SoundPlaybackID PlaySound(SoundID sound_id, bool is_looped = false, float volume = 1.f,
	                                  float balance = 0.0f,
	                                  float speed = 1.0f, bool is_paused = false);
	virtual SoundPlaybackID Play3DSound(SoundID sound_id, const Vec3& pos, const Vec3& vel, bool is_looped = false, 
		float volume = 1.f, float balance = 0.0f, float speed = 1.0f, 
		bool is_paused = false);
	virtual void StopSound(SoundPlaybackID sound_playback_id);
	virtual void SetSoundPlaybackVolume(SoundPlaybackID sound_playback_id, float volume); // volume is in [0,1]
	virtual void SetSoundPlaybackBalance(SoundPlaybackID sound_playback_id, float balance);
	// balance is in [-1,1], where 0 is L/R centered
	virtual void SetSoundPlaybackSpeed(SoundPlaybackID sound_playback_id, float speed);
	// speed is frequency multiplier (1.0 == normal)

	virtual void ValidateResult(FMOD_RESULT result);
	virtual void Set3DNumListeners(int num_listeners);
	virtual void Set3DListenerAttributes(int listener_id, const Vec3& pos, const Vec3& velocity, const Vec3& forward, const Vec3& up);
	virtual FMOD_VECTOR Vec3ToFmodVector(const Vec3& v_position);


protected:
	FMOD::System* m_fmodSystem;
	std::map<std::string, SoundID> m_registeredSoundIDs;
	std::vector<FMOD::Sound*> m_registeredSounds;
};
