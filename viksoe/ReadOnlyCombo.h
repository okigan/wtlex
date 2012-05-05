#if !defined(AFX_READONLYCOMBO_H__20030420_4EB4_924C_EF20_0080AD509054__INCLUDED_)
#define AFX_READONLYCOMBO_H__20030420_4EB4_924C_EF20_0080AD509054__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// ReadOnly ComboBox control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2003 Bjarke viksoe.
//
// For this control to work, you must add the
//   REFLECT_NOTIFICATIONS()
// macro to the parent's message map.
//
// NOTE: It would probably be a smoother solution to subclass
//       the parent for CBN_SELCHANGE interception.
//       This way we could avoid the need for reflection and
//       special CBN_SELCHANGE handling.
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __cplusplus
  #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
  #error ReadOnlyCombo.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
  #error ReadOnlyCombo.h requires atlctrls.h to be included first
#endif


template< class T, class TBase = CComboBox, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CReadOnlyComboImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   CEdit m_ctrlEdit;
   BOOL m_bReadOnly;
   int m_nPrevSelect;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd == NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   int SetCurSel(int nSelect)
   {
      m_nPrevSelect = nSelect;
      return TBase::SetCurSel(nSelect);
   }

   void SetReadOnly(BOOL bReadOnly)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      m_bReadOnly = bReadOnly;
      if( m_ctrlEdit.IsWindow() ) {
         EnableWindow(!bReadOnly);
         m_ctrlEdit.EnableWindow(TRUE);
         m_ctrlEdit.SetReadOnly(bReadOnly);
      }
      m_nPrevSelect = GetCurSel();
      Invalidate();
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      m_bReadOnly = FALSE;
      m_nPrevSelect = -1;
      if( GetWindow(GW_CHILD) ) m_ctrlEdit = GetWindow(GW_CHILD);
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CReadOnlyComboImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(OCM_CTLCOLOREDIT, OnCtlColor)
      MESSAGE_HANDLER(WM_CTLCOLORMSGBOX, OnCtlColor)
      MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColor)
      MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor)
      REFLECTED_COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnSelChange)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }

   LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      if( !m_bReadOnly) return lRes;
      CDCHandle dc( (HDC) wParam );
      dc.SetBkMode(TRANSPARENT);
      dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
      return (LRESULT) ::GetSysColorBrush(COLOR_BTNFACE);
   }

   LRESULT OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
   {
      if( !m_bReadOnly ) {
         bHandled = FALSE;
         return 0;
      }
      if( m_nPrevSelect != GetCurSel() ) {
         SetCurSel(m_nPrevSelect);
         ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM) m_hWnd);
      }
      return 0;
   }
};


class CReadOnlyComboCtrl : public CReadOnlyComboImpl<CReadOnlyComboCtrl>
{
public:
   DECLARE_WND_CLASS(_T("WTL_ReadOnlyCombo"))
};


#endif // !defined(AFX_READONLYCOMBO_H__20030420_4EB4_924C_EF20_0080AD509054__INCLUDED_)

