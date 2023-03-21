#include "..\Public\VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
	m_iStride = sizeof(VTXPOS);
	m_iNumVertices = 3;
	m_iIndicesSize = sizeof(_ushort);
	m_iNumPrimitive = 4;
	m_iNumIndicices = 1;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXPOS*		pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	memcpy(pVertices, pPoints, sizeof(_float3) * m_iNumVertices);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*���� �׸������ϴ� �ε����ϳ��� ũ�� * �ε����� ����. 
	== �ﰢ���� �׸������� �ε��� ������ ũ�� * �ﰢ���ǰ��� */;
	m_BufferDesc.ByteWidth = sizeof(_ushort) * 4;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort*		pIndices = new _ushort[4];
	ZeroMemory(pIndices, sizeof(_ushort) * 4);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;
	
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void * pArg)
{
	return S_OK;
}


CVIBuffer_Cell * CVIBuffer_Cell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints)
{
	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Cell::Clone(void * pArg)
{
	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();

}
