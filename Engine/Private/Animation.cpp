#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
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

	return S_OK;
}

void CAnimation::Play_Animation(_double TimeDelta)
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
			m_isFinished = true;
		}
	}

	for (auto& pChannel : m_vecChannel)
		pChannel->Invalidate_Transform(m_TimeAcc);
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

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannel)
		Safe_Release(pChannel);

	m_vecChannel.clear();
}

