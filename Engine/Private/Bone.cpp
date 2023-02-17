#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone* pParent)
{
	strcpy_s(m_szName ,pAINode->mName.data); // ���� �̸��� �� ����� ����
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // ���� �̵� ��� ����
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); // �θ� ��� �׵����� 
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
		//�θ� ����� ���� ���� combined ��Ŀ� �ڱ� �ڽ��� ����� ������
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

