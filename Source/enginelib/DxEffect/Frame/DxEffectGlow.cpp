// DxEffectGlow.cpp: implementation of the DxEffectGlow class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxGlowMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "./DxEffectGlow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DXUSERMATERIAL::SaveFile( sc::SerialFile &SFile )
{
    SFile << bGlowUse;
    SFile << bColorUse;
    SFile << cColor;

    SFile << DWORD( 1 );

    SFile.WriteBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.WriteBuffer( szGlowTex, sizeof( char ) * MAX_PATH );
}

void DXUSERMATERIAL::LoadFile( sc::SerialFile &SFile )
{
    SFile >> bGlowUse;
    SFile >> bColorUse;
    SFile >> cColor;

    DWORD nTemp;
    SFile >> nTemp;

    SFile.ReadBuffer( szTexture, sizeof( char ) * MAX_PATH );
    SFile.ReadBuffer( szGlowTex, sizeof( char ) * MAX_PATH );
}

bool DXUSERMATERIAL::LoadTexture( DWORD mipMap, bool bBackgroundLoading )
{
    if( _tcslen( szGlowTex ) <= 0 ) 
        return false;

	textureRes = TextureManager::GetInstance().LoadTexture(
        szGlowTex,
		false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        mipMap,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		bBackgroundLoading,
		FALSE );

    return true;
}

LPDIRECT3DSTATEBLOCK9	DxEffectGlow::m_pSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectGlow::m_pSB_SAVE = NULL;

const DWORD DxEffectGlow::TYPEID = DEF_EFFECT_GLOW;
const DWORD	DxEffectGlow::VERSION = 0x00000100;
const char DxEffectGlow::NAME[] = "[ Glow effect ]";
const DWORD DxEffectGlow::FLAG = NULL;

void DxEffectGlow::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(GLOW_PROPERTY);
	dwVer = VERSION;
}

void DxEffectGlow::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(GLOW_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectGlow::DxEffectGlow() :
	m_dwFlag(0L),
	m_pTempTexture(NULL),

	m_fTime(0),

	m_dwFrameMaterials(0),
	m_pFrameMaterials(NULL),

	m_dwMaterials(0),
	m_pMaterials(NULL)
{
	m_dwFlag |= (USEDAY|USENIGHT);
}

DxEffectGlow::~DxEffectGlow()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pFrameMaterials );
}

HRESULT DxEffectGlow::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}
	
	return S_OK;
}

HRESULT DxEffectGlow::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );

	return S_OK;
}

HRESULT DxEffectGlow::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			DxAnalysis ( pd3dDevice, pmsMeshs );
			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectGlow::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );;
}


HRESULT DxEffectGlow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectGlow::DeleteDeviceObjects ()
{
	return S_OK;
}

void DxEffectGlow::ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
        m_pMaterials[i].OnDestroyDevice();
        m_pMaterials[i].LoadTexture( 0, true );
	}
}

HRESULT DxEffectGlow::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;
	return S_OK;
}

HRESULT DxEffectGlow::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	return S_OK;
	if ( !DxGlowMan::GetInstance().IsOptionEnable() )						return S_OK;	// 옵션에서 꺼져 있으면 동작하지 않는다.
	if ( DxLightMan::GetInstance()->IsNightAndDay() == FALSE )				return S_OK;	// 무조건 낮 옵션이 있을경우 사용안한다;
	if ( GLPeriod::GetInstance().IsOffLight() && !(m_dwFlag&USEDAY) )		return S_OK;	// 낮에 사용 안한다;
	if ( !GLPeriod::GetInstance().IsOffLight() && !(m_dwFlag&USENIGHT) )	return S_OK;	// 밤에 사용 안한다.

	if ( !DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
		return S_OK;

	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );

	DxGlowMan::GetInstance().SetGlowON ();			// 글로우 효과를 사용 한다고 알려준다.

	//	Note : Render 
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 메쉬 그리기.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFERQ pIB	= NULL;
				LPDIRECT3DVERTEXBUFFERQ pVB = NULL;

				//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					
					dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwFVFSize = pmsMeshs->m_pDxOctreeMesh->m_dwFVF;
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}

				pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

				DWORD					AttribTableSize = 0;
				LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;
				
				if ( pmsMeshs->m_pLocalMesh )
				{	
					pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &AttribTableSize );
				}
				else
				{
					AttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
				}
				
				LPD3DXATTRIBUTERANGE pAttribTable;
				pAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

				if ( pmsMeshs->m_pLocalMesh )
				{
					pmsMeshs->m_pLocalMesh->GetAttributeTable( pAttribTable, &AttribTableSize );
				}
				else
				{
					memcpy ( pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * AttribTableSize );
				}

				//	Note : 불투명 메쉬 드로잉.
				//		pmsMeshs->m_cMaterials --> AttribTableSize
				//
				for ( DWORD i=0; i<AttribTableSize; i++ )
				{
					if ( !m_pMaterials[pAttribTable[i].AttribId].bGlowUse )	continue;

					m_pTempTexture = m_pMaterials[pAttribTable[i].AttribId].GetTexture();

					if ( m_pMaterials[pAttribTable[i].AttribId].bColorUse )
					{
						DxImageMap ( pd3dDevice, m_pMaterials[pAttribTable[i].AttribId].GetTexture(), DxSurfaceTex::GetInstance().m_pTempSuf64, 
												m_pMaterials[pAttribTable[i].AttribId].cColor, 64 );
						m_pTempTexture = DxSurfaceTex::GetInstance().m_pTempTex64;
					}

					pd3dDevice->SetTexture ( 0, m_pTempTexture );
					pd3dDevice->SetMaterial ( &pmsMeshs->m_rgMaterials[pAttribTable[i].AttribId] );

					// Note : StateBlock Capture
					m_pSB_SAVE->Capture();
					m_pSB->Apply();

					//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
					//
					if ( pmsMeshs->m_pLocalMesh )
					{
						pd3dDevice->SetIndices ( pIB );
						pd3dDevice->SetStreamSource ( 0, pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pLocalMesh->GetFVF () );
					}
					else
					{
						pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB );
						pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, 0, dwFVFSize );
						pd3dDevice->SetFVF ( pmsMeshs->m_pDxOctreeMesh->m_dwFVF );
					}

					pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );

					pd3dDevice->DrawIndexedPrimitive
					(
						D3DPT_TRIANGLELIST,
						0,
						pAttribTable[i].VertexStart,
						pAttribTable[i].VertexCount,
						pAttribTable[i].FaceStart * 3,
						pAttribTable[i].FaceCount
					);

					// Note : StateBlock Apply
					m_pSB_SAVE->Apply();
				}

				SAFE_DELETE(pAttribTable);

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
					if(pVB) pVB->Release ();
				}
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render ( pframeChild, pd3dDevice, pFrameMesh, pLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );

	return S_OK;
}

HRESULT DxEffectGlow::DxAnalysis ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	HRESULT	hr = S_OK;

	m_dwMaterials = pmsMeshs->m_cMaterials;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXUSERMATERIAL[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pmsMeshs->m_strTextureFiles[i].GetString() );

		if ( !strlen(m_pMaterials[i].szGlowTex) )
			StringCchCopy( m_pMaterials[i].szGlowTex, MAX_PATH, pmsMeshs->m_strTextureFiles[i].GetString() );

         m_pMaterials[i].LoadTexture( 0, true );
	}

	if ( pmsMeshs->m_pDxOctreeMesh )
	{
		if ( !m_pFrameMaterials )	return S_OK;

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			for ( DWORD j=0; j<m_dwFrameMaterials; j++ )
			{
				if ( !strcmp ( m_pMaterials[i].szTexture, m_pFrameMaterials[j].szTexture ) )
				{
					m_pMaterials[i].bColorUse	= m_pFrameMaterials[j].bColorUse;
					m_pMaterials[i].bGlowUse	= m_pFrameMaterials[j].bGlowUse;
					m_pMaterials[i].cColor		= m_pFrameMaterials[j].cColor;
					StringCchCopy( m_pMaterials[i].szGlowTex, MAX_PATH, m_pFrameMaterials[j].szGlowTex );

                    m_pMaterials[i].OnDestroyDevice();
                    m_pMaterials[i].LoadTexture( 0, true );
				}
			}
		}
	}

	return hr;
}

void DxEffectGlow::SaveBuffer ( sc::SerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(DXUSERMATERIAL_OLD)*m_dwFrameMaterials
			+sizeof(DWORD)+sizeof(DXUSERMATERIAL_OLD)*m_dwMaterials;

	// Material 저장
	SFile << m_dwFrameMaterials;
	if ( m_pFrameMaterials )
    {
        for( DWORD i=0; i<m_dwFrameMaterials; ++i )
            m_pFrameMaterials[ i ].SaveFile( SFile );
    }

	SFile << m_dwMaterials;
	if ( m_pMaterials )
    {
        for( DWORD i=0; i<m_dwMaterials; ++i )
            m_pMaterials[ i ].SaveFile( SFile );
    }
}

void DxEffectGlow::LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwFrameMaterials;
	SAFE_DELETE_ARRAY ( m_pFrameMaterials );
	if ( m_dwFrameMaterials )
	{
		m_pFrameMaterials = new DXUSERMATERIAL[m_dwFrameMaterials];
		for( DWORD i=0; i<m_dwFrameMaterials; ++i )
            m_pFrameMaterials[ i ].LoadFile( SFile );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXUSERMATERIAL[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
        {
            m_pMaterials[ i ].LoadFile( SFile );
            m_pMaterials[i].LoadTexture( 0, true );
		}
	}
}

void DxEffectGlow::LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	// Material
	SFile >> m_dwFrameMaterials;
	SAFE_DELETE_ARRAY ( m_pFrameMaterials );
	if ( m_dwFrameMaterials )
	{
		m_pFrameMaterials = new DXUSERMATERIAL[m_dwFrameMaterials];
		for( DWORD i=0; i<m_dwFrameMaterials; ++i )
            m_pFrameMaterials[ i ].LoadFile( SFile );
	}

	// Material
	SFile >> m_dwMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	if ( m_dwMaterials )
	{
		m_pMaterials = new DXUSERMATERIAL[m_dwMaterials];
		for( DWORD i=0; i<m_dwMaterials; ++i )
        {
            m_pMaterials[i].LoadFile( SFile );
			m_pMaterials[i].LoadTexture( 0, true );
		}
	}
}

HRESULT DxEffectGlow::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff )
{
	DxEffectGlow* pSrcEff = (DxEffectGlow*) pSrcEffect;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pFrameMaterials );

	// Material 
	if ( pSrcEff->m_pMaterials )
	{
		//	원본 만들기
		m_dwMaterials = pSrcEff->m_dwMaterials;
		m_pMaterials = new DXUSERMATERIAL[pSrcEff->m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pMaterials[i] = pSrcEff->m_pMaterials[i];
			m_pMaterials[i].LoadTexture( 0, true );
		}

		//	Frame 만들기
		m_dwFrameMaterials = pSrcEff->m_dwMaterials;
		m_pFrameMaterials = new DXUSERMATERIAL[m_dwFrameMaterials];

        for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
            m_pFrameMaterials[i] = pSrcEff->m_pMaterials[i];

            m_pFrameMaterials[i].textureRes.Clear();
        }
	}

	return S_OK;
}