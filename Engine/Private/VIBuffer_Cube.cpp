#include "VIBuffer_Cube.h"


CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}


Engine::CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
	: CVIBuffer(rhs)
{

}


HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumPrimitive = 12;
	m_iNumIndicices = 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXCUBE*		pVertices = new VTXCUBE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexUV = pVertices[0].vPosition;

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexUV = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexUV = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexUV = pVertices[3].vPosition;


	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexUV = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexUV = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexUV = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexUV = pVertices[7].vPosition;
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	/* +X */
	pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
	pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;


	/* -X */
	pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
	pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;

	/* +Y */
	pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
	pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;

	/* -Y */
	pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
	pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;


	/* +Z */
	pIndices[8]._0 = 5; pIndices[8]._1 = 4; pIndices[8]._2 = 7;
	pIndices[9]._0 = 5; pIndices[9]._1 = 7; pIndices[9]._2 = 6;

	/* -Z */
	pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
	pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		return pInstance;

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("VIBuffer_Cube Clone Fail");
		Safe_Release(pInstance);
		
	}

	return pInstance;
}

void CVIBuffer_Cube::Free(void)
{
	__super::Free();
}

