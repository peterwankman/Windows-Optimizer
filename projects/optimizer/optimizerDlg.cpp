/* 
 * Optimizer -- Speed up Windows
 * 
 * Copyright (C) 2007-2011  Anonymous
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to 
 * the Free Software Foundation, Inc.
 * 51 Franklin Street, Fifth Floor
 * Boston, MA  02110-1301, USA
 * 
 */

#include "stdafx.h"
#include "optimizer.h"
#include "optimizerDlg.h"

#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NUMSTRINGS	5

void SetSlider(CSliderCtrl *Slider, int Sel) {
	int Min, Max, Inc;

	Min = Slider->GetRangeMin();
	Max = Slider->GetRangeMax();
	Inc = ((Max - Min) * .9) / (NUMSTRINGS - 1);

	Slider->SetPos(Sel * Inc + Min);
}

CoptimizerDlg::CoptimizerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CoptimizerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CoptimizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CoptimizerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_PRESET, &CoptimizerDlg::OnBnClickedRadioPreset)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, &CoptimizerDlg::OnBnClickedRadioCustom)
	ON_CBN_SELCHANGE(IDC_COMBO_PRESET, &CoptimizerDlg::OnCbnSelchangeComboPreset)
	ON_BN_CLICKED(IDC_BUTTON1, &CoptimizerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CoptimizerDlg::OnInitDialog()
{
	CSliderCtrl *Slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_CUSTOM);
	CComboBox *CBox = (CComboBox *)GetDlgItem(IDC_COMBO_PRESET);
	CProgressCtrl *ProgressBar = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	CString buf;
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
	CheckRadioButton(IDC_RADIO_PRESET, IDC_RADIO_CUSTOM, IDC_RADIO_PRESET);

	Slider->SetRange(1000, 10000);
	Slider->SetTicFreq(1000);	

	CBox->ResetContent();
	for(int i = 0; i < NUMSTRINGS; i++) {
		buf.LoadString(IDS_STRING1 + i);
		CBox->AddString(buf);
	}

	CBox->SetCurSel(2);
	SetSlider(Slider, 2);

	ProgressBar->SetRange(0, 100);

	return TRUE;
}

void CoptimizerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CoptimizerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CoptimizerDlg::OnBnClickedRadioPreset()
{	
	CComboBox *CBox = (CComboBox *)GetDlgItem(IDC_COMBO_PRESET);
	CSliderCtrl *Slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_CUSTOM);
	int Sel;

	Sel = CBox->GetCurSel();
	CBox->EnableWindow(TRUE);
	
	SetSlider(Slider, Sel);
	Slider->EnableWindow(FALSE);
}

void CoptimizerDlg::OnBnClickedRadioCustom()
{
	CSliderCtrl *Slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_CUSTOM);
	CComboBox *CBox = (CComboBox *)GetDlgItem(IDC_COMBO_PRESET);

	CBox->EnableWindow(FALSE);
	Slider->EnableWindow(TRUE);
}

void CoptimizerDlg::OnCbnSelchangeComboPreset()
{
	CSliderCtrl *Slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_CUSTOM);
	CComboBox *CBox = (CComboBox *)GetDlgItem(IDC_COMBO_PRESET);
	int Sel;

	Sel = CBox->GetCurSel();
	SetSlider(Slider, Sel);
}

void CoptimizerDlg::OnBnClickedButton1()
{
	CButton *OptimizeButton = (CButton*)GetDlgItem(IDC_BUTTON1);
	CButton *CancelButton = (CButton*)GetDlgItem(IDCANCEL);
	CProgressCtrl *ProgressBar = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	int RetVal;

	OptimizeButton->SetWindowText(TEXT("Please wait..."));
	OptimizeButton->EnableWindow(FALSE);
	CancelButton->EnableWindow(FALSE);

	ProgressBar->SetPos(0);
	OptimizeBoot(OptimizeButton);
#ifndef LDRONLY
	ProgressBar->SetPos(25);
	OptimizeRegistry(OptimizeButton);
	ProgressBar->SetPos(50);
	OptimizeLibraries(OptimizeButton);
	ProgressBar->SetPos(75);
	OptimizeSystem(OptimizeButton);
#endif
	ProgressBar->SetPos(100);

	OptimizeButton->SetWindowText(TEXT("Optimize!"));
	OptimizeButton->EnableWindow(TRUE);
	CancelButton->EnableWindow(TRUE);
	
	RetVal = MessageBox(TEXT("You need to reboot the system for the changes to take effect. Reboot now?"),
		TEXT("Done."), MB_OKCANCEL);

	if(RetVal == IDOK)
		Reboot();
}

/*)\
\(*/
