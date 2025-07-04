#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxEffect/ShaderConstant.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "./dxeffectcamera.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------[CAMERA_PROPERTY]
const DWORD	CAMERA_PROPERTY::VERSION	= 0x0101;
const char	CAMERA_PROPERTY::NAME[]	= "CAMERA_PROPERTY";

DxEffSingle* CAMERA_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectCamera *pEffect = new DxEffectCamera;

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj		= m_bMoveObj;

	pEffect->m_iCameraSet	= m_iCameraSet;
	pEffect->m_fValue		= m_fValue;
	pEffect->m_fPlayTime	= 1.f/m_fPlayNum;

	return pEffect;
}

HRESULT CAMERA_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [정적 맴버 정의 및 초기화] ----------------------------------------------
//
//
const DWORD	DxEffectCamera::TYPEID	= EFFSINGLE_CAMERA;
const DWORD	DxEffectCamera::FLAG		= NULL;
const char	DxEffectCamera::NAME[]	= "카메라";

//	Note : 
//
BOOL DxEffectCamera:: VAILEDDEVICE				= FALSE;

HRESULT DxEffectCamera::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	VAILEDDEVICE = TRUE;
	return S_OK;
}

HRESULT DxEffectCamera::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;
	return S_OK;
}

void DxEffectCamera::OnInitDevice_STATIC()
{
}

void DxEffectCamera::OnDeleteDevice_STATIC()
{
}


HRESULT DxEffectCamera::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectCamera::InvalidateDeviceObjects ()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [클레스 본체] ------------------------------------------------------------
//
//
DxEffectCamera::DxEffectCamera () :
	m_pd3dDevice(NULL)
{
	m_fGLifeTime	= 0.0f;
	m_fTimeSum		= 0.f;

	m_vCameraMoveSum	= D3DXVECTOR3 (0.f,0.f,0.f);
}

DxEffectCamera::~DxEffectCamera ()
{
	CleanUp ();

	D3DXVECTOR3	*pFromPt	= &DxViewPort::GetInstance().GetFromPt();
	//D3DXVECTOR3	*pLookatPt	= &DxViewPort::GetInstance().GetLookatPt();
	pFromPt->y		-= m_vCameraMoveSum.y;
	//pLookatPt->y	-= m_vCameraMoveSum.y;
	m_vCameraMoveSum.y = 0.f;
}

HRESULT DxEffectCamera::FinalCleanup()
{
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffectCamera::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vUp( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXMATRIX		matLocal;

	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fTimeSum	+= fElapsedTime;	// 누적 시간

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )
	{
		goto _RETURN;
	}

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )	
		{
			m_dwRunFlag |= EFF_PLY_PLAY;
		}
		else
		{
			// 시작전임.
			goto _RETURN;
		}
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )			
	{
		m_dwRunFlag |= EFF_PLY_END;
	}

	if ( m_dwRunFlag&EFF_PLY_RENDPASS )	
	{
		D3DXVECTOR3	*pFromPt = &DxViewPort::GetInstance().GetFromPt();
		//D3DXVECTOR3	*pLookatPt = &DxViewPort::GetInstance().GetLookatPt();
		pFromPt->y		-= m_vCameraMoveSum.y;
		//pLookatPt->y	-= m_vCameraMoveSum.y;
		m_vCameraMoveSum.y = 0.f;

		goto _RETURN;	// 흔들리지 않도록
	}

	float		AddY;
	D3DXVECTOR3	*pFromPt = &DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3	*pLookatPt = &DxViewPort::GetInstance().GetLookatPt();

	switch ( m_iCameraSet )
	{
	//	카메라 위 아래로 흔들기
	case CAMERA_MOVE_UP_DOWN:
		if ( m_fTimeSum > m_fPlayTime )
		{
			m_fTimeSum = 0.f;
			AddY = 1.f*m_fValue*((rand()%10)/10.f);		// m_fValue - 진동강도

			if ( m_vCameraMoveSum.y > 0.0f )
			{
				m_vCameraMoveSum.y	-= AddY;
				pFromPt->y			-= AddY;
			//	pLookatPt->y		-= AddY;
			}
			else
			{
				m_vCameraMoveSum.y	+= AddY;
				pFromPt->y			+= AddY;
			//	pLookatPt->y		+= AddY;
			}
		}
		break;

		// 카메라 회전.
	case CAMERA_ROTATION_Y:
		// 0.0173f 이라는 값은 공식이 아닌 Test 로 정해진 값임. // m_fValue(360) 수치를 넣으면 1초당 360 도를 돌기위한 수치임.
		DxViewPort::GetInstance().CameraRotation( D3DXVECTOR3( 0.f, (m_fValue * fElapsedTime) * 0.0173f, 0.f ), FALSE );		// m_fValue - 회전속도
		break;
	}

	if( m_dwRunFlag & EFF_PLY_END )
	{
		pFromPt->y		-= m_vCameraMoveSum.y;
	//	pLookatPt->y	-= m_vCameraMoveSum.y;
		m_vCameraMoveSum.y = 0.f;
	}

	DxViewPort::GetInstance().SetViewTrans( *pFromPt, *pLookatPt, vUp );
	DxViewPort::GetInstance().ComputeClipVolume();

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectCamera::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return hr;
}