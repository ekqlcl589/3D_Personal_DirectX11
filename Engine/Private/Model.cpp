#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Texture.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	:CComponent(rhs)
	, m_pAiScene(rhs.m_pAiScene)
	, m_vecMesh(rhs.m_vecMesh)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMaterial(rhs.m_vecMaterial)
	, m_iNumMaterial(rhs.m_iNumMaterial)
	, m_LocalMatrix(rhs.m_LocalMatrix)
{
	for (auto& pMesh : m_vecMesh)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(pMaterial.pMaterialTexture[i]);
		}
	}
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath, MODEL_TYPE eType, _fmatrix LocalMatrix)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };// | aiProcess_GlobalScale}; //이거 플래그값 1/100 아닌듯?


	if (MODEL_NONANIM == eType)	
		iFlag |= aiProcess_PreTransformVertices;	

	m_pAiScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAiScene)
		return E_FAIL;
	
	XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix);

	if (FAILED(Ready_Bones(m_pAiScene->mRootNode)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(LocalMatrix)))
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

HRESULT CModel::Ready_Meshes(_fmatrix LocalMatrix)
{
	if (nullptr == m_pAiScene)
		return E_FAIL;

	m_iNumMeshes = m_pAiScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, MODEL_NONANIM, m_pAiScene->mMeshes[i], LocalMatrix);

		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	m_iNumMaterial = m_pAiScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterial; ++i)
	{
		MODEL_MATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODEL_MATERIAL));

		aiMaterial* pMaterial = m_pAiScene->mMaterials[i];
		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
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

HRESULT CModel::Ready_Bones(aiNode * pAINode)
{
	CBone* pBone = CBone::Create(pAINode);
	if (nullptr == pBone)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
		Ready_Bones(pAINode->mChildren[i]); // 재귀 호출을 통해 자식의 갯수만큼 루프를 돌면서 자식들을 채워줌

	return S_OK;
}

HRESULT CModel::SetUp_ShaderMaterialResource(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMaterial[m_vecMesh[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->SetUp_ShaderResource(pShaderCom, pConstantName);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, MODEL_TYPE eType, _fmatrix LocalMatrix)
{
	CModel * pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, LocalMatrix)))
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

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);

}
