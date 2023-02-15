#include "..\Public\VIBuffer_Rect.h"


CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iIndicesSize = sizeof(FACEINDICES16);
	m_iNumPrimitive = 2;
	m_iNumIndicices = 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion INDEXBUFFER
	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Rect::Set_Buffer(_float x, _float y)
{
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	pVertices[0].vPosition = { 0.5f, y , 0.f };
	pVertices[0].vTexUV = { 0.f, x };

	pVertices[1].vPosition = { 0.5f, y, 0.f };
	pVertices[1].vTexUV = { 0.5f, x };

	pVertices[2].vPosition = { 0.5f, -0.5f, 0.f };
	pVertices[2].vTexUV = { 0.5f, 0.5f };

	pVertices[3].vPosition = { -0.5f, -0.5f, 0.f };
	pVertices[3].vTexUV = { 0.f, 0.5f };

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return;

	Safe_Delete_Array(pVertices);

}

CVIBuffer_Rect * CVIBuffer_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Rect * pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Fail : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Rect::Clone(void * pArg)
{
	CVIBuffer_Rect * pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Fail : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
