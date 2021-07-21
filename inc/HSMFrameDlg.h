
// HSMFrameDlg.h : header file
//

#pragma once

#include "hsmframework.h"
#include "afxwin.h"

// CHSMFrameDlg dialog
class CHSMFrameDlg : public CDialogEx
{
// Construction
public:
	CHSMFrameDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HSMFRAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


/////////////////////////////////////////////////////////////////
// State Machine details
/////////////////////////////////////////////////////////////////
public:
  // Extend the base event.
  struct hsm_frame_event
  {
    struct hsm_event baseEvent;
    int extraEventInfo;
  };

  // Extend the base signal.
  enum hsm_frame_signal
  {
    FRAME_SIG_START = HSM_SIG_USER,
    FRAME_SIG_A,
    FRAME_SIG_B,
    FRAME_SIG_C,
    FRAME_SIG_D,
    FRAME_SIG_E,
    FRAME_SIG_F,
    FRAME_SIG_G,
    FRAME_SIG_H,
    FRAME_SIG_I,
    FRAME_SIG_J,
    FRAME_SIG_K,
    FRAME_SIG_L,
    FRAME_TRANSITION_SIG
  };

  // Extend the base state variable.
  struct hsm_frame_state
  {
    struct hsm_state base;
    CHSMFrameDlg* pDlg;
    int extraStateInfo;
  };

  // Custon Initialization 
  void InitStateMachine(hsm_frame_state* pStateMachine, CHSMFrameDlg* pDlg);

  // Add states
  static hsm_state_result RootState(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s0(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s1(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s11(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s12(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s13(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s2(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s21(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s211(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result s22(struct hsm_state* self, struct hsm_event const * hsmEvent);

  static hsm_state_result state0(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result state1(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result state2(struct hsm_state* self, struct hsm_event const * hsmEvent);
  static hsm_state_result state3(struct hsm_state* self, struct hsm_event const * hsmEvent);

/////////////////////////////////////////////////////////////////

// Worker Members

  void WriteDisplay(CString message);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnBnClickedButtonStart();
  afx_msg void OnBnClickedButtonA();
  CListBox m_TransitionList;
  afx_msg void OnBnClickedButtonB();
  afx_msg void OnBnClickedButtonC();
  afx_msg void OnBnClickedButtonD();
  afx_msg void OnBnClickedButtonE();
  afx_msg void OnBnClickedButtonF();
  afx_msg void OnBnClickedButtonG();
  afx_msg void OnBnClickedButtonH();
  afx_msg void OnBnClickedButtonI();
  afx_msg void OnBnClickedButtonJ();
  afx_msg void OnBnClickedButtonK();
  afx_msg void OnBnClickedButtonL();
  afx_msg void OnBnClickedButtonStep();
};
