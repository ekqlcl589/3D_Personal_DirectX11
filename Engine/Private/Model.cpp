#include "..\Public\Model.h"
#include "Mesh.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	:CComponent(rhs)
{
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath, MODEL_TYPE eType)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (MODEL_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAiScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAiScene)
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& pModel : m_vecMesh)
	{
		if (nullptr != pModel)
			pModel->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	if (nullptr == m_pAiScene)
		return E_FAIL;

	m_iNumMeshes = m_pAiScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAiScene->mMeshes[i]);

		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, MODEL_TYPE eType)
{
	CModel * pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType)))
	{
		MSG_BOX("Model Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel * pInstance = new CModel(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Model Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();
}
