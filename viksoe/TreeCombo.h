#if !defined(AFX_TREECOMBO_H__20051115_86FB_C577_C1A0_0080AD509054__INCLUDED_)
#define AFX_TREECOMBO_H__20051115_86FB_C577_C1A0_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// TreeCombo - ComboBox & CTreeViewCtrl hybrid
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2005 Bjarke Viksoe.
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
  #error TreeCombo.h requires atlctrls.h to be included first
#endif

#if WINVER < 0x0501
  // Requiring this WINVER version is a bug in the platform SDK since 
  // GetComboBoxInfo is reported to be available on Win98 and WinNT4...
  #error Needs WINVER 0x0501 since GetComboBoxInfo must be declared
#endif


template< class T, class TBase = CComboBox, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CTreeComboImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   DWORD m_dwTreeStyle;
   CContainedWindowT<CTreeViewCtrl> m_ctrlTree;
   HTREEITEM m_hPrevItem;

   CTreeComboImpl() : 
      m_dwTreeStyle(TVS_HASLINES|TVS_HASBUTTONS|TVS_SHOWSELALWAYS)
   {
   }

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }
   DWORD SetTreeStyle(DWORD dwStyle)
   {
      ATLASSERT(!m_ctrlTree.IsWindow());   // Before control is created, please!
      DWORD dwOldStyle = m_dwTreeStyle;
      m_dwTreeStyle = dwStyle;
      return dwOldStyle;
   }

   CTreeViewCtrl GetTreeCtrl() const
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree;
   }
   void ResetContent()
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      m_ctrlTree.DeleteAllItems();
      Invalidate();
   }
   void SelectItem(HTREEITEM hItem)
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      m_ctrlTree.SelectItem(hItem);
      Invalidate();
   }
   HTREEITEM GetSelectedItem() const
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.GetSelectedItem();
   }
   HTREEITEM InsertItem(LPTVINSERTSTRUCT tvi)
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.InsertItem(tvi);
   }
   HTREEITEM InsertItem(HTREEITEM hParent, LPCTSTR pstrText, int iImage = -1, int iSelImage = -1, LPARAM lParam = 0, HTREEITEM hInsertAfter = TVI_LAST)
   {
      ATLASSERT(!::IsBadStringPtr(pstrText,-1));
      TVINSERTSTRUCT tvis = { 0 };
      tvis.hParent = hParent;
      tvis.hInsertAfter = hInsertAfter;
      tvis.item.pszText = const_cast<LPTSTR>(pstrText);
      tvis.item.lParam = lParam;
      tvis.item.iImage = iImage;
      tvis.item.iSelectedImage = iSelImage;
      tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
      if( iImage >= 0 ) tvis.item.mask |= TVIF_IMAGE;
      if( iSelImage >= 0 ) tvis.item.mask |= TVIF_SELECTEDIMAGE;
      return InsertItem(&tvis);
   }
   BOOL DeleteItem(HTREEITEM hItem)
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.DeleteItem(hItem);
   }
   BOOL ExpandItem(HTREEITEM hItem)
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.Expand(hItem);
   }
   HTREEITEM GetRootItem() const
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.GetRootItem();
   }
   CImageList SetImageList(HIMAGELIST hImages, int iType = TVSIL_NORMAL)
   {
      ATLASSERT(m_ctrlTree.IsWindow());
      return m_ctrlTree.SetImageList(hImages, iType);
   }

   // Unsupported methods

   void InsertString(int /*iIndex*/, LPCTSTR /*pstrText*/)
   {
      ATLASSERT(false);
   }
   void AddString(LPCTSTR /*pstrText*/)
   {
      ATLASSERT(false);
   }
   void DeleteString(LPCTSTR /*pstrText*/)
   {
      ATLASSERT(false);
   }
   void SetCurSel(int /*iIndex*/)
   {
      ATLASSERT(false);
   }
   int GetCurSel() const
   {
      ATLASSERT(false);
      return -1;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));

      // Need to set this in resource editor
      ATLASSERT(GetStyle() & CBS_OWNERDRAWFIXED);
      ATLASSERT((GetStyle() & 0x0F)==CBS_DROPDOWNLIST);

      m_ctrlTree.Create(this, 1, NULL, &rcDefault, NULL, WS_POPUP|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|m_dwTreeStyle, WS_EX_TOOLWINDOW);
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CTreeComboImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
      MESSAGE_HANDLER(WM_APP + 1, OnTreeHandleInput)
      REFLECTED_COMMAND_CODE_HANDLER(CBN_DROPDOWN, OnDropDown)
   ALT_MSG_MAP(1)
      MESSAGE_HANDLER(WM_CHAR, OnTreeChar)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnTreeKillFocus)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnTreeButtonUp)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_ctrlTree.DestroyWindow();
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnTreeHandleInput(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      ReleaseCapture();  // Force list to close prematurely!
      COMBOBOXINFO CBInfo = { 0 };
      CBInfo.cbSize = sizeof(CBInfo);
      GetComboBoxInfo(&CBInfo);
      RECT rcList = { 0 };
      ::GetWindowRect(CBInfo.hwndList, &rcList);
      ::ShowWindow(CBInfo.hwndList, SW_HIDE);
      m_ctrlTree.SetWindowPos(HWND_TOP, &rcList, SWP_SHOWWINDOW);
      m_ctrlTree.EnsureVisible(m_ctrlTree.GetSelectedItem());
      // BUG: We need to set focus to the tree because we want click/scroll/key events,
      //      but this steals focus from the top parent. Ideally we should hook the parent
      //      and prevent WM_NCACTIVATE message.
      m_ctrlTree.SetFocus();
      return 0;
   }

   LRESULT OnDropDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
   {
      // List is about to be displayed; let's allow the COMBOBOX to finish processing
      // these events and them we'll show our own control instead.
      // BUG: The CBN_CLOSEUP gets screwed by this.
      m_hPrevItem = m_ctrlTree.GetSelectedItem();
      PostMessage(WM_APP + 1);
      bHandled = FALSE;
      return 0;
   }

   // Tree control

   LRESULT OnTreeKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      // Simulate notifications
      if( m_ctrlTree.GetSelectedItem() != m_hPrevItem ) ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM) m_hWnd);
      // Hide our tree again...
      m_ctrlTree.ShowWindow(SW_HIDE);
      bHandled = FALSE;
      return 0;
   }
   LRESULT OnTreeButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = m_ctrlTree.DefWindowProc();
      // If we've clicked on a tree-item we just hide the tree again...
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      TVHITTESTINFO hti = { 0 };
      hti.pt = pt;
      m_ctrlTree.HitTest(&hti);
      if( (hti.flags & TVHT_ONITEM) != 0 ) SetFocus();
      return lRes;
   }
   LRESULT OnTreeChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( wParam == VK_RETURN ) {
         SetFocus();
         return 0;
      }
      if( wParam == VK_ESCAPE ) {
         m_ctrlTree.SelectItem(m_hPrevItem);
         SetFocus();
         return 0;
      }
      bHandled = FALSE;
      return 0;
   }

   // Owner draw

   LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
      ::SetBkColor(lpDIS->hDC, ::GetSysColor(COLOR_WINDOW));
      ::SetTextColor(lpDIS->hDC, ::GetSysColor(COLOR_WINDOWTEXT));
      CImageList Image = m_ctrlTree.CreateDragImage(m_ctrlTree.GetSelectedItem());
      if( Image.IsNull() ) return 1;
      POINT pt = { lpDIS->rcItem.left, lpDIS->rcItem.top };
      Image.Draw(lpDIS->hDC, 0, pt, ILD_NORMAL);
      Image.Destroy();
      if( (lpDIS->itemState & ODS_FOCUS) != 0 ) ::DrawFocusRect(lpDIS->hDC, &lpDIS->rcItem);
      return 1;
   }
};

class CTreeComboCtrl : public CTreeComboImpl<CTreeComboCtrl, CComboBox, CWinTraitsOR<CBS_OWNERDRAWVARIABLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS> >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_TreeComboBox"), GetWndClassName())  
};


#endif // !defined(AFX_TREECOMBO_H__20051115_86FB_C577_C1A0_0080AD509054__INCLUDED_)

