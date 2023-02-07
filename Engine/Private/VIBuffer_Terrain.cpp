#include "..\Public\VIBuffer_Terrain.h"


CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_dwNumVerticesX(rhs.m_dwNumVerticesX)
	, m_dwNumVerticesZ(rhs.m_dwNumVerticesZ)
	, m_fVexPos(rhs.m_fVexPos)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{ // bitmap의 3개의 계층 모두 파일 입출력을 통해 데이터를 읽어와야 함 
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	_ulong* pPixel = new _ulong[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iStride = sizeof(VTXNORTEX);
	m_dwNumVerticesX = ih.biWidth;
	m_dwNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_dwNumVerticesX * m_dwNumVerticesZ;
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumPrimitive = (m_dwNumVerticesX - 1) * (m_dwNumVerticesZ - 1) * 2;
	m_iNumIndicices = 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEXBUFFER

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
	m_fVexPos = new _float3[m_iNumVertices];
	ZeroMemory(m_fVexPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_dwNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_dwNumVerticesX; j++)
		{
			_uint iIndex = i * m_dwNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 15.0f, i);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_dwNumVerticesX - 1.0f), i / (m_dwNumVerticesZ - 1.0f));
			m_fVexPos[iIndex] = pVertices[iIndex].vPosition;
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_ulong	dwTriIdx = 0;

	for (_uint i = 0; i < m_dwNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_dwNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_dwNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_dwNumVerticesX,
				iIndex + m_dwNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			pIndices[dwTriIdx]._0 = iIndices[0];
			pIndices[dwTriIdx]._1 = iIndices[1];
			pIndices[dwTriIdx]._2 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._0].vPosition);

			vDest = XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vPosition);

			// 두 벡터의 크기를 구하고 길이를 1로 만든 후 외적을 통해 법선벡터 정의 
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));


			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._0].vNormal) + vNormal);

			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vNormal) + vNormal);

			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._2].vNormal) + vNormal);

			dwTriIdx++;

			pIndices[dwTriIdx]._0 = iIndices[0];
			pIndices[dwTriIdx]._1 = iIndices[2];
			pIndices[dwTriIdx]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._0].vPosition);

			vDest = XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vPosition);

			// 두 벡터의 크기를 구하고 길이를 1로 만든 후 외적을 통해 법선벡터 정의 
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));


			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._0].vNormal) + vNormal);

			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._1].vNormal) + vNormal);

			XMStoreFloat3(&pVertices[pIndices[dwTriIdx]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwTriIdx]._2].vNormal) + vNormal);

			dwTriIdx++;

		}
	}

#pragma endregion INDEXBUFFER
	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

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
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	return S_OK;
}


CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain * pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Create Fail : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain * pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Fail : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if(!m_isCloned)
		Safe_Delete_Array(m_fVexPos);
}
