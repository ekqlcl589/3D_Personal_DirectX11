#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	:CComponent(rhs)
	, m_pAiScene(rhs.m_pAiScene)
	, m_vecMesh(rhs.m_vecMesh)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMaterial(rhs.m_vecMaterial)
	, m_iNumMaterial(rhs.m_iNumMaterial)
{
	for (auto& pMesh : m_vecMesh)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(pMaterial.pMaterialTexture[i]);
	}
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath, MODEL_TYPE eType)
{
	_uint			iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (MODEL_NONANIM == eType)	
		iFlag |= aiProcess_PreTransformVertices;	

	m_pAiScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAiScene)
		return E_FAIL;
	
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_vecMesh[iMeshIndex]->Render();

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

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	m_iNumMaterial = m_pAiScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterial; i++)
	{
		MODEL_MATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODEL_MATERIAL));

		aiMaterial* pMaterial = m_pAiScene->mMaterials[i];
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString strPath;
			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;
			// 여기서 break를 걸면 모델들이 가지고 있는 머테리얼이 다 불리기 전에 없으면 튕겨져 나가서 전부 불러오지 못함

			char szDrive[MAX_PATH] = "";
			char szPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szPath, MAX_PATH, nullptr, 0, nullptr, 0);

			char szFileName[MAX_PATH] = "";
			char szExt[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			_tchar szMaterialTexturePath[MAX_PATH] = TEXT("");
			char szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szPath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szMaterialTexturePath, MAX_PATH);

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szMaterialTexturePath);

			if (nullptr == ModelMaterial.pMaterialTexture[j])
				return E_FAIL;
		}
		m_vecMaterial.push_back(ModelMaterial);
	}
	return S_OK;
}

HRESULT CModel::SetUp_ShaderMaterialResource(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMaterial[m_vecMesh[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->SetUp_ShaderResource(pShaderCom, pConstantName);
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

	for (auto& Material : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_Release(Material.pMaterialTexture[i]);
		}
	}

	m_vecMaterial.clear();

	for (auto& pMesh : m_vecMesh)
		Safe_Release(pMesh);

}
