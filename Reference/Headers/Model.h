#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final :
	public CComponent
{
public:
	enum MODEL_TYPE {MODEL_NONANIM, MODEL_ANIM, MODEL_END};
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

	class CBone* Get_BonePtr(const char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, MODEL_TYPE eType, _fmatrix LocalMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_ShaderMaterialResource(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT SetUp_BoneMatrices(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex);
	HRESULT SetUp_Animation(_uint iAnimationIndex);
	HRESULT Play_Animation(_double TimeDelta);
	HRESULT Render(_uint iMeshIndex);

public:
	_uint Get_Animations() { return m_iNumAnimations; }

private:
	HRESULT Ready_Meshes(_fmatrix LocalMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, class CBone* pParent);
	HRESULT Ready_Animation();

public:
	vector<class CAnimation*>* Get_Anim() { return &m_vecAnimations; }
	_double Get_AnimDuration();
	_double Get_AnimTimeAcc();

private:
	Assimp::Importer m_Importer;
	const aiScene* m_pAiScene = { nullptr };

private:
	MODEL_TYPE m_eType = { MODEL_END };

private:
	_uint m_iNumMeshes = { 0 };
	vector<class CMesh*> m_vecMesh;

private:
	_uint m_iNumBones = { 0 };
	vector<class CBone*> m_vecBones; // 모델에 정의 되어 있는 전체 뼈 

private:
	_uint m_iNumMaterial = { 0 };
	vector<MODEL_MATERIAL> m_vecMaterial;

private:
	_uint m_iNumAnimations = { 0 };
	_uint m_iCurrAnimation = { 0 };
	vector<class CAnimation*> m_vecAnimations;
private:
	_float4x4 m_LocalMatrix;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, MODEL_TYPE eType, _fmatrix LocalMatrix);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END