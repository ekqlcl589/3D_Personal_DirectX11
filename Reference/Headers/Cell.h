#pragma once

#include "Base.h"

/* 네비게이션응ㄹ 구성하는 하나의 삼각형. */
BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

public:
	virtual	HRESULT Initialize(const _float3* pPoints, _int iIndex);
public:
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	void SetUp_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighbors[eLine] = pNeighbor->m_iIndex;
	}

	_bool isIn(_fvector vPosition, _int* pNeighborIndex);


#ifdef _DEBUG
public:
	//void Render(class CShader* pShader, _float4 vColor);
#endif // _DEBUG



private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float3			m_vPoints[POINT_END];
	_float3			m_vNormals[LINE_END];
	_int			m_iIndex = { 0 };
	_int			m_iNeighbors[3] = { -1, -1, -1 };

#ifdef _DEBUG
private:
//	class CVIBuffer_Cell*			m_pVIBuffer = nullptr;
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END