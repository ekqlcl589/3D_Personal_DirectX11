#pragma once
#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTwoHandedSwordWait final : public CWeapon
{
private:
	CTwoHandedSwordWait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTwoHandedSwordWait(const CTwoHandedSwordWait& rhs);
	virtual ~CTwoHandedSwordWait() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision(CGameObject* pObj)override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float3 m_fScale = { 0.7f, 0.7f, 0.7f };
public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;

};

END
