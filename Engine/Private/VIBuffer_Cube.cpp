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

	pVertices[0].vPosition = _float3(-1.f, 1.f, -1.f);
	pVertices[0].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[0].vTexUV = XMFLOAT2(pVertices[0].vPosition.x, pVertices[0].vPosition.y);

	pVertices[1].vPosition = _float3(1.f, 1.f,-1.f );
	pVertices[1].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[1].vTexUV = XMFLOAT2(pVertices[1].vPosition.x, pVertices[1].vPosition.y);

	pVertices[2].vPosition = _float3( 1.f, -1.f, -1.f );
	pVertices[2].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[2].vTexUV = XMFLOAT2(pVertices[2].vPosition.x, pVertices[2].vPosition.y);

	pVertices[3].vPosition = _float3( -1.f, -1.f, -1.f );
	pVertices[3].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[3].vTexUV = XMFLOAT2(pVertices[3].vPosition.x, pVertices[3].vPosition.y);

	pVertices[4].vPosition = _float3( -1.f, 1.f, 1.f );
	pVertices[4].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[4].vTexUV = XMFLOAT2(pVertices[4].vPosition.x, pVertices[4].vPosition.y);

	pVertices[5].vPosition = _float3( 1.f, 1.f, 1.f );
	pVertices[5].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[5].vTexUV = XMFLOAT2(pVertices[5].vPosition.x, pVertices[5].vPosition.y);

	pVertices[6].vPosition = _float3( 1.f, -1.f, 1.f );
	pVertices[6].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[6].vTexUV = XMFLOAT2(pVertices[6].vPosition.x, pVertices[6].vPosition.y);

	pVertices[7].vPosition = _float3( -1.f, -1.f, 1.f);
	pVertices[7].vNormal = _float3(0.f, 0.f, 0.f);
	pVertices[7].vTexUV = XMFLOAT2(pVertices[7].vPosition.x, pVertices[7].vPosition.y);

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

	// x+
	pIndices[0]._0 = 1;
	pIndices[0]._1 = 5;
	pIndices[0]._2 = 6;

	pIndices[1]._0 = 1;
	pIndices[1]._1 = 6;
	pIndices[1]._2 = 2;

	// x-
	pIndices[2]._0 = 4;
	pIndices[2]._1 = 0;
	pIndices[2]._2 = 3;

	pIndices[3]._0 = 4;
	pIndices[3]._1 = 3;
	pIndices[3]._2 = 7;

	// y+
	pIndices[4]._0 = 4;
	pIndices[4]._1 = 5;
	pIndices[4]._2 = 1;

	pIndices[5]._0 = 4;
	pIndices[5]._1 = 1;
	pIndices[5]._2 = 0;

	// y-
	pIndices[6]._0 = 3;
	pIndices[6]._1 = 2;
	pIndices[6]._2 = 6;

	pIndices[7]._0 = 3;
	pIndices[7]._1 = 6;
	pIndices[7]._2 = 7;

	// z+
	pIndices[8]._0 = 7;
	pIndices[8]._1 = 6;
	pIndices[8]._2 = 5;

	pIndices[9]._0 = 7;
	pIndices[9]._1 = 5;
	pIndices[9]._2 = 4;

	// z-
	pIndices[10]._0 = 0;
	pIndices[10]._1 = 1;
	pIndices[10]._2 = 2;

	pIndices[11]._0 = 0;
	pIndices[11]._1 = 2;
	pIndices[11]._2 = 3;

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

