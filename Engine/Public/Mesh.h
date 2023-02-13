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

public:
	HRESULT Initialize_Prototype(CModel::MODEL_TYPE eType, const aiMesh* pAiMesh, _fmatrix LocalMatrix);
	HRESULT Initialize(void* pArg);

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix LocalMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh);

private:
	_uint m_iMaterialIndex = { 0 };

	_uint m_iNumBones = { 0 }; //�޽ÿ� ������ �ִ� ���� ����

	vector<class CBone*> m_vecBones; // �޽ÿ� ������ �ִ� ����

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eType, const aiMesh* pAiMesh, _fmatrix LocalMatrix);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free()override;
};

END