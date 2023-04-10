#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr;
	m_pSound = nullptr;

	ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

void CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	LoadSoundFile();
}


void CSoundMgr::SoundPlay(wstring pSoundKey, CHANNELID eID, float fVolume)
{
	map<wstring, FMOD::Sound*>::iterator iter;


	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return pSoundKey == iter.first;
	});

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;

	if (m_pChannelArr[eID]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannelArr[eID]);
	}

	m_pChannelArr[eID]->setVolume(fVolume);

}

void CSoundMgr::PlayBGM(wstring pSoundKey, float fVolume)
{
	map<wstring, FMOD::Sound*>::iterator iter;


	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return pSoundKey == iter.first;
	});

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;

	if (m_pChannelArr[SOUND_BGM]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannelArr[SOUND_BGM]);
	}

	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		(m_pChannelArr[i])->stop();
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
}

void CSoundMgr::Update_Sound(void)
{
	m_pSystem->update();
}

void CSoundMgr::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	long long handle = long long(_findfirst64("../../Client/Bin/Resources/Sound/*.*", &fd));

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[MAX_PATH] = "../../Client/Bin/Resources/Sound/";
	char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);


		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &m_pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, m_pSound);

			delete[] pSoundKey;
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext64(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

void CSoundMgr::Free(void)
{
	for (auto& Mypair : m_mapSound)
	{
		(Mypair.second)->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}