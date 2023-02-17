#include "..\Public\Mesh.h"
#include "Bone.h"


CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	:CVIBuffer(rhs)
{
}

void CMesh::Get_BoneMatrices(_float4x4 * pMeshBoneMatrices, _fmatrix LocalMatrix)
{
	_uint iIndex = 0;

	for (auto& pBone : m_vecBones)
	{
		XMStoreFloat4x4(&pMeshBoneMatrices[iIndex++], XMLoadFloat4x4(&pBone->Get_OffsetMatrix()) * XMLoadFloat4x4(&pBone->Get_CombinedTransformMatrix()) * LocalMatrix);
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::MODEL_TYPE eType, const aiMesh * pAiMesh, class CModel* pModel, _fmatrix LocalMatrix)
{
	strcpy_s(m_szName, pAiMesh->mName.data);

	m_iMaterialIndex = pAiMesh->mMaterialIndex;
	m_iNumVertices = pAiMesh->mNumVertices;
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumPrimitive = pAiMesh->mNumFaces;
	m_iNumIndicices = 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = CModel::MODEL_NONANIM == eType ? 
		Ready_VertexBuffer_For_NonAnim(pAiMesh, LocalMatrix) : Ready_VertexBuffer_For_Anim(pAiMesh, pModel);
	
	if (FAILED(hr))
		return E_FAIL;

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

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAiMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAiMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAiMesh->mFaces[i].mIndices[2];
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	
	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh * pAIMesh, _fmatrix LocalMatrix)
{
	m_iStride = sizeof(VTXNONANIMMODEL);

#pragma region VERTEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXNONANIMMODEL* pVertices = new VTXNONANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNONANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), LocalMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), LocalMatrix));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion VERTEXBUFFER

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh * pAIMesh, class CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);

#pragma region VERTEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXANIMMODEL* pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));

		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

	}

	m_iNumBones = pAIMesh->mNumBones; // 4개 까지 받을 수 있음 

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		// 메시에 영향을 주는 뼈들 중 i번 째 뼈
		aiBone* pAIBone = pAIMesh->mBones[i];

		CBone* pBone = pModel->Get_BonePtr(pAIBone->mName.data);

		if (nullptr == pBone)
			return E_FAIL;

		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		pBone->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_vecBones.push_back(pBone);
		Safe_AddRef(pBone);

		/* 이 뼈가 영향을 주는 정점들의 갯수. */
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			/* 이 i번째 뼈가 영향을 주는 j번째 정점의 인덱스 */
			if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	// 내 게임에서 이게 꼭 필요한지는 모르겠음
	// 외부 모델을 플레이어 뼈에 붙이는 행위를 할 떄 참고 하면 됨
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		CBone* pBone = { nullptr };

		pBone = pModel->Get_BonePtr(m_szName);

		if (nullptr == pBone)
			return E_FAIL;

		m_vecBones.push_back(pBone);
	}
#pragma endregion VERTEXBUFFER

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::MODEL_TYPE eType, const aiMesh* pAiMesh, class CModel* pModel, _fmatrix LocalMatrix)
{
	CMesh * pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAiMesh, pModel, LocalMatrix)))
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

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);

	m_vecBones.clear();
}
