#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode)
{
	strcpy_s(m_szName ,pAINode->mName.data); // 뼈의 이름을 내 멤버로 복사
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // 뼈의 이동 행렬 복사
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); // 부모 행렬 항등으로 

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

