#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}


HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
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

void CChannel::Invalidate_Transform(_double TrackPosition, _uint* pCurrKeyFrame, const vector<class CBone*>& Bones)
{
	if (0.0 == TrackPosition)
		*pCurrKeyFrame = 0;

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
		while (TrackPosition >= m_vecKeyFrame[*pCurrKeyFrame + 1].Time)
			++*pCurrKeyFrame; //한 프레임 넘겨줌, 혹여나 여기서 프레임을 빠르게 하기 위해 tickpers를 늘리면 증가 폭이 커지면서 2 ~ 3개의 프레임이 넘어가 버리면서 스케일이나 이런게 튀는 현상이 발생할 수 있음 
		// 그래서 조건문이 아니라 while문을 돌면서 루프 안에서 굴리게 끔 만듦

		while (TrackPosition < m_vecKeyFrame[*pCurrKeyFrame].Time)
			--*pCurrKeyFrame;

		_double Ratio = (TrackPosition - m_vecKeyFrame[*pCurrKeyFrame].Time) /
			(m_vecKeyFrame[*pCurrKeyFrame + 1].Time - m_vecKeyFrame[*pCurrKeyFrame].Time);

		_vector	vSourScale, vDestScale;
		_vector	vSourRotation, vDestRotation;
		_vector	vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame].vScale);
		vSourRotation = XMLoadFloat4(&m_vecKeyFrame[*pCurrKeyFrame].vRotation);
		vSourPosition = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame].vPosition);

		vDestScale = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_vecKeyFrame[*pCurrKeyFrame + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Linear_Transform(_double TrackPosition, _uint* pCurrKeyFrame, const vector<class CBone*>& Bones, vector<class CChannel*> vecChannel, _bool& bCheck, _double fAnimTick)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	m_AnimTic = fAnimTick;

	if (m_AnimTic >= 25.0)
		m_Ratio = 8.0;

	_double Ratio = (TrackPosition / m_Ratio);
	for (auto& iter = vecChannel.begin(); iter != vecChannel.end(); iter++)
	{

		if (!strcmp((*iter)->m_szName, m_szName))
		{
			_vector	vSourScale, vDestScale;
			_vector	vSourRotation, vDestRotation;
			_vector	vSourPosition, vDestPosition;

			if (Ratio >= 1.0)
				bCheck = false;

			vSourScale = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame].vScale);
			vSourRotation = XMLoadFloat4(&m_vecKeyFrame[*pCurrKeyFrame].vRotation);
			vSourPosition = XMLoadFloat3(&m_vecKeyFrame[*pCurrKeyFrame].vPosition);

			vDestScale = XMLoadFloat3(&((*iter)->Get_KeyFrame(0).vScale));
			vDestRotation = XMLoadFloat4(&((*iter)->Get_KeyFrame(0).vRotation));
			vDestPosition = XMLoadFloat3(&((*iter)->Get_KeyFrame(0).vPosition));

			vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);


			_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
			return;
		}
	}

	vScale = XMLoadFloat3(&m_vecKeyFrame[0].vScale);
	vRotation = XMLoadFloat4(&m_vecKeyFrame[0].vRotation);
	vPosition = XMLoadFloat3(&m_vecKeyFrame[0].vPosition);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	if (Ratio >= 1.0)
		bCheck = false;
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
	m_vecKeyFrame.clear();
}

