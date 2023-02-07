#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Texture(rhs.m_Texture)
{
	for (auto& pSRV : m_Texture)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* PTextureFilePath, _uint iNumTexture)
{
	m_Texture.reserve(iNumTexture);

	_tchar szFullPath[MAX_PATH] = TEXT(""); // �Լ� ȣ�� �� ���ڷ� �޴� ��θ� �޾���

	for (_uint i = 0; i < iNumTexture; i++)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;

		wsprintf(szFullPath, PTextureFilePath, i);

		_tchar szExt[MAX_PATH] = TEXT(""); // Ȯ���ڸ� �޾Ƽ� �ҷ��� ���� Ÿ�� ���� 

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT hr = 0;

		if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_Texture.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::SetUp_ShaderResource(CShader * pShader, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShader || iTextureIndex >= m_Texture.size())
		return E_FAIL;

	return pShader->Set_ShaderResourceView(pConstantName, m_Texture[iTextureIndex]);
}

HRESULT CTexture::SetUp_ShaderResourceArray(CShader * pShader, const char * pConstantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRVs[8] = { nullptr }; // ���̴��� ���� �� �ִ� �ִ� �ؽ���: 8

	_uint iTextureIndex = 0; // pSRVs���ڷ� �������� ��� 

	for (auto& pSRV : m_Texture)
		pSRVs[iTextureIndex++] = pSRV;

	return pShader->Set_ShaderResourceViewArray(pConstantName, pSRVs, m_Texture.size());
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* PTextureFilePath, _uint iNumTexture)
{
	CTexture * pInstance = new CTexture(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(PTextureFilePath, iNumTexture)))
	{
		MSG_BOX("CTexture Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture * pInstance = new CTexture(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTexture Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Texture)
		Safe_Release(pSRV);

	m_Texture.clear();
}
