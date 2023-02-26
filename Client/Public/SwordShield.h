#pragma once
#include "Weapon.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSwordShield final : public CWeapon
{
private:
	CSwordShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordShield(const CSwordShield& rhs);
	virtual ~CSwordShield() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;

};

END
