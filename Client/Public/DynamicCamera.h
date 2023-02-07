#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CGameInstance;

END


BEGIN(Client)

class CDynamicCamera final : public CCamera
{
private:
	CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual void Tick(_double TimeDelta)override;
	virtual void LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	void Key_Input(_double TimeDelta);

private:
	void Mouse_Check(_double TimeDelta);
	void Mouse_Fix();

private:
	CGameInstance* m_pInstance = nullptr;
	_bool m_bCheck = false;
	_bool m_bFix = false;

public:
	static CDynamicCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free()override;
};

END