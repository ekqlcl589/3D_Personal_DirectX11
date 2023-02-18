#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);

public:
	void Play_Animation(_double TimeDelta);
	_double Get_Duration() { return m_Duration; }
	_double Get_TimeAcc() { return m_TimeAcc; }

private:
	char m_szName[MAX_PATH] = "";
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_TimeAcc = { 0.0 };

	_bool m_isFinished = false;
	_bool m_isLoop = true;

private:
	_uint m_iNumChannels = { 0 };
	vector<class CChannel*> m_vecChannel;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free()override;
};

END