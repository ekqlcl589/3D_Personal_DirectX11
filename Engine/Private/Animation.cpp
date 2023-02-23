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

	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

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
		if (true == m_isLoop)
		{
			m_TimeAcc = 0.0;
			m_isFinished = true;
		}
		else
		{
			m_TimeAcc = 0.0;
			m_isFinished = true;
		}
			m_Check = true;
	}
	else
		m_Check = false;

	if (false == m_isFinished || true == m_isLoop)
	{
		_uint iChannelIndex = 0;

		for (auto& pChannel : m_vecChannel)
		{
			m_NextKeyFrame = m_CurrKeyFrame;
			pChannel->Invalidate_Transform(m_TimeAcc, &m_CurrKeyFrame[iChannelIndex++], Bones);
			//pChannel->Linear_Transform(m_TimeAcc, &m_CurrKeyFrame[iChannelIndex++], &m_NextKeyFrame[iChannelIndex], Bones);
		}

		
	}

	if (true == m_isLoop)
		m_isFinished = false;
}

_bool CAnimation::Play_Animation_Last(_double TimeDelta, _double CheckTime, const vector<class CBone*>& Bones)
{
	CheckTime = m_TimeAcc;

	return m_Check;
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

