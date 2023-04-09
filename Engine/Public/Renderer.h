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
	enum RENDERGROUP 
	{
		RENDER_PRIORITY,
		RENDER_NONALPHA, 
		RENDER_BLUR,
		RENDER_GLOW,
		RENDER_NONLIGHT, 
		RENDER_ALPHABLEND, 
		RENDER_UI, 
		RENDER_END
	};

public:
	virtual HRESULT Initialize_Prototype()override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw_Renderer();
	
#ifdef _DEBUG
public:
	HRESULT Add_DebugGroup(class CComponent* pComponent); // ��ü�� �߰��Ǿ� �ִ� ������Ʈ���� �� ������ ���� �ʰ� �׸��� ���� �߰� 
	HRESULT Render_DebugGroup();
#endif // _DEBUG

private:
	list<class CGameObject*> m_RenderObject[RENDER_END];
	typedef list<class CGameObject*> RENDEROBJECT;

#ifdef _DEBUG
private:
	list<class CComponent*> m_DebugRenderObjects;
#endif // _DEBUG

private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
	class CShader* m_pShader = nullptr;
	class CShader* m_pBlurShader = nullptr;
	_float4x4 m_FullScreenWorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Mgr* m_pLight_Manager = { nullptr };

private:
	void Render_Priority();
	void Render_NonAlphaBlend();
	void Render_Lights();
	void Render_Blur();
	void Render_Glow();

	void Render_Blend();
	void Render_NonLight();
	void Render_AlphaBlend();
	void Render_UI();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free();
};

END