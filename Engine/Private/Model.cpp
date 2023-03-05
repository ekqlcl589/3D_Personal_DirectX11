#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	:CComponent(rhs)
	, m_pAiScene(rhs.m_pAiScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMaterial(rhs.m_vecMaterial)
	, m_iNumMaterial(rhs.m_iNumMaterial)
	, m_LocalMatrix(rhs.m_LocalMatrix)
	, m_eType(rhs.m_eType)
	, m_iNumAnimations(rhs.m_iNumAnimations)

{
	for (auto& pPrototypeMesh : rhs.m_vecMesh)
		m_vecMesh.push_back((CMesh*)(pPrototypeMesh->Clone()));

	for (auto& pPrototypeAnim : rhs.m_vecAnimations)
		m_vecAnimations.push_back(pPrototypeAnim->Clone());

	for (auto& pMaterial : m_vecMaterial)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(pMaterial.pMaterialTexture[i]);
		}
	}
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)
	{
		return !strcmp(pBone->Get_Name(), pBoneName);
	});
	return *iter;
}

_uint CModel::Get_BoneIndex(const char * pBoneName)
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		if (0 == strcmp(m_vecBones[i]->Get_Name(), pBoneName))
			return i;
	}
	return 0;
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath, MODEL_TYPE eType, _fmatrix LocalMatrix)
{
	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };// | aiProcess_GlobalScale}; //이거 플래그값 1/100 아닌듯?


	if (MODEL_NONANIM == eType)	
		iFlag |= aiProcess_PreTransformVertices;	

	m_pAiScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAiScene)
		return E_FAIL;
	
	m_eType = eType;

	XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix);

	if (FAILED(Ready_Bones(m_pAiScene->mRootNode, nullptr))) //RootNode는 제일 처음 불리는 모델이라서 부모가 없음 
		return E_FAIL;

	m_iNumBones = m_vecBones.size();

	if (FAILED(Ready_Meshes(LocalMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	// 복제본 마다 새로운 뼈를 만듦
	if (FAILED(Ready_Bones(m_pAiScene->mRootNode, nullptr))) // 깊은 복사를 위해 복사 생성 초기화 부분도 불러줌 
		return E_FAIL;

	m_iNumBones = m_vecBones.size();

	for (_uint i = 0; i < m_iNumMeshes; i++)
		m_vecMesh[i]->SetUp_MeshBones(this);

	return S_OK;
}

HRESULT CModel::SetUp_ShaderMaterialResource(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMaterial[m_vecMesh[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->SetUp_ShaderResource(pShaderCom, pConstantName);
}

HRESULT CModel::SetUp_BoneMatrices(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4 MeshBoneMatrices[256];

	m_vecMesh[iMeshIndex]->Get_BoneMatrices(MeshBoneMatrices, XMLoadFloat4x4(&m_LocalMatrix));

	if (FAILED(pShaderCom->Set_MatrixArray(pConstantName, MeshBoneMatrices, 256)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::SetUp_Animation(_uint iAnimationIndex)
{
	if (iAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	if (m_iCurrAnimation == iAnimationIndex)
	{
		m_vecAnimations[m_iCurrAnimation]->Set_CurrKeyFrame(); // finished , 프레임 초기화
		m_iCurrAnimation = iAnimationIndex;
		m_Check = false;
	}
	else
	{
		m_vecAnimations[m_iCurrAnimation]->Set_TimeAcc(); // 틱, 프레임 초기화
		m_iNextAnimation = iAnimationIndex;
	}
	return S_OK;
}

HRESULT CModel::Play_Animation(_double TimeDelta)
{	
	if (m_iCurrAnimation == m_iNextAnimation && m_Check == false)
	{	
		m_vecAnimations[m_iCurrAnimation]->Play_Animation(TimeDelta, m_vecBones);

		if (true == m_vecAnimations[m_iCurrAnimation]->Get_AnimFinished())
		{
			m_vecAnimations[m_iCurrAnimation]->Set_TimeAcc();
			m_vecAnimations[m_iCurrAnimation]->Set_CurrKeyFrame();
		}
	}
	else
	{
		if (m_vecAnimations[m_iCurrAnimation]->Play_Animation_Last(TimeDelta, m_vecBones, m_vecAnimations[m_iNextAnimation], m_Check))
		{
			m_vecAnimations[m_iCurrAnimation]->Reset();
			m_iCurrAnimation = m_iNextAnimation;

		}
	}
	
	for (auto& pBone : m_vecBones)
	{
		pBone->Invalidate_CombinedMatrix();
	}

	return S_OK;
}

HRESULT CModel::SetUp_AnimationTest(_uint iAnimationIndex)
{
	if (iAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	m_iCurrAnimation = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::Play_AnimationTest(_double TimeDelta)
{
	m_vecAnimations[m_iCurrAnimation]->Play_Animation(TimeDelta, m_vecBones);

	for (auto& pBone : m_vecBones)
	{
		pBone->Invalidate_CombinedMatrix();
	}

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
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAiScene->mMeshes[i], this, LocalMatrix);

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

HRESULT CModel::Ready_Bones(aiNode * pAINode, CBone* pParent)
{
	CBone* pBone = CBone::Create(pAINode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; i++)
		Ready_Bones(pAINode->mChildren[i], pBone); // 재귀 호출을 통해 자식의 갯수만큼 루프를 돌면서 자식들을 채워줌

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAiScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAiScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}

_double CModel::Get_AnimDuration()
{
	return m_vecAnimations[m_iCurrAnimation]->Get_Duration();
}

_double CModel::Get_AnimTimeAcc()
{
	return	m_vecAnimations[m_iCurrAnimation]->Get_TimeAcc();
}

_double CModel::Get_AnimTick()
{
	return	m_vecAnimations[m_iCurrAnimation]->Get_TickPerSecond();
}

_bool CModel::Get_AnimCheck()
{
	return m_vecAnimations[m_iCurrAnimation]->Get_AnimCheck();
}

void CModel::Set_AnimTick(_double Tick)
{
	m_vecAnimations[m_iCurrAnimation]->Set_AnimTick(Tick);
}

_bool CModel::Get_AnimFinished()
{
	return m_vecAnimations[m_iCurrAnimation]->Get_AnimFinished();
}

_bool CModel::Get_LerpAnimFinished()
{
	return m_vecAnimations[m_iCurrAnimation]->Get_LerpAnimFinished();
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

	for (auto& pAnim : m_vecAnimations)
		Safe_Release(pAnim);
	m_vecAnimations.clear();

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);
	m_vecBones.clear();

	for (auto& pMesh : m_vecMesh)
		Safe_Release(pMesh);
	m_vecMesh.clear();
}
