#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CPlayerComboReady final :
	public CGameObject
{
public:
	enum TYPE { TYPE_1, TYPE_2, TYPE_3, TYPE_4, TYPE_END};

	typedef struct tagReadyEffect
	{
		_vector vPosition;
		TYPE eType;
	}READYEFFECT;

private:
	CPlayerComboReady(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerComboReady(const CPlayerComboReady& rhs);
	virtual ~CPlayerComboReady() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_vector m_vPosition;

	_bool m_bActive = false;

	READYEFFECT m_ReadyDesc;

public:
	static CPlayerComboReady* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END