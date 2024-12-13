#include "pch.h"
#include "CScene_Start.h"
#include "CObject.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CCore.h"
#include "CPathMgr.h"
#include "CTexture.h"
#include "CResMgr.h"
#include "CRes.h"

#include "CCollisionMgr.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"
#include "CSoundMgr.h"
#include "CUIMgr.h"
#include "CSound.h"


#include "CCamera.h"
#include "AI.h"
#include "CIdleState.h"
#include "CTraceState.h"
#include "CRigidBody.h"
#include "SelectGDI.h"
#include "CTimeMgr.h"
#include "CGround.h"
#include "CBlock.h"
#include "CFlag.h"
#include "CBackGround.h"
#include "CUI.h"
#include "CBtnUI.h"

#include "Networking.h"
Networking net;

CScene_Start::CScene_Start()
	:m_bUseForce(false)
	, m_fForceRadius(500.f)
	, m_fCurRadius(0.f)
	, m_fForce(500.f)
	, m_vecHeart(0)
	, m_bHeartDelete(false)
	, m_bSceneEnd(false)
	, m_fEndTime(0.f)
	, m_csBgm(nullptr)
	, m_bPhase(false)
	, m_pUI_Reset(nullptr)
	, m_pUI_Resume(nullptr)
	, m_pUI_End(nullptr)
	, m_pUI_Main(nullptr)
{
}

CScene_Start::~CScene_Start()
{
	//Safe_Delete_Vec(m_vecHeart);
}

void Reset(DWORD_PTR a, DWORD_PTR b)
{
	ChangeScene(SCENE_TYPE::START);
}

void CScene_Start::Resume()
{
	m_bPhase = false;
	DeleteObject(m_pUI_End);
	DeleteObject(m_pUI_Reset);
	DeleteObject(m_pUI_Resume);
	DeleteObject(m_pUI_Main);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::MONSTER);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::PROJ_PLAYER);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::GROUND);

	// 캐릭터와 충돌
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::BLOCK);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::COIN);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::FLAG);

	// 몬스터와 코인이 충돌하면 코인 삭제 06/13
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::COIN);
}

void Main(DWORD_PTR a, DWORD_PTR b)
{
	ChangeScene(SCENE_TYPE::INTRO);
}

void End(DWORD_PTR a, DWORD_PTR b)
{
	ChangeScene(SCENE_TYPE::EnD);
}

void CScene_Start::update()
{

	// 오버라이딩 하긴 했지만 부모의 upadate 코드를 갖다 쓸 것이다.
	//CScene::update();
	// 이럼 오버라이딩 왜함?
	// + @ 뭔가를 더 하겠다.
	//if (KEY_TAP(KEY::ENTER))
	//{
	//	// 엔터키가 눌리면 TOOL_SCENE으로 바꾸겠다
	//	// 근데 바로 씬을 바꾸는 게 아니라 이벤트에 등록해서 업뎃-파이널업뎃-렌더가 끝난 후 이벤트 처리
	//	ChangeScene(SCENE_TYPE::TOOL);
	//	
	//}

	/*if (KEY_TAP(KEY::LBTN))
	{
		Vec2 LookAt = CCamera::GetInst()->GetRealPos(MOUSE_POS);
		CCamera::GetInst()->SetLookAt(LookAt);
	}*/
	if (KEY_TAP(KEY::ESC))
	{
		if (m_bPhase == false)
		{
			m_bPhase = true;
			CCollisionMgr::GetInst()->Reset();

			//	// UI 생성 - 리셋
			//	m_pUI_Reset = new CBtnUI;
			//	m_pUI_Reset->SetName(L"Reset");
			//	m_pUI_Reset->SetScale(Vec2(300.f, 80.f));
			//	m_pUI_Reset->SetPos(Vec2(CCore::GetInst()->GetResolution().x / 2 - m_pUI_Reset->GetScale().x / 2, 300.f));
			//	m_pUI_Reset->SetTex(CResMgr::GetInst()->LoadTexture(L"ResetBtn", L"texture\\ReStartBtn.bmp"));
			//	((CBtnUI*)m_pUI_Reset)->SetClickedCallBack(Reset, 0, 0);
			//	AddObject(m_pUI_Reset, GROUP_TYPE::UI);

			//	// UI 생성 - 재개
			//	m_pUI_Resume = new CBtnUI;
			//	m_pUI_Resume->SetName(L"Resume");
			//	m_pUI_Resume->SetScale(Vec2(300.f, 80.f));
			//	m_pUI_Resume->SetPos(Vec2(CCore::GetInst()->GetResolution().x / 2 - m_pUI_Resume->GetScale().x / 2, 400.f));
			//	m_pUI_Resume->SetTex(CResMgr::GetInst()->LoadTexture(L"ResumeBtn", L"texture\\ResumeBtn.bmp"));
			//	((CBtnUI*)m_pUI_Resume)->SetClickedCallBack(this,(SCENE_MEMFUNC)&CScene_Start::Resume);
			//	AddObject(m_pUI_Resume, GROUP_TYPE::UI);

			//	// UI 생성 - 메인화면
			//	m_pUI_Main = new CBtnUI;
			//	m_pUI_Main->SetName(L"Main");
			//	m_pUI_Main->SetScale(Vec2(300.f, 80.f));
			//	m_pUI_Main->SetPos(Vec2(CCore::GetInst()->GetResolution().x / 2 - m_pUI_Main->GetScale().x / 2, 500.f));
			//	m_pUI_Main->SetTex(CResMgr::GetInst()->LoadTexture(L"MainBtn", L"texture\\MainBtn.bmp"));
			//	((CBtnUI*)m_pUI_Main)->SetClickedCallBack(Main, 0, 0);
			//	AddObject(m_pUI_Main, GROUP_TYPE::UI);

			//	// UI 생성 - 종료
			//	m_pUI_End = new CBtnUI;
			//	m_pUI_End->SetName(L"End");
			//	m_pUI_End->SetScale(Vec2(300.f, 80.f));
			//	m_pUI_End->SetPos(Vec2(CCore::GetInst()->GetResolution().x / 2 - m_pUI_End->GetScale().x / 2, 600.f));
			//	m_pUI_End->SetTex(CResMgr::GetInst()->LoadTexture(L"QuitBtn", L"texture\\QuitBtn.bmp"));
			//	((CBtnUI*)m_pUI_End)->SetClickedCallBack(End, 0, 0);
			//	AddObject(m_pUI_End, GROUP_TYPE::UI);
		}
		else if (m_bPhase == true)
		{
			m_bPhase = false;
			/*	DeleteObject(m_pUI_End);
				DeleteObject(m_pUI_Reset);
				DeleteObject(m_pUI_Resume);
				DeleteObject(m_pUI_Main);*/
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::MONSTER);
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::PROJ_PLAYER);
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::GROUND);

			// 캐릭터와 충돌
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::BLOCK);
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::COIN);
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::FLAG);

			// 몬스터와 코인이 충돌하면 코인 삭제 06/13
			CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::COIN);
		}
	}


	if (m_bPhase)
	{
		/*const vector<CObject*>& vecUI = GetGroupObject(GROUP_TYPE::UI);
		for (int i = 0; i < vecUI.size(); ++i)
			vecUI[i]->update();*/
		return;
	}



	if (KEY_HOLD(KEY::LBTN))
	{
		m_bUseForce = true;
		CreateForce();
	}
	else
	{
		m_bUseForce = false;
	}



	for (UINT i = 0; i < (UINT)GROUP_TYPE::END; ++i)
	{
		const vector<CObject*>& vecObj = GetGroupObject((GROUP_TYPE)(i));
		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			if (!vecObj[j]->IsDead())
			{
				if (m_bUseForce && vecObj[j]->GetRigidBody())
				{
					Vec2 vDiff = vecObj[j]->GetPos() - m_vForcePos;
					float fLen = vDiff.Length();
					// 물체와 힘이 발생한 지점간의 거리가, 힘의 반경보다 작으면 그 물체는 반경 안에 있다
					if (fLen < m_fForceRadius)
					{
						// 힘을 얼마나 받는 지에 대한 비율
						float fRatio = 1.f - (fLen / m_fForceRadius);
						// 적용될 힘
						float fForce = m_fForce * fRatio;		// 방향		*	적용될 힘의 크기
						vecObj[j]->GetRigidBody()->AddForce(vDiff.Normalize() * fForce);

					}
				}
				// ★★★★★각오브젝트의 업데이트 수행★★★★★
				vecObj[j]->update();
			}
		}
	}

	if (((CPlayer*)GetPlayer())->GetHP() == 4 && ((CPlayer*)GetPlayer())->GetDamaged() == true)
	{
		((CPlayer*)GetPlayer())->SetDamaged(false);
		DeleteObject(m_vecHeart.back());
		m_vecHeart.pop_back();
	}
	if (((CPlayer*)GetPlayer())->GetHP() == 3 && ((CPlayer*)GetPlayer())->GetDamaged() == true)
	{
		((CPlayer*)GetPlayer())->SetDamaged(false);
		DeleteObject(m_vecHeart.back());
		m_vecHeart.pop_back();
	}
	if (((CPlayer*)GetPlayer())->GetHP() == 2 && ((CPlayer*)GetPlayer())->GetDamaged() == true)
	{
		((CPlayer*)GetPlayer())->SetDamaged(false);
		DeleteObject(m_vecHeart.back());
		m_vecHeart.pop_back();
	}
	if (((CPlayer*)GetPlayer())->GetHP() == 1 && ((CPlayer*)GetPlayer())->GetDamaged() == true)
	{
		((CPlayer*)GetPlayer())->SetDamaged(false);
		DeleteObject(m_vecHeart.back());
		m_vecHeart.pop_back();
	}
	if (((CPlayer*)GetPlayer())->GetHP() == 0 && ((CPlayer*)GetPlayer())->GetDamaged() == true)
	{
		((CPlayer*)GetPlayer())->SetDamaged(false);
		DeleteObject(m_vecHeart.back());
		m_vecHeart.pop_back();


		CResMgr::GetInst()->LoadSound(L"End", L"sound\\end.wav");
		CSound* pNewSound = CResMgr::GetInst()->FindSound(L"End");
		pNewSound->Play();
		//m_csBgm->Stop();
		m_bSceneEnd = true;
	}

	// 플레이어 낙사 코드 추가 필요
	if (GetPlayer()->GetPos().y >= 900 && m_bSceneEnd == false)
	{
		CResMgr::GetInst()->LoadSound(L"End", L"sound\\end.wav");
		CSound* pNewSound = CResMgr::GetInst()->FindSound(L"End");
		pNewSound->Play();
		//m_csBgm->Stop();
		m_bSceneEnd = true;
	}


	if (m_bSceneEnd == true)
	{
		m_fEndTime += fDT;
		if (m_fEndTime >= 2.f)
			ChangeScene(SCENE_TYPE::EnD);
	}

	switch (net.other)
	{
	
	case JO:
	{
		//CObject* opObj = net.CreatePlayer();
		// 새 접속 플레이어 추가
		//AddObject(opObj, GROUP_TYPE::PLAYER);
		// 현재 씬에 플레이어 등록
		//RegisterPlayer(opObj);
		net.other = ID;  // 상태를 초기화하여 새 플레이어 생성 중복 방지
		break;
	}
	
	case QU:
		// 플레이어 죽는 코드 추가
		break;
	case MO:
	{
		// 플레이어 이동 코드 추가
		break;
	}
	case ID:
	{
		Vec2 pos = GetPlayer()->GetPos();
		net.px = pos.x;
		net.py = pos.y;
		net.sendPos(CCore::GetInst()->m_iCoin);
		break;
	}
	default:
		break;
	}

	if (KEY_TAP(KEY::SPACE))
		net.sendJump();
}

void CScene_Start::finalupdate()
{
	if (m_bPhase)
	{
	/*	const vector<CObject*>& vecUI = GetGroupObject(GROUP_TYPE::UI);
		for (int i = 0; i < vecUI.size(); ++i)
			vecUI[i]->finalupdate();*/
		return;
	}
	
		
	CScene::finalupdate();
}

void CScene_Start::render(HDC _dc)
{


	CScene::render(_dc);

	//if (!m_bUseForce)
	//	return;
	//SelectGDI gdi1(_dc,BRUSH_TYPE::HOLLOW);
	//SelectGDI gdi2(_dc, PEN_TYPE::GREEN);

	/*m_fCurRadius += m_fForceRadius * 2.f * fDT;
	if (m_fCurRadius > m_fForceRadius)
	{
		m_fCurRadius = 0.f;
	}
	Vec2 vRenderPos = CCamera::GetInst()->GetRenderPos(m_vForcePos);

	Ellipse(_dc
		, (int)(vRenderPos.x - m_fCurRadius)
		, (int)(vRenderPos.y - m_fCurRadius)
		, (int)(vRenderPos.x + m_fCurRadius)
		, (int)(vRenderPos.y + m_fCurRadius));*/
		//m_vForcePos;
	LOGFONT lf;
	lf.lfHeight = 50;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = HANGEUL_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	lstrcpy(lf.lfFaceName, TEXT("궁서"));

	HFONT hFont = CreateFontIndirect(&lf);

	HFONT OldFont = (HFONT)SelectObject(_dc, hFont);
	wchar_t wsCoin[100] = L"coin";
	wsprintfW(wsCoin, L"COIN : %d ", CCore::GetInst()->m_iCoin);
	TextOut(_dc, 10, 10, wsCoin, lstrlenW(wsCoin));
	if (net.other_coin)
	{
		wsprintfW(wsCoin, L"OTHER_COIN : %d ", net.other_coin);
		TextOut(_dc, 300, 10, wsCoin, lstrlenW(wsCoin));
	}
	SelectObject(_dc, OldFont);
	DeleteObject(hFont);
}

void CScene_Start::Enter()
{
	if (!net.Init())
		net.sendEnter();
		net.Run();
	 
	CCore::GetInst()->m_iCoin = 0;
	CCore::GetInst()->m_iHP = 5;

	// 06/16 사운드 추가
	CSoundMgr::GetInst()->init();
	// 각 신 업데이트에 넣으면 될듯
	// Sound 로드 테스트★★★★★★
	CResMgr::GetInst()->LoadSound(L"BGM_01", L"sound\\BGM.wav");
	m_csBgm = CResMgr::GetInst()->FindSound(L"BGM_01");
	m_csBgm->Play();
	m_csBgm->SetPosition(0.f);			// 백분율, 재생 위치 설정 ex 50% 구간
	m_csBgm->PlayToBGM(true);				// BGM으로 Play 
	m_csBgm->SetVolume(30.f);				// 0~100 까지 볼륨 설정pNewSound
	


	// 해당 신이 시작될 때
	// CObject : 백그라운드 추가
	CObject* pObj1 = new CBackGround;
	pObj1->SetName(L"BG");
	pObj1->SetPos(Vec2(0.f, 384.f));
	pObj1->SetScale(Vec2(100.f, 100.f));
	AddObject(pObj1, GROUP_TYPE::BG);


	// CObject : CPlayer 추가
	/*CObject* pObj = new CPlayer;
	pObj->SetName(L"Player");
	pObj->SetPos(Vec2(0.f, 384.f));
	pObj->SetScale(Vec2(267.f, 133.f));
	AddObject(pObj, GROUP_TYPE::PLAYER);*/

	CObject* pObj = net.CreatePlayer();
	// 새 접속 플레이어 추가
	AddObject(pObj, GROUP_TYPE::PLAYER);

	// 현재 씬에 플레이어 등록
	RegisterPlayer(pObj);

	// 플레이어 ★다운캐스팅 복사생성 >> 너무 기니까 clone함수 구현해서 하자
	/*CObject* pOtherPlayer = new CPlayer(*(CPlayer*)pObj);
	pOtherPlayer->SetPos(Vec2(740.f, 384.f));
	AddObject(pOtherPlayer, GROUP_TYPE::PLAYER);*/

	/*CObject* pOtherPlayer = pObj->Clone();
	pOtherPlayer->SetPos(Vec2(pObj->GetPos().x + 100, pObj->GetPos().y));
	AddObject(pOtherPlayer, GROUP_TYPE::PLAYER);*/

	// 카메라가 플레이어 따라다니기
	CCamera::GetInst()->SetTarget(pObj);

	CCamera::GetInst()->SetLookAt(Vec2(GetPlayer()->GetPos().x, 400.f));
	//// 몬스터 배치
	//// 몬스터 팩토리를 통한 생성
	Vec2 vResolution = CCore::GetInst()->GetResolution();
	//CMonster* pMon = CMonFactory::CreateMonster(MON_TYPE::NORMAL, vResolution / 2.f - Vec2(0.f, 300.f));
	//AddObject(pMon, GROUP_TYPE::MONSTER);

	//CreateObject(pMon) 이벤트 등록을 통한 enter가 아닌 update 중 오브젝트 생성하기


	// 수동
	//int iMonCount = 1;
	////float fMoveDistance = 25.f;
	//float fObjScale = 50.f;
	//Vec2 vResolution = CCore::GetInst()->GetResolution();
	////float fTerm = (vResolution.x - ((fMoveDistance + fObjScale/ 2.f) * 2)) / (float)(iMonCount - 1);
	//
	//// State를 가진 AI 생성
	//AI* pAI = new AI;
	//pAI->AddState(new CIdleState);
	//pAI->AddState(new CTraceState);

	//CMonster* pMonsterObj = nullptr;
	//// CObject : CMonster 추가
	//for (int i = 0; i < iMonCount; ++i)
	//{
	//	// CObject : CMonster 추가
	//	pMonsterObj = new CMonster;
	//	pMonsterObj->SetName(L"Monster");
	//	pMonsterObj->SetPos(vResolution / 2.f - Vec2(0.f,300.f));
	//	pMonsterObj->SetScale(Vec2(fObjScale, fObjScale));
	//	// AI 설정
	//	pMonsterObj->SetAI(pAI);
	////	pMonsterObj->SetMoveDistance(fMoveDistance);
	////	pMonsterObj->SetCenterPos(pMonsterObj->GetPos());


	//	AddObject(pMonsterObj, GROUP_TYPE::MONSTER);
	//}
	//LoadTile(L"Tile\\Start.tile");
	// 
	// 

	//// 땅 물체 배치 
	CObject* pGround1 = new CGround;
	pGround1->SetName(L"Ground");
	pGround1->SetPos(Vec2(3500.f, 800.f));
	pGround1->SetScale(Vec2(7000.f, 400.f));
	AddObject(pGround1, GROUP_TYPE::GROUND);

	CObject* pGround11 = new CGround;
	pGround11->SetName(L"Ground");
	pGround11->SetPos(Vec2(4000.f, 440.f));
	pGround11->SetScale(Vec2(700.f, 100.f));
	AddObject(pGround11, GROUP_TYPE::GROUND);
	CObject* pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(4000.f, 340.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);

	CMonster* pMon = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(4350.f, 540.f));
	AddObject(pMon, GROUP_TYPE::MONSTER);

	CObject* pGround2 = new CGround;
	pGround2->SetName(L"Ground");
	pGround2->SetPos(Vec2(8700.f, 800.f));
	pGround2->SetScale(Vec2(3000.f, 400.f));
	AddObject(pGround2, GROUP_TYPE::GROUND);

	CObject* pGround21 = new CGround;
	pGround21->SetName(L"Ground");
	pGround21->SetPos(Vec2(9000.f, 440.f));
	pGround21->SetScale(Vec2(700.f, 100.f));
	AddObject(pGround21, GROUP_TYPE::GROUND);



	CObject* pGround22 = new CGround;
	pGround22->SetName(L"Ground");
	pGround22->SetPos(Vec2(9500.f, 240.f));
	pGround22->SetScale(Vec2(300.f, 100.f));
	AddObject(pGround22, GROUP_TYPE::GROUND);

	// 06/16 거대화 포션 추가 - 테스트
	pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(9500.f, 140.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);

	pMon = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(9850.f, 540.f));
	AddObject(pMon, GROUP_TYPE::MONSTER);

	CObject* pGround23 = new CGround;
	pGround23->SetName(L"Ground");
	pGround23->SetPos(Vec2(11000.f, 440.f));
	pGround23->SetScale(Vec2(300.f, 100.f));
	AddObject(pGround23, GROUP_TYPE::GROUND);

	pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(11000.f, 340.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);
	/*CObject* pGround24 = new CGround;
	pGround24->SetName(L"Ground");
	pGround24->SetPos(Vec2(113000.f, 440.f));
	pGround24->SetScale(Vec2(500.f, 100.f));
	AddObject(pGround24, GROUP_TYPE::GROUND);

	pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(113000.f, 340.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);*/

	CObject* pGround3 = new CGround;
	pGround3->SetName(L"Ground");
	pGround3->SetPos(Vec2(12400.f, 800.f));
	pGround3->SetScale(Vec2(4000.f, 400.f));
	AddObject(pGround3, GROUP_TYPE::GROUND);

	CObject* pGround4 = new CGround;
	pGround4->SetName(L"Ground");
	pGround4->SetPos(Vec2(17600.f, 800.f));
	pGround4->SetScale(Vec2(6000.f, 400.f));
	AddObject(pGround4, GROUP_TYPE::GROUND);

	// 코인 추가
	for (int i = 0; i < 180; ++i)
	{
			CObject* pBlock = new CBlock;
			pBlock->SetName(L"Coin");
			pBlock->SetPos(Vec2(1000.f + 100.f * i, 535.f));
			pBlock->SetScale(Vec2(60.f, 60.f));
			AddObject(pBlock, GROUP_TYPE::COIN);
		if (i % 3 == 0 && i != 0)
		{
			CObject* pBanana1 = new CBlock;
			pBanana1->SetName(L"Banana");
			pBanana1->SetPos(Vec2(1000.f * i + 50, 580.f));
			pBanana1->SetScale(Vec2(60.f, 60.f));
			AddObject(pBanana1, GROUP_TYPE::BLOCK);
		}
	}

	// 06/16 거대화 포션 추가 - 테스트
	pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(900.f, 540.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);

	pBigPotion = new CBlock;
	pBigPotion->SetName(L"Big_Potion");
	pBigPotion->SetPos(Vec2(1900.f, 540.f));
	pBigPotion->SetScale(Vec2(60.f, 60.f));
	AddObject(pBigPotion, GROUP_TYPE::BLOCK);

	// 06/16 바나나 추가 - 테스트
	CObject* pBanana = new CBlock;
	pBanana->SetName(L"Banana");
	pBanana->SetPos(Vec2(1050.f, 580.f));
	pBanana->SetScale(Vec2(60.f, 60.f));
	AddObject(pBanana, GROUP_TYPE::BLOCK);

	pBanana = new CBlock;
	pBanana->SetName(L"Banana");
	pBanana->SetPos(Vec2(1850.f, 580.f));
	pBanana->SetScale(Vec2(60.f, 60.f));
	AddObject(pBanana, GROUP_TYPE::BLOCK);

	// 적 추가 4/2/6/2
	// 추후 몬스터 팩토리로 구현 예정
	// 몬스터 충돌 시 애니메이션 및 움직임 구현 후 변경 해주세요

	//CMonster* pMon = CMonFactory::CreateMonster(MON_TYPE::NORMAL, vResolution / 2.f - Vec2(0.f, 300.f));
	//AddObject(pMon, GROUP_TYPE::MONSTER);
	for (int i = 0; i < 4; ++i)
	{
		/*	CObject* pMonster = new CMonster;
			pMonster->SetName(L"Monster");
			pMonster->SetPos(Vec2(2000.f + 500.f * i, 540.f));
			pMonster->SetScale(Vec2(40.f, 40.f));
			AddObject(pMonster, GROUP_TYPE::MONSTER);*/


		CMonster* pMon1 = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(2000.f + 500.f * i, 540.f));
		AddObject(pMon1, GROUP_TYPE::MONSTER);
	}
	for (int i = 0; i < 2; ++i)
	{
		/*CObject* pMonster = new CMonster;
		pMonster->SetName(L"Monster");
		pMonster->SetPos(Vec2(8000.f + 500.f * i, 540.f));
		pMonster->SetScale(Vec2(40.f, 40.f));
		AddObject(pMonster, GROUP_TYPE::MONSTER);*/


		CMonster* pMon2 = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(8000.f + 500.f * i, 540.f));
		AddObject(pMon2, GROUP_TYPE::MONSTER);
	}
	for (int i = 0; i < 6; ++i)
	{
		/*	CObject* pMonster = new CMonster;
			pMonster->SetName(L"Monster");
			pMonster->SetPos(Vec2(11000.f + 500.f * i, 540.f));
			pMonster->SetScale(Vec2(40.f, 40.f));
			AddObject(pMonster, GROUP_TYPE::MONSTER);*/


		CMonster* pMon3 = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(11000.f + 500.f * i, 540.f));
		AddObject(pMon3, GROUP_TYPE::MONSTER);
	}
	for (int i = 0; i < 2; ++i)
	{
		/*CObject* pMonster = new CMonster;
		pMonster->SetName(L"Monster");
		pMonster->SetPos(Vec2(16000.f + 500.f * i, 540.f));
		pMonster->SetScale(Vec2(40.f, 40.f));
		AddObject(pMonster, GROUP_TYPE::MONSTER);*/


		CMonster* pMon4 = CMonFactory::CreateMonster(MON_TYPE::NORMAL, Vec2(16000.f + 500.f * i, 540.f));
		AddObject(pMon4, GROUP_TYPE::MONSTER);
	}

	// 깃발 추가
	CObject* pFlag = new CFlag;
	pFlag->SetName(L"Flag");
	pFlag->SetPos(Vec2(20300.f, 540.f));
	pFlag->SetScale(Vec2(150.f, 150.f));
	AddObject(pFlag, GROUP_TYPE::FLAG);

	// 생명 추가
	// 1
	CObject* pHeart = new CBlock;
	pHeart->SetName(L"HP");
	pHeart->SetPos(Vec2(-120.f, 50.f));
	pHeart->SetScale(Vec2(50.f, 50.f));
	AddObject(pHeart, GROUP_TYPE::BLOCK);
	m_vecHeart.push_back(pHeart);
	// 2
	pHeart = new CBlock;
	pHeart->SetName(L"HP");
	pHeart->SetPos(Vec2(-60.f, 50.f));
	pHeart->SetScale(Vec2(50.f, 50.f));
	AddObject(pHeart, GROUP_TYPE::BLOCK);
	m_vecHeart.push_back(pHeart);

	// 3
	pHeart = new CBlock;
	pHeart->SetName(L"HP");
	pHeart->SetPos(Vec2(0.f, 50.f));
	pHeart->SetScale(Vec2(50.f, 50.f));
	AddObject(pHeart, GROUP_TYPE::BLOCK);
	m_vecHeart.push_back(pHeart);

	// 4
	pHeart = new CBlock;
	pHeart->SetName(L"HP");
	pHeart->SetPos(Vec2(60.f, 50.f));
	pHeart->SetScale(Vec2(50.f, 50.f));
	AddObject(pHeart, GROUP_TYPE::BLOCK);
	m_vecHeart.push_back(pHeart);

	// 5
	pHeart = new CBlock;
	pHeart->SetName(L"HP");
	pHeart->SetPos(Vec2(120.f, 50.f));
	pHeart->SetScale(Vec2(50.f, 50.f));
	AddObject(pHeart, GROUP_TYPE::BLOCK);
	m_vecHeart.push_back(pHeart);




	//테스트용 깃발 추가
	/*CObject* pFlag2 = new CFlag;
	pFlag2->SetName(L"Flag");
	pFlag2->SetPos(Vec2(600.f, 540.f));
	pFlag2->SetScale(Vec2(150.f, 150.f));
	AddObject(pFlag2, GROUP_TYPE::FLAG);*/


	// 충돌 지정
	// Player 그룹과 Monster 그룹 간의 충돌 지정
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::MONSTER);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::PROJ_PLAYER);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::GROUND);

	// 캐릭터와 충돌
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::BLOCK);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::COIN);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::FLAG);

	// 캐릭터와 충돌
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::OTHER_PLAYER, GROUP_TYPE::BLOCK);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::OTHER_PLAYER, GROUP_TYPE::COIN);
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::OTHER_PLAYER, GROUP_TYPE::FLAG);

	// 몬스터와 코인이 충돌하면 코인 삭제 06/13
	CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::COIN);

	//CCollisionMgr::GetInst()->CheckGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::PROJ_PLAYER);

	// 카메라 LookAt 지정
	//CCamera::GetInst()->SetLookAt(vResolution / 2.f);
	start();
}

void CScene_Start::Exit()
{
	m_csBgm->Stop();
	CCore::GetInst()->m_iHP = ((CPlayer*)GetPlayer())->GetHP();
	DeleteAll();

	// 기존 해당 씬에서 지정해 놨던 충돌 그룹을 해제해 놔야 함
	CCollisionMgr::GetInst()->Reset();

	// 게임 종료시 네트워크도 종료
	net.sendExit();
	net.Exit();
}

void CScene_Start::CreateForce()
{
	m_vForcePos = CCamera::GetInst()->GetRealPos(MOUSE_POS);
}
