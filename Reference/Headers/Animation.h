#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void Play_Animation(_double TimeDelta, const vector<class CBone*>& Bones);

	_bool Play_Animation_Last(_double TimeDelta, const vector<class CBone*>& Bones, CAnimation* pAnim, _bool& bCheck);
	// TimEAcc 0으로 만드는 함수를 호출 해서 
	void Set_TimeAcc() { m_TimeAcc = 0.0; }
public:
	_double Get_Duration() { return m_Duration; }
	_double Get_TimeAcc() { return m_TimeAcc; }
	_double Get_TickPerSecond() { return m_TickPerSecond; }
	_bool Get_AnimCheck() { return m_Check; }
	void Set_AnimTick(_double TickPerSecond) { m_TickPerSecond = TickPerSecond; }

	_bool Get_AnimFinished() { return m_isFinished; }

	void Set_CurrKeyFrame();
	vector<class CChannel*> Get_Channel() { return m_vecChannel; }
private:
	char m_szName[MAX_PATH] = "";
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_SecondTick = { 0.0 };
	_double m_TimeAcc = { 0.0 };

	_double m_Ratio = { 8.0 };


	_bool m_isFinished = false;
	_bool m_isLoop = true;

	_bool m_Check = false;

private:
	_uint m_iNumChannels = { 0 };
	class CBone* m_vecBone;
	vector<class CChannel*> m_vecChannel;

	vector<_uint> m_CurrKeyFrame; // 애 자체가 다 0으로 초기화 되어야 한다
	vector<_uint> m_NextKeyFrame;

public:
	_uint Get_ChannelIndex() { return m_iChannelIndex; }
private:
	_uint m_iChannelIndex = 0;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual CAnimation* Clone();
	virtual void Free()override;
};

END