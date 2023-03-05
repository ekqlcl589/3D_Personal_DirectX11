#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TimeAcc(rhs.m_TimeAcc)
	, m_isFinished(rhs.m_isFinished)
	, m_isLoop(rhs.m_isLoop)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_vecChannel(rhs.m_vecChannel)
	, m_CurrKeyFrame(rhs.m_CurrKeyFrame)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_vecChannel)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration * 0.9;

	m_SecondTick = m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannel.push_back(pChannel);
	}

	m_CurrKeyFrame.resize(m_iNumChannels);

	return S_OK;
}

void CAnimation::Play_Animation(_double TimeDelta, const vector<class CBone*>& Bones)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;
	m_isFinished = false;

	if (m_TimeAcc >= m_Duration)
	{
		Set_CurrKeyFrame();

	}

	m_iChannelIndex = 0;
	for (auto& pChannel : m_vecChannel)
	{
		pChannel->Invalidate_Transform(m_TimeAcc, &m_CurrKeyFrame[m_iChannelIndex], Bones);
		m_iChannelIndex += 1;
	}
		
}

_bool CAnimation::Play_Animation_Last(_double TimeDelta, const vector<class CBone*>& Bones, CAnimation* pAnim, _bool& bCheck)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;
	m_isLerpFinished = false;
	m_isFinished = false;

	if (m_TickPerSecond >= 25.0)
	{
		m_Ratio = 8.0;
		bCheck = false;

		//return true; // 주석 없었음
	}

	if(m_TimeAcc <= m_Ratio)
	{
		m_iChannelIndex = 0;

		for (auto& pChannel : m_vecChannel)
		{
			pChannel->Linear_Transform(m_TimeAcc, &m_CurrKeyFrame[m_iChannelIndex], Bones, pAnim->Get_Channel(), bCheck, m_TickPerSecond);
		
			m_iChannelIndex += 1;
		}
	}
	else
	{
		Reset();
		bCheck = false;

		return true;

	}

	return false;
}

void CAnimation::Set_TimeAcc()
{
	//for (auto& iter : m_CurrKeyFrame)
	//{
	//	iter = 0;
	//}
	m_TimeAcc = 0.0;
}


void CAnimation::Set_CurrKeyFrame()
{
	for (auto& iter : m_CurrKeyFrame)
	{
		iter = 0;
	}
	m_isFinished = true;
	//m_TimeAcc = 0.0;
}

void CAnimation::Reset()
{
	m_TimeAcc = 0.0;
	m_isLerpFinished = true;
	for (auto& iter : m_CurrKeyFrame)
	{
		iter = 0;
	}
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannel)
		Safe_Release(pChannel);

	m_vecChannel.clear();
}

