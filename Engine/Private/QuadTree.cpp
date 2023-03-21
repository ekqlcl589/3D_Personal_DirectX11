#include "..\Public\QuadTree.h"
#include "Frustum.h"
#include "GameInstance.h"



CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenter = (iLT + iRB) >> 1;	

	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_pChilds[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenter, iLC);
	m_pChilds[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenter);
	m_pChilds[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, iRB, iBC);
	m_pChilds[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(CFrustum* pFrustum, const _float3* pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces)
{
	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT] || 
		true == isDraw(pVerticesPos))
	{
		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
			isDraw[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->isDraw(pVerticesPos);

		if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
			isDraw[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->isDraw(pVerticesPos);

		if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
			isDraw[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->isDraw(pVerticesPos);

		if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
			isDraw[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->isDraw(pVerticesPos);

		_bool		isIn[] = {
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RB]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LB]]), 0.0f),
		};

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			/*오른쪽 위 삼각혀 ㅇ그려야되냐? */
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/*오른쪽 위 삼각혀 ㅇ그려야되냐? */
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}
			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iLC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iLC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}


			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iBC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iBC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RT];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}


			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}
		}
		return;
	}


	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])));

	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFaces);
		}
	}


}

_bool CQuadTree::isDraw(const _float3* pVerticesPos)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_float4	vCamPosition = pGameInstance->Get_CamPos();

	RELEASE_INSTANCE(CGameInstance);

	_float	fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat4(&vCamPosition)));

	_float	fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])));
	
	if (fDistance * 0.3f > fWidth)
		return true;
	return false;	
}

void CQuadTree::SetUp_Neighbors()
{
	if (nullptr == m_pChilds[CHILD_LT]->m_pChilds[CHILD_LT])
		return;

	m_pChilds[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RT];
	m_pChilds[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_LB];

	m_pChilds[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChilds[CHILD_LT];
	m_pChilds[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_RB];

	m_pChilds[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChilds[CHILD_LB];
	m_pChilds[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChilds[CHILD_RT];

	m_pChilds[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RB];
	m_pChilds[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChilds[CHILD_LT];

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChilds[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChilds[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_RT];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChilds[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChilds[CHILD_RT];
		m_pChilds[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChilds[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChilds[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChilds[CHILD_LB];
		m_pChilds[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChilds[CHILD_RB];
	}

	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChilds[i])
			m_pChilds[i]->SetUp_Neighbors();
	}
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Cloned : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (_uint i = 0; i < CHILD_END; ++i)
		Safe_Release(m_pChilds[i]);	
}
