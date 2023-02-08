#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final :
	public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	HRESULT Initialize_Prototype(const aiMesh* pAiMesh);
	HRESULT Initialize(void* pArg);

private:
	_uint m_iNumMeshes = { 0 };
	vector<CMesh*> m_vecMesh;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAiMesh);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END