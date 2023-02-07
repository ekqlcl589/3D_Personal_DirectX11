#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CRenderer final :
	public CComponent
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// renderer�� ����� ������Ʈ�� �ƴϱ� ������ ���� �����ڴ� ������ ����
	virtual ~CRenderer() = default;

public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_NONALPHA, RENDER_NONLIGHT, RENDER_END};

public:
	virtual HRESULT Initialize_Prototype()override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw_Renderer();
	
private:
	list<class CGameObject*> m_RenderObject[RENDER_END];
	typedef list<class CGameObject*> RENDEROBJECT;

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free();
};

END