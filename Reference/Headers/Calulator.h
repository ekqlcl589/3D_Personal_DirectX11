#pragma once
#include "Component.h"


BEGIN(Engine)

class CTransform;
class CVIBuffer_Terrain;
class CVIBuffer;

class ENGINE_DLL CCalculator : public CComponent
{
public:
	typedef struct tagPicking
	{
		_vector vRayPos;
		_vector vRayDir;
		_float3 fMousePos;
		_float fDist;
		_bool bPicking = false;
	}PICKING;

public:
	CCalculator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CCalculator(const CCalculator& rhs);
	virtual ~CCalculator() = default;

public:
	HRESULT	Initialize_Prototype(void);
	HRESULT Initialize(void* pArg);
	//_float Compute_HeightOnTerrain(_vector* pPos, const _vector* pTerrainVtxPos, const _ulong& dwCntX = 129, const _ulong& dwCntZ = 129, const _ulong& dwItv = 1);
	// 지형 태우는건 일단 보류
	_bool Picking_OnTerrain(HWND hWnd, CVIBuffer* pTerrainVtxCom, CTransform* pTerrainTransform);

public:
	PICKING Get_PickingState() { return m_Picking; }
	void Set_PickingState() { m_Picking.bPicking = false; }

private:
	PICKING m_Picking;
public:
	static CCalculator*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent*	Clone(void* pArg);

private:
	virtual void Free(void) override;
};

END