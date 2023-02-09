#pragma once

namespace Engine
{
	/* 그래픽 디바이스를 초기화하기위해 필요한 데이터들 */
	typedef struct tagGraphic_Desc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		WINMODE			eMode;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		HWND			hWnd;

	}GRAPHIC_DESC;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexx_Position_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC Element[2];
	}VTXTEX_DELARATION;

	typedef struct tagVertexCube_TEXTURECUBE
	{
		XMFLOAT3			vPosition;			// 위치
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;			// 텍스처 uv좌표

	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertexCube_Position_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC Element[2];
	}VTXCUBE_DELARATION;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexx_Position_Normal_TexCoord_Decleartion
	{
		const static unsigned int iNumElements = 3;
		static D3D11_INPUT_ELEMENT_DESC Element[3];
	}VTXNORTEX_DELARATION;

	typedef struct tagLightDesc
	{
		enum LIGHT_TYPE{ LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_END};

		LIGHT_TYPE eLightType = LIGHT_END;

		_float4 vDirection; // 방향

		_float4 vPosition; // 점
		float fRange;

		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;


	}LIGHT_DESC;

	typedef struct tagVertex_NonAnim_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXNONANIMMODEL;

	typedef struct ENGINE_DLL tagVertexx_tagVertex_NonAnim_Model_Decleartion
	{
		const static unsigned int iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC Element[4];
	}VTXNONANIMMODEL_DELARATION;

	typedef struct tagModelMaterial
	{
		class CTexture* pMaterialTexture[AI_TEXTURE_TYPE_MAX] = { nullptr };
	}MODEL_MATERIAL;

}


