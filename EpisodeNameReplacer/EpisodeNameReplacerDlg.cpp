
// EpisodeNameReplacerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EpisodeNameReplacer.h"
#include "EpisodeNameReplacerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDX_COL_EPNUMBER	0
#define IDX_COL_EPTITLE		1

// CEpisodeNameReplacerDlg dialog

CEpisodeNameReplacerDlg::CEpisodeNameReplacerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEpisodeNameReplacerDlg::IDD, pParent)
{
}

void CEpisodeNameReplacerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_EPISODES, m_lstEpisodes);
	DDX_Control(pDX, IDC_COMBO_SEASONS, m_cmbSeasons);
}

BEGIN_MESSAGE_MAP(CEpisodeNameReplacerDlg, CDialog)
	ON_BN_CLICKED(IDC_GETEPISODES, OnBnClickedGetepisodes)
	ON_BN_CLICKED(IDC_GETSEASONS, OnBnClickedGetseasons)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)

	ON_EN_CHANGE(IDC_SERIES, OnEnChangeSeries)
	ON_EN_CHANGE(IDC_SEASONFOLDER, OnEnChangeSeasonfolder)
	ON_BN_CLICKED(IDC_REPLACE, OnBnClickedReplace)
END_MESSAGE_MAP()

// CEpisodeNameReplacerDlg message handlers

BOOL CEpisodeNameReplacerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	m_lstEpisodes.GetClientRect(rect);

	int nWidth = rect.Width() - GetSystemMetrics(SM_CXVSCROLL);
	m_lstEpisodes.InsertColumn(IDX_COL_EPNUMBER, _T("#"), LVCFMT_LEFT, (int)(nWidth * 0.05));
	m_lstEpisodes.InsertColumn(IDX_COL_EPTITLE, _T("Episode Title"), LVCFMT_LEFT, (int)(nWidth * 0.95));

	EnableControls();
	return TRUE;
}

void CEpisodeNameReplacerDlg::OnBnClickedGetepisodes()
{
	CString rSeason;
	CString rData;

	m_lstEpisodes.DeleteAllItems();

	m_xmlParser.Go_to_Root();
	m_xmlParser.Go_to_Descendant(_T("Episode"));

	GetDlgItemText(IDC_COMBO_SEASONS, m_rSeasonNumber);

	do
	{
		m_xmlParser.Go_to_Child(_T("Combined_season"));
		rData = m_xmlParser.Get_TextValue();

		if (rData != m_rSeasonNumber)
		{
			m_xmlParser.Go_to_Parent();
			continue;
		}

		m_xmlParser.Go_to_Parent();
		m_xmlParser.Go_to_Child(_T("EpisodeNumber"));
		rData = m_xmlParser.Get_TextValue();

		int nItem = m_lstEpisodes.InsertItem(m_lstEpisodes.GetItemCount(), rData);

		m_xmlParser.Go_to_Parent();
		m_xmlParser.Go_to_Child(_T("EpisodeName"));
		rData = m_xmlParser.Get_TextValue();

		m_lstEpisodes.SetItemText(nItem, IDX_COL_EPTITLE, rData);
		m_xmlParser.Go_to_Parent();

	} while (m_xmlParser.Go_Forward());

	EnableControls();
}

BOOL CEpisodeNameReplacerDlg::GetXMLData(CString rRequest, CString& rXMLData)
{
	CStringA rXMLDataA;
	BOOL bRet = DownloadFile(_T("www.thetvdb.com"), rRequest, AfxGetApp()->m_pszAppName, rXMLDataA);

	rXMLData = rXMLDataA;
	return bRet;
}

void CEpisodeNameReplacerDlg::OnBnClickedGetseasons()
{
	CString rData;
	CString rRequest;
	CString rXMLData;

	CWaitCursor wait;
	m_cmbSeasons.ResetContent();

	CString rSeries;
	GetDlgItemText(IDC_SERIES, rSeries);

	m_rSeriesName = rSeries;
	rSeries.Replace(_T(" "), _T("_"));

	rRequest.Format(_T("/api/GetSeries.php?seriesname=%s"), rSeries);
	GetXMLData(rRequest, rXMLData);

	m_xmlParser.Load_XML_From_Buffer(rXMLData);
	m_xmlParser.Go_to_Root();
	m_xmlParser.Go_to_Descendant(_T("seriesid"));

	rData = m_xmlParser.Get_TextValue();

	rRequest.Format(_T("/api/24866296BDF8634D/series/%s/all/"), rData);
	GetXMLData(rRequest, rXMLData);

	m_xmlParser.Load_XML_From_Buffer(rXMLData);
	m_xmlParser.Go_to_Descendant(_T("Episode"));

	CString rLastSeasonAdded;

	do
	{
		m_xmlParser.Go_to_Child(_T("Combined_season"));
		rData = m_xmlParser.Get_TextValue();

		if (rData == rLastSeasonAdded)
		{
			m_xmlParser.Go_to_Parent();
			continue;
		}

		rLastSeasonAdded = rData;

		if (m_cmbSeasons.FindString(0, rData) != -1)
		{
			m_xmlParser.Go_to_Parent();
			continue;
		}

		m_cmbSeasons.InsertString(m_cmbSeasons.GetCount(), rData);
		m_xmlParser.Go_to_Parent();

	} while (m_xmlParser.Go_Forward());

	if (m_cmbSeasons.GetCount() > 0)
		m_cmbSeasons.SetCurSel(0);

	EnableControls();
}


void CEpisodeNameReplacerDlg::OnBnClickedBrowse()
{
	CFolderPickerDialog dlg;
	if (dlg.DoModal() == IDCANCEL)
		return;

	m_rFolderPath = dlg.GetFolderPath();
	SetDlgItemText(IDC_SEASONFOLDER, m_rFolderPath);

	EnableControls();
}

void CEpisodeNameReplacerDlg::EnableControls()
{
	CString rTxt;
	GetDlgItemText(IDC_SERIES, rTxt);

	BOOL bEnable = rTxt.GetLength() > 0;
	EnableDlgItem(IDC_GETSEASONS, bEnable);

	bEnable = m_cmbSeasons.GetCount() > 0;
	EnableDlgItem(IDC_COMBO_SEASONS, bEnable);
	EnableDlgItem(IDC_GETEPISODES, bEnable);

	bEnable = m_lstEpisodes.GetItemCount() > 0;
	EnableDlgItem(IDC_SEASONFOLDER, bEnable);
	EnableDlgItem(IDC_BROWSE, bEnable);

	GetDlgItemText(IDC_SEASONFOLDER, rTxt);
	bEnable = bEnable && !rTxt.IsEmpty();

	EnableDlgItem(IDC_REPLACE, bEnable);
}

void CEpisodeNameReplacerDlg::EnableDlgItem(UINT nID, BOOL bEnable)
{
	::EnableWindow(::GetDlgItem(GetSafeHwnd(), nID), bEnable);
}

void CEpisodeNameReplacerDlg::OnEnChangeSeries()
{
	EnableControls();
}

void CEpisodeNameReplacerDlg::OnEnChangeSeasonfolder()
{
	EnableControls();
}

CString CEpisodeNameReplacerDlg::GetEpisodeTitle(int nEpNumber)
{
	CString rEpTitle;
	
	CString rEpNumber;
	rEpNumber.Format(_T("%d"), nEpNumber);

	int nItemCount = m_lstEpisodes.GetItemCount();

	for (int cnt = 0; cnt < nItemCount; cnt++)
	{
		if (rEpNumber != m_lstEpisodes.GetItemText(cnt, IDX_COL_EPNUMBER))
			continue;

		rEpTitle = m_lstEpisodes.GetItemText(cnt, IDX_COL_EPTITLE);
		return rEpTitle;
	}

	return rEpTitle;
}

void CEpisodeNameReplacerDlg::OnBnClickedReplace()
{
	CString rSeasonFormat;
	rSeasonFormat.Format(_T("S%.2d"), _ttoi(m_rSeasonNumber));

	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	CString rFolderSearchStr = m_rFolderPath + _T("\\*");
	hFind = FindFirstFile(rFolderSearchStr, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) 
	{
		CString rTxt;
		rTxt.Format(_T("Error %d occured"), GetLastError());
		AfxMessageBox(rTxt, MB_ICONERROR | MB_OK);

		return;
	} 

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		CString rFilename(ffd.cFileName);
		
		int nPos = rFilename.Find(rSeasonFormat);
		if (nPos == -1)
			continue;

		CString rEpNumber = rFilename.Mid(nPos + 4, 2);
		
		nPos = rFilename.ReverseFind(_T('.'));

		ASSERT(nPos > -1);
		CString rExt = rFilename.Mid(nPos, rFilename.GetLength() - nPos);

		int nEpNumber = _ttoi(rEpNumber);

		CString rOldFilename;
		CString rNewFilename;
		CString rEpTitle = GetEpisodeTitle(nEpNumber);

		rEpTitle.Replace(_T(":"), _T(""));
		rEpTitle.Replace(_T("?"), _T(""));
		rEpTitle.Replace(_T("/"), _T(""));
		rEpTitle.Replace(_T("\\"), _T(""));
		rEpTitle.Replace(_T("*"), _T(""));
		rEpTitle.Replace(_T("<"), _T(""));
		rEpTitle.Replace(_T(">"), _T(""));
		rEpTitle.Replace(_T("|"), _T(""));
		rEpTitle.Replace(_T("\""), _T(""));

		rOldFilename.Format(_T("%s\\%s"), m_rFolderPath, rFilename);
		rNewFilename.Format(_T("%s\\%s - %sE%.2d - %s%s"), m_rFolderPath, m_rSeriesName, rSeasonFormat, nEpNumber, rEpTitle, rExt);
		_trename(rOldFilename, rNewFilename);
	}
	while (FindNextFile(hFind, &ffd) != 0);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) 
	{
		CString rTxt;
		rTxt.Format(_T("Error %d occured"), dwError);
		AfxMessageBox(rTxt, MB_ICONERROR | MB_OK);

		FindClose(hFind);
		return;
	}

	FindClose(hFind);
}
