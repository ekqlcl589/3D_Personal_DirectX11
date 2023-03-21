#pragma once

#include "Base.h"

BEGIN(Engine)

/* 가장 큰 지형을 표현하는 하나의 노드.   */
class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CFrustum* pFrustum, const _float3* pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces);
	void SetUp_Neighbors();

private:
	_uint			m_iCorners[CORNER_END] = { 0 };
	_uint			m_iCenter = { 0 };
	CQuadTree*		m_pChilds[CHILD_END] = { nullptr };
	CQuadTree*		m_pNeighbor[NEIGHBOR_END] = { nullptr };

private:
	_bool isDraw(const _float3* pVerticesPos);


public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END