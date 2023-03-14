#pragma once
#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTwoHandedSword final : public CWeapon
{
private:
	CTwoHandedSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTwoHandedSword(const CTwoHandedSword& rhs);
	virtual ~CTwoHandedSword() = default;

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

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;

};

END
