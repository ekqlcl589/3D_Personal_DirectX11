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

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, MODEL_TYPE eType);
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render(_uint iMeshIndex);
	HRESULT SetUp_ShaderMaterialResource(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const char* pModelFilePath);


private:
	Assimp::Importer m_Importer;
	const aiScene* m_pAiScene = { nullptr };

private:
	_uint m_iNumMeshes = { 0 };
	vector<class CMesh*> m_vecMesh;

private:
	_uint m_iNumMaterial = { 0 };
	vector<MODEL_MATERIAL> m_vecMaterial;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, MODEL_TYPE eType);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END