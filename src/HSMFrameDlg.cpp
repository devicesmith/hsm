
// HSMFrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HSMFrame.h"
#include "HSMFrameDlg.h"
#include "afxdialogex.h"
#include "VersionNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHSMFrameDlg dialog


// Main state variables.
struct CHSMFrameDlg::hsm_frame_state hsmFrameMachine;
struct CHSMFrameDlg::hsm_frame_event hsmFrameEventPool[10];
struct hsm_event* hsmFrameEventQueue[10];


CHSMFrameDlg::CHSMFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHSMFrameDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHSMFrameDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_TRANSITIONS, m_TransitionList);
}

BEGIN_MESSAGE_MAP(CHSMFrameDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_BUTTON_START, &CHSMFrameDlg::OnBnClickedButtonStart)
  ON_BN_CLICKED(IDC_BUTTON_A, &CHSMFrameDlg::OnBnClickedButtonA)
  ON_BN_CLICKED(IDC_BUTTON_B, &CHSMFrameDlg::OnBnClickedButtonB)
  ON_BN_CLICKED(IDC_BUTTON_C, &CHSMFrameDlg::OnBnClickedButtonC)
  ON_BN_CLICKED(IDC_BUTTON_D, &CHSMFrameDlg::OnBnClickedButtonD)
  ON_BN_CLICKED(IDC_BUTTON_E, &CHSMFrameDlg::OnBnClickedButtonE)
  ON_BN_CLICKED(IDC_BUTTON_F, &CHSMFrameDlg::OnBnClickedButtonF)
  ON_BN_CLICKED(IDC_BUTTON_G, &CHSMFrameDlg::OnBnClickedButtonG)
  ON_BN_CLICKED(IDC_BUTTON_H, &CHSMFrameDlg::OnBnClickedButtonH)
  ON_BN_CLICKED(IDC_BUTTON_I, &CHSMFrameDlg::OnBnClickedButtonI)
  ON_BN_CLICKED(IDC_BUTTON_J, &CHSMFrameDlg::OnBnClickedButtonJ)
  ON_BN_CLICKED(IDC_BUTTON_K, &CHSMFrameDlg::OnBnClickedButtonK)
  ON_BN_CLICKED(IDC_BUTTON_L, &CHSMFrameDlg::OnBnClickedButtonL)
  ON_BN_CLICKED(IDC_BUTTON_STEP, &CHSMFrameDlg::OnBnClickedButtonStep)
END_MESSAGE_MAP()


// CHSMFrameDlg message handlers

BOOL CHSMFrameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

  // Add version to title bar of dialog
  TCHAR dialogName[255];
  GetWindowText(dialogName, sizeof(dialogName));
  CString newName;
  newName.Format(_T("%s v%s"), dialogName, _T(STRPRODUCTVER));
  SetWindowText(newName);

  // Setup state machine
  InitStateMachine((CHSMFrameDlg::hsm_frame_state*)&hsmFrameMachine, this);

  SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHSMFrameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHSMFrameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/////////////////////////////////////////////////
// State Machine States
/////////////////////////////////////////////////
#define CAST_DIALOG(a) (((struct hsm_frame_state*)(a))->pDlg)
// CAST_DIALOG(self)->WriteDisplay(_T("blah"));
#define CAST_FRAME_STATE(a) ((struct hsm_frame_state*)(a))
// CAST_FRAME_STATE(self)->pDlg->WriteDisplay(_T("blah"));

void CHSMFrameDlg::InitStateMachine(hsm_frame_state* pStateMachine, CHSMFrameDlg* pDlg)
{
  hsmEventPoolInit((hsm_event*)&hsmFrameEventPool, sizeof(hsmFrameEventPool));
  hsmEventQueueInit(hsmFrameEventQueue, sizeof(hsmFrameEventQueue));
  hsmInitStateMachine();
  pStateMachine->pDlg = pDlg;
}

hsm_state_result CHSMFrameDlg::RootState(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  return STATE_HANDLED;
}

hsm_state_result CHSMFrameDlg::s0(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S0:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S0:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_D:
      self->stateHandler = s211;
      return STATE_CHANGED;
    case FRAME_SIG_E:
      self->stateHandler = s21;
      return STATE_CHANGED;
    case FRAME_SIG_G:
      self->stateHandler = s11;
      return STATE_CHANGED;
    case FRAME_SIG_I:
      self->stateHandler = s1;
      return STATE_CHANGED;
    case FRAME_TRANSITION_SIG:
      self->stateHandler = state0;
      return STATE_CHANGED;
  }
  self->stateHandler = RootState;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::s1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S1:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S1:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_I:
      self->stateHandler = s2;
      return STATE_CHANGED;
  }
  self->stateHandler = s0;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::s11(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S11:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S11:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_J:
      self->stateHandler = s0;
      return STATE_CHANGED;
    case FRAME_SIG_H:
      self->stateHandler = s211;
      return STATE_CHANGED;
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::s12(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S12:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S12:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_K:
      self->stateHandler = s13;
      return STATE_CHANGED;
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;

}

hsm_state_result CHSMFrameDlg::s13(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S13:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S13:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_K:
      self->stateHandler = s12;
      return STATE_CHANGED;
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::s2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S2:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S2:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_G:
      self->stateHandler = s11;
      return STATE_CHANGED;
    case FRAME_SIG_I:
      self->stateHandler = s1;
      return STATE_CHANGED;
  }
  self->stateHandler = s0;
  return STATE_DO_SUPERSTATE;
}

// Initial State
hsm_state_result CHSMFrameDlg::s21(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S21:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S21:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_A:
      self->stateHandler = s211;
      return STATE_CHANGED;
    case FRAME_SIG_F:
      self->stateHandler = s22;
      return STATE_CHANGED;
  }
  self->stateHandler = s2;
  return STATE_DO_SUPERSTATE;

}

hsm_state_result CHSMFrameDlg::s211(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S211:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S211:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_A:
      return STATE_HANDLED;
    case FRAME_SIG_B:
      self->stateHandler = s21;
      return STATE_CHANGED;
    case FRAME_SIG_C:
      self->stateHandler = s2;
      return STATE_CHANGED;
  }
  self->stateHandler = s21;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::s22(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("S22:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("S22:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_SIG_L:
      self->stateHandler = s13;
      return STATE_CHANGED;
  }
  self->stateHandler = s2;
  return STATE_DO_SUPERSTATE;

}

hsm_state_result CHSMFrameDlg::state0(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("state0:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("state0:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_TRANSITION_SIG:
      self->stateHandler = state1;
      return STATE_CHANGED;
  }
  self->stateHandler = RootState;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::state1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("state1:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("state1:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_TRANSITION_SIG:
      self->stateHandler = state2;
      return STATE_CHANGED;
  }
  self->stateHandler = state0;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::state2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("state2:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("state2:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_TRANSITION_SIG:
      self->stateHandler = state3;
      return STATE_CHANGED;
  }
  self->stateHandler = state0;
  return STATE_DO_SUPERSTATE;
}

hsm_state_result CHSMFrameDlg::state3(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      CAST_DIALOG(self)->WriteDisplay(_T("state3:Entry"));
      return STATE_HANDLED;
    case HSM_SIG_EXIT:
      CAST_DIALOG(self)->WriteDisplay(_T("state3:Exit"));
      return STATE_HANDLED;
    case HSM_SIG_INITIAL:
      return STATE_HANDLED;
    case FRAME_TRANSITION_SIG:
      self->stateHandler = state1;
      return STATE_CHANGED;
  }
  self->stateHandler = state0;
  return STATE_DO_SUPERSTATE;
}


/////////////////////////////////////////////////
// Worker Members
/////////////////////////////////////////////////
void CHSMFrameDlg::WriteDisplay(CString message)
{
  int pos = m_TransitionList.GetCount();
  m_TransitionList.SetSel(pos, FALSE);
  //CString newMsg;
  //newMsg.Format(_T("%d:%s"), pos, message);
  //m_TransitionList.AddString(newMsg);
  m_TransitionList.AddString(message);
  m_TransitionList.SetCurSel(pos);
  UpdateData();
}


/////////////////////////////////////////////////
// Windows Message Handlers
/////////////////////////////////////////////////
void CHSMFrameDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: Add your message handler code here and/or call default
  hsmProcess((struct hsm_state *)&hsmFrameMachine);

  CDialogEx::OnTimer(nIDEvent);
}


void CHSMFrameDlg::OnBnClickedButtonStart()
{
  WriteDisplay(_T("Start"));
  hsmInitialState((hsm_state*)&hsmFrameMachine, s21);
  
  GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
}


void CHSMFrameDlg::OnBnClickedButtonA()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();

  if(e)
  {
    WriteDisplay(_T("A"));
    e->baseEvent.signal = FRAME_SIG_A;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonB()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("B"));
    e->baseEvent.signal = FRAME_SIG_B;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonC()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("C"));
    e->baseEvent.signal = FRAME_SIG_C;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonD()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("D"));
    e->baseEvent.signal = FRAME_SIG_D;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonE()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("E"));
    e->baseEvent.signal = FRAME_SIG_E;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonF()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("F"));
    e->baseEvent.signal = FRAME_SIG_F;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonG()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("G"));
    e->baseEvent.signal = FRAME_SIG_G;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonH()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("H"));
    e->baseEvent.signal = FRAME_SIG_H;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonI()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("I"));
    e->baseEvent.signal = FRAME_SIG_I;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonJ()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("J"));
    e->baseEvent.signal = FRAME_SIG_J;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonK()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("K"));
    e->baseEvent.signal = FRAME_SIG_K;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonL()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("L"));
    e->baseEvent.signal = FRAME_SIG_L;
    fifoPush((struct hsm_event*)e);
  }
}


void CHSMFrameDlg::OnBnClickedButtonStep()
{
  struct hsm_frame_event* e = (hsm_frame_event*)hsmEventNew();
  if(e)
  {
    WriteDisplay(_T("st"));
    e->baseEvent.signal = FRAME_TRANSITION_SIG;
    fifoPush((struct hsm_event*)e);
  }
}
