#include "..\Public\Calulator.h"
#include "GameInstance.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"

CCalculator::CCalculator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CCalculator::CCalculator(const CCalculator & rhs)
	:CComponent(rhs)
{
}

HRESULT CCalculator::Initialize_Prototype(void)
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CCalculator::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

_bool CCalculator::Picking_OnTerrain(HWND hWnd, CVIBuffer * pTerrainVtxCom,  CTransform * pTerrainTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);


	D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof D3D11_VIEWPORT);
	UINT INumViewPort = 0;
	m_pContext->RSGetViewports(&INumViewPort, NULL);
	D3D11_VIEWPORT* pViewPort = new D3D11_VIEWPORT[INumViewPort];
	m_pContext->RSGetViewports(&INumViewPort, pViewPort);

	float VP_W = (float)pViewPort[0].Width;
	float VP_H = (float)pViewPort[0].Height;

	Safe_Delete_Array(pViewPort);

	// ºäÆ÷Æ® -> Åõ¿µ

	_float3 fMouse = XMFLOAT3(((float)ptMouse.x / (VP_W * 0.5f) - 1.f), ((float)ptMouse.y / (-VP_H * 0.5f) + 1.f), 0.f);
	
	 _vector vMousePos = XMVectorSet(((float)ptMouse.x / (VP_W * 0.5f) - 1.f),
		 ((float)ptMouse.y / (-VP_H * 0.5f) + 1.f), 
		 0.f, 1.f);

	// Åõ¿µ -> ºä
	 _matrix matProj = pInstance->Get_TransformMatrixInverse(CPipeLine::TS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	_vector vRayDir, vRayPos;

	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPos;

	// ºä -> ¿ùµå
	_matrix matView = pInstance->Get_TransformMatrixInverse(CPipeLine::TS_VIEW);
	vRayPos = XMVector3TransformCoord(vRayPos, matView);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(vRayDir, matView));

	// ¿ùµå -> ·ÎÄÃ
	_matrix matWorld;
	matWorld = pTerrainTransform->Get_WorldMatrixInverse();
	vRayPos = XMVector3TransformCoord(vRayPos, matWorld);
	vRayDir = XMVector3TransformNormal(vRayDir, matWorld);

	RELEASE_INSTANCE(CGameInstance);

	_ulong dwNumVerticesZ = dynamic_cast<CVIBuffer_Terrain*>(pTerrainVtxCom)->Get_Vtx(2);
	_ulong dwNumVerticesX = dynamic_cast<CVIBuffer_Terrain*>(pTerrainVtxCom)->Get_Vtx(0);
	_float Min_Distance = 9999.f;
	_float fDistance;
	_vector pIndices[3]{};
	for (_uint i = 0; i < dwNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < dwNumVerticesX - 1; j++)
		{
			_uint iIndex = i * dwNumVerticesX + j;

			_vector		iIndices[] = {
				iIndex + dwNumVerticesX,
				iIndex + dwNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};
			pIndices[0] = iIndices[0];
			pIndices[1] = iIndices[1];
			pIndices[2] = iIndices[2];
			
			_float3* fPos = dynamic_cast<CVIBuffer_Terrain*>(pTerrainVtxCom)->Get_VtxPos();

			if (TriangleTests::Intersects(vRayPos, XMVector3Normalize(vRayDir), XMLoadFloat3(&fPos[iIndex + dwNumVerticesX]), XMLoadFloat3(&fPos[iIndex + dwNumVerticesX + 1]), XMLoadFloat3(&fPos[iIndex + 1]), fDistance))
			{
				m_Picking.vRayDir = vRayDir;
				m_Picking.fMousePos = fMouse;
				m_Picking.fDist = fDistance;
				m_Picking.bPicking = true;
				_vector j = vRayPos + XMVector3Normalize(vRayDir) * fDistance;
				m_Picking.vRayPos = j;

				//RELEASE_INSTANCE(CGameInstance);

				return m_Picking.bPicking;
			}
			
			pIndices[0] = iIndices[0];
			pIndices[1] = iIndices[2];
			pIndices[2] = iIndices[3];

			if (TriangleTests::Intersects(vRayPos, XMVector3Normalize(vRayDir), XMLoadFloat3(&fPos[iIndex + dwNumVerticesX]), XMLoadFloat3(&fPos[iIndex + 1]), XMLoadFloat3(&fPos[iIndex]), fDistance))
			{
				m_Picking.vRayDir = vRayDir;
				m_Picking.fMousePos = fMouse;
				m_Picking.fDist = fDistance;
				m_Picking.bPicking = true;
				_vector j = vRayPos + XMVector3Normalize(vRayDir) * fDistance;
				m_Picking.vRayPos = j;

				//RELEASE_INSTANCE(CGameInstance);

				return m_Picking.bPicking;
			}

		}
	}

	//RELEASE_INSTANCE(CGameInstance);

	return m_Picking.bPicking;
}

CCalculator * CCalculator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCalculator * pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Fail : CCalculator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCalculator::Clone(void * pArg)
{
	CCalculator * pInstance = new CCalculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Fail : CCalculator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCalculator::Free(void)
{
	__super::Free();

}
