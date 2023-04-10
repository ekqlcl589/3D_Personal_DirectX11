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

	// ���� ���ҽ� ������ ���� ��ü 
	map<wstring, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel*      m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD::System*      m_pSystem{ nullptr };
	FMOD::Sound*      m_pSound{ nullptr };



public:
	virtual   void Free(void)   override;
};

END
