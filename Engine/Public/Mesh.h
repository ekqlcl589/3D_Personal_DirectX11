#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final :	public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

	void Get_BoneMatrices(_float4x4* pMeshBoneMatrices, _fmatrix LocalMatrix);

public:
	HRESULT Initialize_Prototype(CModel::MODEL_TYPE eType, const aiMesh* pAiMesh, class CModel* pModel, _fmatrix LocalMatrix);
	HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_MeshBones(class CModel* pModel);

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix LocalMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel);

private:
	char m_szName[MAX_PATH] = "";
	_uint m_iMaterialIndex = { 0 };

	_uint m_iNumBones = { 0 }; //메시에 영향을 주는 뼈의 갯수

	vector<class CBone*> m_vecBones; // 메시에 영향을 주는 뼈들

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eType, const aiMesh* pAiMesh, class CModel* pModel, _fmatrix LocalMatrix);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free()override;
};

END