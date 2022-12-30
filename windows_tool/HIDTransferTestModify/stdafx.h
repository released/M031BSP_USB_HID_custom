// stdafx.h : �i�b�����Y�ɤ��]�t�зǪ��t�� Include �ɡA
// �άO�g�`�ϥΫo�ܤ��ܧ�
// �M�ױM�� Include �ɮ�
//

#pragma once

// �p�G�z�������u����������x�A�Эק�U�C�w�q�C
// �Ѧ� MSDN ���o���P���x�����Ȫ��̷s��T�C
#ifndef WINVER              // ���\�ϥ� Windows XP (�t) �H�᪩�����S�w�\��C
#define WINVER 0x0501       // �N���ܧ󬰰w�� Windows ��L�������A���ȡC
#endif

#ifndef _WIN32_WINNT        // ���\�ϥ� Windows XP (�t) �H�᪩�����S�w�\��C
#define _WIN32_WINNT 0x0501 // �N���ܧ󬰰w�� Windows ��L�������A���ȡC
#endif

#ifndef _WIN32_WINDOWS      // ���\�ϥ� Windows 98 (�t) �H�᪩�����S�w�\��C
#define _WIN32_WINDOWS 0x0410 // �N���ܧ󬰰w�� Windows Me (�t) �H�᪩�����A���ȡC
#endif

#ifndef _WIN32_IE           // ���\�ϥ� IE 6.0 (�t) �H�᪩�����S�w�\��C
#define _WIN32_IE 0x0600    // �N���ܧ󬰰w�� IE ��L�������A���ȡC
#endif

#define WIN32_LEAN_AND_MEAN     // �q Windows ���Y�ư����`�ϥΪ�����
#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // ���T�w�q������ CString �غc�禡

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // �q Windows ���Y�ư����`�ϥΪ�����
#endif

// #include <afx.h>
// #include <afxwin.h>         // MFC �֤߻P�зǤ���
// #include <afxext.h>         // MFC �X�R�\��
// #ifndef _AFX_NO_OLE_SUPPORT
// #include <afxdtctl.h>       // MFC �䴩�� Internet Explorer 4 �q�α��
// #endif
// #ifndef _AFX_NO_AFXCMN_SUPPORT
// #include <afxcmn.h>         // MFC �䴩�� Windows �q�α��
// #endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>



// TODO: �b���Ѧұz���{���һݭn����L���Y
