
// EpisodeNameReplacerDlg.h : header file
//

#pragma once

#include "XMLParser.h"

// CEpisodeNameReplacerDlg dialog
class CEpisodeNameReplacerDlg : public CDialog
{
public:
	CEpisodeNameReplacerDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_EPISODENAMEREPLACER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	BOOL GetXMLData(CString rRequest, CString& rXMLData);

	afx_msg void OnBnClickedGetepisodes();
	afx_msg void OnBnClickedGetseasons();	
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedReplace();

	afx_msg void OnEnChangeSeries();
	afx_msg void OnEnChangeSeasonfolder();


private:
	CString m_rFolderPath;
	CString m_rSeasonNumber;
	CString m_rSeriesName;

	XML_PARSER m_xmlParser;
	CListCtrl m_lstEpisodes;
	CComboBox m_cmbSeasons;

	void EnableControls();
	void EnableDlgItem(UINT nID, BOOL bEnable);
	
	CString GetEpisodeTitle(int nEpNumber);
};

