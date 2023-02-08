#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Terrain;
class CTexture;
class CTransform;
class CCalculator;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURE { TEX_DIFFUSE, TEX_MULTI_DIFFUSE, TEX_BRUSH, TEX_FILTER, TEX_END };

protected:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Check_Picking();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBuffer_Terrain = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pMultiTextureCom[TEX_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CCalculator* m_pCaCom = { nullptr };

private:
	ID3D11ShaderResourceView* m_pFilterSRV = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResource();
	HRESULT Create_FilterTexture(); // 마스크맵을 코드로 제작하기 위한 함수, 쓸지는 모르겠음 

public:
	/* 원형객체를 생성 */
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성 */
	virtual CGameObject* Clone(void* pArg = nullptr) override;

	virtual void Free() override;
};

END