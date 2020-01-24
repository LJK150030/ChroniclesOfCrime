#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem(nullptr)
{
	FMOD_RESULT result;
	result = System_Create(&m_fmodSystem);
	ValidateResult(result);

	result = m_fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
	ValidateResult(result);
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult(result);

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound(const std::string& soundFilePath, const bool is_3D, 
	const bool is_looping, const bool is_streaming)
{
	std::map<std::string, SoundID>::iterator found = m_registeredSoundIDs.find(soundFilePath);
	if (found != m_registeredSoundIDs.end())
	{
		return found->second;
	}

	unsigned int options = FMOD_DEFAULT;

	if (is_3D)
	{
		options |= FMOD_3D;
	}

	if (is_looping) 
	{
		options |= FMOD_LOOP_NORMAL;  
	}

	if (is_streaming) 
	{
		options |= FMOD_CREATESTREAM; 
	}

	FMOD::Sound* newSound = nullptr;
	m_fmodSystem->createSound(soundFilePath.c_str(), options, nullptr, &newSound);
	if (newSound)
	{
		SoundID newSoundID = m_registeredSounds.size();
		m_registeredSoundIDs[soundFilePath] = newSoundID;
		m_registeredSounds.push_back(newSound);
		return newSoundID;
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound(SoundID soundID, bool isLooped, float volume, float balance, float speed,
                                       bool isPaused)
{
	size_t numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound(sound, nullptr, isPaused, &channelAssignedToSound);
	if (channelAssignedToSound)
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency(&frequency);
		channelAssignedToSound->setFrequency(frequency * speed);
		channelAssignedToSound->setVolume(volume);
		channelAssignedToSound->setPan(balance);
		channelAssignedToSound->setLoopCount(loopCount);
	}

	return (SoundPlaybackID)channelAssignedToSound;
}

SoundPlaybackID AudioSystem::Play3DSound(const SoundID sound_id, const Vec3& pos, const Vec3& vel, const bool is_looped,
	const float volume, const float balance, const float speed, const bool is_paused)
{
	size_t numSounds = m_registeredSounds.size();
	if (sound_id < 0 || sound_id >= numSounds)
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[sound_id];
	if (!sound)
		return MISSING_SOUND_ID;

	FMOD::Channel* channel_assigned_to_sound = nullptr;
	m_fmodSystem->playSound(sound, nullptr, is_paused, &channel_assigned_to_sound);
	channel_assigned_to_sound->set3DAttributes(&Vec3ToFmodVector(pos), &Vec3ToFmodVector(vel));
	channel_assigned_to_sound->setMode(FMOD_3D_INVERSEROLLOFF);
	channel_assigned_to_sound->set3DMinMaxDistance(1.0f, 1000.0f);
	if (channel_assigned_to_sound)
	{
		int loopCount = is_looped ? -1 : 0;
		unsigned int playbackMode = is_looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channel_assigned_to_sound->setMode(playbackMode);
		channel_assigned_to_sound->getFrequency(&frequency);
		channel_assigned_to_sound->setFrequency(frequency * speed);
		channel_assigned_to_sound->setVolume(volume);
		channel_assigned_to_sound->setPan(balance);
		channel_assigned_to_sound->setLoopCount(loopCount);
	}

	return reinterpret_cast<SoundPlaybackID>(channel_assigned_to_sound);
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound(SoundPlaybackID soundPlaybackID)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*)soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume(SoundPlaybackID soundPlaybackID, float volume)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*)soundPlaybackID;
	channelAssignedToSound->setVolume(volume);
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance(SoundPlaybackID soundPlaybackID, float balance)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*)soundPlaybackID;
	channelAssignedToSound->setPan(balance);
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed(SoundPlaybackID soundPlaybackID, float speed)
{
	if (soundPlaybackID == MISSING_SOUND_ID)
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*)soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound(&currentSound);
	if (!currentSound)
		return;

	int ignored = 0;
	currentSound->getDefaults(&frequency, &ignored);
	channelAssignedToSound->setFrequency(frequency * speed);
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		ERROR_RECOVERABLE( Stringf(
			"Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result
		) );
	}
}

void AudioSystem::Set3DNumListeners(const int num_listeners)
{
	FMOD_RESULT setup_listeners = m_fmodSystem->set3DNumListeners(num_listeners);
	ValidateResult(setup_listeners);
}

void AudioSystem::Set3DListenerAttributes(const int listener_id, const Vec3& pos, const Vec3& velocity,
	const Vec3& forward, const Vec3& up)
{
	FMOD_VECTOR fmod_position = Vec3ToFmodVector(pos);
	FMOD_VECTOR fmod_velocity = Vec3ToFmodVector(velocity);
	FMOD_VECTOR fmod_forward = Vec3ToFmodVector(forward);
	FMOD_VECTOR fmod_up = Vec3ToFmodVector(up);
	FMOD_RESULT setup_listener_attributes = m_fmodSystem->set3DListenerAttributes(listener_id, 
		&fmod_position, &fmod_velocity, 	&fmod_forward, &fmod_up);
	ValidateResult(setup_listener_attributes);
}

FMOD_VECTOR AudioSystem::Vec3ToFmodVector(const Vec3& v_position)
{
	FMOD_VECTOR f_vec;
	f_vec.x = v_position.x;
	f_vec.y = v_position.y;
	f_vec.z = v_position.z;
	return f_vec;
}

#endif // !defined( ENGINE_DISABLE_AUDIO )
