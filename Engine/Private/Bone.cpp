#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode)
{
	strcpy_s(m_szName ,pAINode->mName.data); // ���� �̸��� �� ����� ����
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // ���� �̵� ��� ����
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); // �θ� ��� �׵����� 

	return S_OK;
}

CBone * CBone::Create(aiNode * pAINode)
{
	CBone * pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode)))
	{
		MSG_BOX("CBone Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
}

