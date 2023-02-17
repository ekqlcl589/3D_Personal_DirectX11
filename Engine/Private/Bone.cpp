#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone* pParent)
{
	strcpy_s(m_szName ,pAINode->mName.data); // 뼈의 이름을 내 멤버로 복사
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // 뼈의 이동 행렬 복사
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); // 부모 행렬 항등으로 
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Invalidate_CombinedMatrix()
{
	if (nullptr == m_pParent)
	{
		m_CombinedTransformationMatrix = m_TransformationMatrix; 
		return;
		//부모 행렬이 없는 뼈는 combined 행렬에 자기 자신의 행렬을 대입함
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
}

CBone * CBone::Create(aiNode * pAINode, CBone* pParent)
{
	CBone * pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("CBone Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}

