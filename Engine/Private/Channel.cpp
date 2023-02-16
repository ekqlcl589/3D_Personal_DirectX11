#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}


HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_pBone = pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pBone);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys, pAIChannel->mNumPositionKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3 vScale;
	_float4 vRotation;
	_float3 vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME KeyFrame;
		ZeroMemory(&KeyFrame, sizeof KeyFrame);

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_vecKeyFrame.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_Transform(_double TrackPosition)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	KEYFRAME LastKeyFrame = m_vecKeyFrame.back();
	if (TrackPosition >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	else
	{
		if (TrackPosition >= m_vecKeyFrame[m_iCurrKeyFrame + 1].Time)
			m_iCurrKeyFrame++; //한 프레임 넘겨줌

		_double Ratio = (TrackPosition - m_vecKeyFrame[m_iCurrKeyFrame].Time / 
			(m_vecKeyFrame[m_iCurrKeyFrame + 1].Time - m_vecKeyFrame[m_iCurrKeyFrame].Time));
	
		_vector	vSourScale, vDestScale;
		_vector	vSourRotation, vDestRotation;
		_vector	vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_vecKeyFrame[m_iCurrKeyFrame].vScale);
		vSourRotation = XMLoadFloat4(&m_vecKeyFrame[m_iCurrKeyFrame].vRotation);
		vSourPosition = XMLoadFloat3(&m_vecKeyFrame[m_iCurrKeyFrame].vPosition);

		vDestScale = XMLoadFloat3(&m_vecKeyFrame[m_iCurrKeyFrame + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_vecKeyFrame[m_iCurrKeyFrame + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_vecKeyFrame[m_iCurrKeyFrame + 1].vPosition);
	
		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_vecKeyFrame.clear();
}

