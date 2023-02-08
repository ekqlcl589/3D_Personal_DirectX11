#include "..\Public\Mesh.h"



CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(const aiMesh * pAiMesh)
{
	m_iStride = sizeof(VTXNONANIMMODEL);
	m_iNumVertices = pAiMesh->mNumVertices;
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumPrimitive = pAiMesh->mNumFaces;
	m_iNumIndicices = 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT ;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXNONANIMMODEL* pVertices = new VTXNONANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNONANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAiMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAiMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAiMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAiMesh->mTangents[i], sizeof(_float3));

	}

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

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; i++)
	{
		pIndices[i]._0 = pAiMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAiMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAiMesh->mFaces[i].mIndices[2];
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh* pAiMesh)
{
	CMesh * pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pAiMesh)))
	{
		MSG_BOX("CMesh Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh * pInstance = new CMesh(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMesh Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
}
