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
	void Target_Renewal(_double TimeDelta);

	void Test(_double TimeDelta);

	void Key_Input(_double TimeDelta);

public:
	void SetMaxAt(_float _MaxAt) { m_fMaxAtX = _MaxAt; }
	void View_TarGet(_double TimeDelta);

public:
	void Set_CameraPos(_float x, _float z);
	void Camera_Shake(_fvector vLook, _double TimeDelta); // 범위 지정 STATE_LOOK

private:
	void Mouse_Check(_double TimeDelta);
	void Mouse_Fix();

public:
	void Set_TalkState(bool _bTalkState) { m_bTalkState = _bTalkState; }
	void Set_NpcPos(_float3 _vNpcPos) { m_vNpcPos = _vNpcPos; }
	void Set_bSome(bool _bSome) { m_bSome = _bSome; }
	void Set_AdjustXY(bool _AdjustX, bool _AdjustY)
	{
		m_AdjustX = _AdjustX;
		m_AdjustY = _AdjustY;
	}

private:
	CGameInstance* m_pInstance = nullptr;
	_float m_fRation = 0;
	_bool m_bCheck = false;
	_bool m_bFix = false;

	_float3 m_fDistance = { 0.f, 0.f, 0.f };
	_float m_fDis = 6.f;

	_vector m_vLook = { 1.0f, 0.f, 0.f };

	_float4x4 m_matWorld;

	_float m_fAngleX = 0.f;
	_float m_fAngleY = 0.f;

private:
	_float m_fDistanceValue = 5.f;

	_float m_fMaxEyeX = 0.f;
	_float m_fMaxAtX = 0.f;

	_bool m_bTalkState = false;
	_float3 m_vNpcPos = { 0.f , 0.f , 0.f };
	_bool m_bSome = false;
	_bool m_AdjustX = false;
	_bool m_AdjustY = false;

public:
	static CTargetCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free()override;
};

