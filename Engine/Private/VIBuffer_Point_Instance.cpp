#include "..\Public\VIBuffer_Point_Instance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_pSpeed(rhs.m_pSpeed)
	, m_pOldHeight(rhs.m_pOldHeight)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(_float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iNumInstance)
{
	m_pSpeed = new _float[iNumInstance];

	for (_uint i = 0; i < iNumInstance; ++i)
		m_pSpeed[i] = rand() % ((_int)(fMaxSpeed - fMinSpeed) + 1) + fMinSpeed;

	m_iNumInstance = iNumInstance;
	m_iStrideInstance = sizeof(VTXMATRIX);
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iIndicesSize = sizeof(_ushort);
	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicices = 1;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	/*D3D11_USAGE_DYNAMIC : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 한다. */
	/*D3D11_USAGE_DEAFULT : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 하지 않는다. */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXPOINT*		pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.0f, 1.f);	

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*내가 그릴려고하는 인덱스하나의 크기 * 인덱스의 갯수. 
	== 삼각형을 그리기위한 인덱스 세개의 크기 * 삼각형의갯수 */;
	m_BufferDesc.ByteWidth = m_iNumIndicices * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);	

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCEBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;

	/*D3D11_USAGE_DYNAMIC : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 한다. */
	/*D3D11_USAGE_DEAFULT : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 하지 않는다. */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMATRIX*		pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pOldHeight = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		m_pOldHeight[i] = rand() % (_int(fHeight) + 1) - (_int(fHeight) >> 1);
		pInstanceVertices[i].vPosition = _float4(rand() % (_int(fWidth) + 1) - (_int(fWidth) >> 1), 
			m_pOldHeight[i],
			rand() % (_int(fDepth) + 1) - (_int(fDepth) >> 1), 
			1.f);
	}	

	m_SubResourceData.pSysMem = pInstanceVertices;

	if(FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iStrideInstance

	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iNumIndicices,
		m_iNumInstance,
		0, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Update(/*_fmatrix WorldMatrix, */_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{		
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= m_pSpeed[i] * TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < -30.f)
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_pOldHeight[i];
	}
	



	m_pContext->Unmap(m_pVBInstance, 0);
}


CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iNumInstance)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fWidth, fHeight, fDepth, fMinSpeed, fMaxSpeed, iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pOldHeight);
	 }

	Safe_Release(m_pVBInstance);

}
