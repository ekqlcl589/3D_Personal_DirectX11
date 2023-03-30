#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRuins : public CGameObject
{
public:
	typedef struct tagTileState
	{
		_float3 fPos;
		_int iTileNum;
		_float3 fScale;

		CTransform::TRANSFORM_DESC transformDesc;
	}TILESTATE;

public:
	CRuins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRuins(const CRuins& rhs);
	virtual ~CRuins() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	TILESTATE m_TileState;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CRuins* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END