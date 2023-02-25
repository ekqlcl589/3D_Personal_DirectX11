#pragma once

#include "Client_Defines.h"
#include "Camera.h"
BEGIN(Engine)
class CGameInstance;

END

class CTargetCamera final:
	public CCamera
{
private:
	CTargetCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargetCamera(const CTargetCamera& rhs);
	virtual ~CTargetCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual void Tick(_double TimeDelta)override;
	virtual void LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	void Target_Renewal();

private:
	void Mouse_Check(_double TimeDelta);
	void Mouse_Fix();

private:
	CGameInstance* m_pInstance = nullptr;
	_bool m_bCheck = false;
	_bool m_bFix = false;

public:
	static CTargetCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free()override;
};

