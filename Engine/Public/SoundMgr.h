#pragma once
#include "Base.h"
#include "Engine_Defines.h"


BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Initialize();

public:
	void SoundPlay(wstring pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(wstring pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
	void Update_Sound(void);

private:
	void LoadSoundFile();

	// 사운드 리소스 정보를 갖는 객체 
	map<wstring, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel*      m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System*      m_pSystem{ nullptr };
	FMOD::Sound*      m_pSound{ nullptr };



public:
	virtual   void Free(void)   override;
};

END
