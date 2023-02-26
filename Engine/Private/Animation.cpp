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

	m_Duration = pAIAnimation->mDuration;

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

	if (m_TimeAcc >= m_Duration)
	{
		m_TimeAcc = 0.0;
		m_isFinished = true;
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

	// 꾹 누르는 애니메이션의 경우 누르는 동안 TimeAcc이 8초를 넘을 수 있어서 
	// 그런 애니메이션에 대한 예외처리가 필요함 
	if (m_TimeAcc >= 8.0)
	{
		Set_CurrKeyFrame();
		bCheck = false;

		return true;
	}
	
	else
	{
		m_iChannelIndex = 0;

		for (auto& pChannel : m_vecChannel)
		{
			pChannel->Linear_Transform(m_TimeAcc, &m_CurrKeyFrame[m_iChannelIndex], Bones, pAnim->Get_Channel(), bCheck);
		
			m_iChannelIndex += 1;
		}
	}

	return false;
}

void CAnimation::Set_CurrKeyFrame()
{
	for (auto& iter : m_CurrKeyFrame)
	{
		iter = 0;
	}
	m_isFinished = false;
	m_TimeAcc = 0.0;
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

