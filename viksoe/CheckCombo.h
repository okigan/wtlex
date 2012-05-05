#if !defined(AFX_CHECKCOMBO_H__20030420_CFDE_36CE_899B_0080AD509054__INCLUDED_)
#define AFX_CHECKCOMBO_H__20030420_CFDE_36CE_899B_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CheckComboCtrl - ComboBox with checkboxes
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2003 Bjarke Viksoe.
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
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

#ifndef __cplusplus
  #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
  #error CheckCombo.h requires atlctrls.h to be included first
#endif

#if (_WIN32_IE < 0x0400)
  #error CheckCombo.h requires _WIN32_IE >= 0x0400
#endif


/////////////////////////////////////////////////////////////////////////////
// 

#define CCBS_EX_CLOSEIMMEDIATELY  0x00000001
#define CCBS_EX_ADJUSTEDITHEIGHT  0x00000002


template< class T, class TBase = CComboBox, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CCheckComboImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public COwnerDraw< T >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   CCheckComboImpl() :
      m_ctrlList(this, 1),
      m_dwExtStyle(0UL)
   {
   }

   CContainedWindowT<CListBox> m_ctrlList;
   DWORD m_dwExtStyle;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   DWORD SetExtendedComboStyle(DWORD dwStyle)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      DWORD dwOldStyle = m_dwExtStyle;
      m_dwExtStyle = dwStyle;
      _Init();
      return dwOldStyle;
   }

   int AddString(LPCTSTR pstrText, BOOL bCheck = FALSE)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      int iRes = TBase::AddString(pstrText);
      if( iRes >= 0 ) TBase::SetItemData(iRes, (LPARAM) bCheck);
      return iRes;
   }
   BOOL SetCheckState(int iIndex, BOOL bCheck)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      BOOL bRet = (BOOL) TBase::SetItemData(iIndex, (LPARAM) bCheck);
      Invalidate();
      return bRet;
   }
   BOOL GetCheckState(int iIndex)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      return (BOOL) TBase::GetItemData(iIndex);
   }

   // Deprecated methods

   int GetCurSel() const
   {
      ATLASSERT(false);
      return -1;
   }
   int SetCurSel(int /*iIndex*/)
   {
      ATLASSERT(false);
      return -1;
   }
   LPARAM GetItemData(int /*iIndex*/) const
   {
      ATLASSERT(false);
      return -1;
   }
   DWORD_PTR SetItemData(int /*iIndex*/, DWORD_PTR /*dwItemData*/)
   {
      ATLASSERT(false);
      return -1;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      // Need to set these in resource editor
      ATLASSERT((GetStyle() & 0x0F)==CBS_DROPDOWNLIST);
      ATLASSERT((GetStyle() & CBS_OWNERDRAWFIXED)!=0);
      ATLASSERT((GetStyle() & CBS_HASSTRINGS)!=0);

      if( (m_dwExtStyle & CCBS_EX_ADJUSTEDITHEIGHT) != 0 ) {
         MEASUREITEMSTRUCT mis;
         MeasureItem(&mis);
         SetItemHeight(-1, mis.itemHeight);
      }

      Invalidate();
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CCheckComboImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColorListBox)      
      MESSAGE_HANDLER(WM_CHAR, OnChar)
      CHAIN_MSG_MAP_ALT( COwnerDraw< T >, 1 )
   ALT_MSG_MAP(1)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnListLButtonDown)
      MESSAGE_HANDLER(WM_CHAR, OnListChar)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }
   LRESULT OnCtlColorListBox(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      if( !m_ctrlList.IsWindow() ) m_ctrlList.SubclassWindow((HWND)lParam);
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( wParam == _T(' ') ) {
         if( !m_ctrlList.IsWindow() || !m_ctrlList.IsWindowVisible() ) {
            ShowDropDown(TRUE);
            return 0;
         }
      }
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnListLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      int iHeight = GetItemHeight(0);
      if( pt.x > iHeight ) {
         bHandled = FALSE;
         return 0;
      }
      int iTopIndex = GetTopIndex();
      int iIndex = (pt.y / iHeight) + iTopIndex;
      SetCheckState(iIndex, !GetCheckState(iIndex));
      m_ctrlList.Invalidate();
      if( (m_dwExtStyle & CCBS_EX_CLOSEIMMEDIATELY) != 0 ) bHandled = FALSE;
      return 0;
   }
   LRESULT OnListChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( wParam == _T(' ') ) {
         int iIndex = m_ctrlList.GetCaretIndex();
         SetCheckState(iIndex, !GetCheckState(iIndex));
         m_ctrlList.Invalidate();
         if( (m_dwExtStyle & CCBS_EX_CLOSEIMMEDIATELY) != 0 ) ShowDropDown(FALSE);
         return 0;
      }
      bHandled = FALSE;
      return 0;
   }

   // COwnerDraw

   void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
   {
      CClientDC dc(m_hWnd);
      TEXTMETRIC tm;
      dc.GetTextMetrics(&tm);
      lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;
   }
   void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
   {
      CDCHandle dc = lpDrawItemStruct->hDC;
      RECT rcItem = lpDrawItemStruct->rcItem;
      BOOL bChecked = (BOOL) lpDrawItemStruct->itemData;
      bool bFocus = (lpDrawItemStruct->itemState & ODS_FOCUS) != 0;
      bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;

      // Never paint selected color for combobox itself (edit part)
      if( (lpDrawItemStruct->itemState & ODS_COMBOBOXEDIT) != 0 ) {
         TCHAR szTitle[400] = { 0 };
         int nLen = 0;
         for( int i = 0; i < GetCount(); i++ ) {
            if( !GetCheckState(i) ) continue;
            // Get item text
            TCHAR szItem[64];
            int nItemLen = GetLBTextLen(i);
            ATLASSERT(nItemLen<sizeof(szItem)/sizeof(TCHAR));
            if( nItemLen >= sizeof(szItem)/sizeof(TCHAR) ) break;
            GetLBText(i, szItem);
            // Append if there's room...
            nLen += nItemLen + (nLen > 0 ? 3 : 0);
            if( nLen >= sizeof(szTitle)/sizeof(TCHAR) ) break;
            if( nLen > nItemLen ) ::lstrcat(szTitle, _T(", "));
            ::lstrcat(szTitle, szItem);
         }
         dc.SetBkMode(TRANSPARENT);
         dc.FillRect(&rcItem, ::GetSysColorBrush(bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW));
         dc.SetTextColor(::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
         dc.DrawText(szTitle, -1, &rcItem, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
         return;
      }

      if( lpDrawItemStruct->itemID == -1 ) return;

      // Erase background
      dc.SetBkMode(TRANSPARENT);
      dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
      dc.FillRect(&rcItem, ::GetSysColorBrush(COLOR_WINDOW));
      rcItem.left += 2;

      int cx = rcItem.bottom - rcItem.top;
      RECT rcButton = { rcItem.left, rcItem.top + 1, rcItem.left + cx, rcItem.bottom - 1 };
      dc.DrawFrameControl(&rcButton, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT | (bChecked ? DFCS_CHECKED : 0));
      rcItem.left += cx + 2;

      TCHAR szItem[64] = { 0 };
      ATLASSERT(GetLBTextLen(lpDrawItemStruct->itemID)<64);
      GetLBText(lpDrawItemStruct->itemID, szItem);
      dc.DrawText(szItem, -1, &rcItem, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

      if( bFocus ) {
         RECT rcFocus = rcItem;
         dc.DrawText(szItem, -1, &rcFocus, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_CALCRECT);
         rcFocus.bottom += 1;
         ::InflateRect(&rcFocus, 2, 0);
         dc.DrawFocusRect(&rcFocus);
      }
   }
};

class CCheckComboCtrl : public CCheckComboImpl<CCheckComboCtrl, CComboBox, CWinTraitsOR<CBS_OWNERDRAWVARIABLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS> >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_CheckedComboBox"), GetWndClassName())  
};


#endif // !defined(AFX_CHECKCOMBO_H__20030420_CFDE_36CE_899B_0080AD509054__INCLUDED_)

