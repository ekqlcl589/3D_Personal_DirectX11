#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel);

private:
	char m_szName[MAX_PATH] = "";

private: _uint m_iNumKeyFrames = { 0 };
		 vector<KEYFRAME> m_vecKeyFrame;

public:
	static CChannel* Create(aiNodeAnim* pAIChannel);
	virtual void Free()override;
};

END