#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final :	public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_ulong Get_Vtx(int i) { if (i == 0) return m_dwNumVerticesX; else if (i == 2) return m_dwNumVerticesZ; else return 0; }

	_float3* Get_VtxPos() { return m_fVexPos; }
private:
	_ulong m_dwNumVerticesX = { 0 };
	_ulong m_dwNumVerticesZ = { 0 };
	_float3* m_fVexPos;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END