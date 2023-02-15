#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation);

private:
	char m_szName[MAX_PATH] = "";
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };

private:
	_uint m_iNumChannels = { 0 };
	vector<class CChannel*> m_vecChannel;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	virtual void Free()override;
};

END