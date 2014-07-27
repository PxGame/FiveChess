# define _CRT_SECURE_NO_WARNINGS
# include <Windows.h>
# include <iostream>

///////////////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���
HINSTANCE m_hInst;								//����ʵ��
HWND m_hWnd;									//�����ھ��
LPCSTR m_lpWindowName = "MainWindow";			//������

HDC m_hDC;										//�豸DC
HBRUSH m_hBrush;								//��ˢ���

int m_sWidth = ::GetSystemMetrics(SM_CXSCREEN);	//��ȡ��Ļ��С
int m_sHeight = ::GetSystemMetrics(SM_CYSCREEN);

int m_Width = 500;								//���ڴ�С
int m_Height = 500;

int m_ChessPos[25][25] = { 0 };					//��������
POINT m_pt;										//����һ����������±�
//BOOL m_ChessIsYes = FALSE;						//����λ���Ƿ���Ч

////////////////////////////////////////////////////////////////////////////////////
//��������
LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//��Ϣ��Ӧ
HWND InitApplication(HINSTANCE hInstance);										//��ʼ������
void DrawChessboard(void);														//��������
void DrawChess(int player, int x, int y);										//��������
void DrawAllChess(void);														//������������
void PlayChess(int player = 2);													//����
void isWinner(void);															//��ǰһ�����Ƿ�ʤ��
void ChessRule(void);															//�������


///////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int ShowCmd)
{
	////����ռ�����̲���
	//for (int i = 0; i < 25; i++)
	//{
	//	for (int j = 0; j < 25; j++)
	//	{
	//		m_ChessPos[i][j] = 2;
	//	}
	//}


	m_hInst = hInstance;//�������ʵ��

	if (NULL == InitApplication(hInstance))
	{
		return ERROR;
	}

	m_hDC = GetDC(m_hWnd);//�����豸DC

	ShowWindow(m_hWnd, SW_SHOW);//��ʾ����

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WinProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		DrawChessboard();
		DrawAllChess();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		ChessRule();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CLOSE :
		if (IDYES == MessageBox(m_hWnd, "�Ƿ�ر���Ϸ��", "��ʾ", MB_YESNO))
		{
			//�ͷ���Դ
			ReleaseDC(m_hWnd, m_hDC);
			DeleteObject(m_hBrush);
			//������Ϣ
			DestroyWindow(m_hWnd);
		}
		break;
	case WM_DESTROY :
		PostQuitMessage(0);
		break;
	default :
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

HWND InitApplication(HINSTANCE hInstance)
{
	WNDCLASS wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WinProc;
	wnd.lpszClassName = m_lpWindowName;
	wnd.lpszMenuName = NULL;
	wnd.style = NULL;

	RegisterClass(&wnd);

	m_hWnd = CreateWindow(m_lpWindowName, "������", WS_CAPTION | WS_SYSMENU,
		(m_sWidth - m_Width) / 2, (m_sHeight - m_Height) / 2, m_Width + 6, m_Height + 28,
		NULL, NULL, hInstance, NULL);

	return m_hWnd;
}

///////////////////////////////////////////////////////////////////////////////////
void DrawChessboard(void)
{
	int delta = 0;//����
	
	//����
	for (delta = 0; delta <= 480; delta += 20)
	{
		//����
		MoveToEx(m_hDC, 10, 10 + delta, NULL);
		LineTo(m_hDC, 490, 10 + delta);

		//����
		MoveToEx(m_hDC, 10 + delta, 10, NULL);
		LineTo(m_hDC, 10 + delta, 490);
	}
	
	//�����ĵ�
	m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	SelectObject(m_hDC, m_hBrush);
	Ellipse(m_hDC, 247, 247, 253, 253);
}

void DrawChess(int player, int x, int y)
{
	int r = 8;//Բ�İ뾶

	switch (player)
	{
	case 1 :
		m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);//ʵ�ĺ�Բ
		break;
	case 2 :
		m_hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);//���ĺ�Ȧ
		break;
	default:
		return ;
	}

	SelectObject(m_hDC, m_hBrush);//ѡ��ˢ
	Ellipse(m_hDC, x - r, y - r, x + r, y + r);//����
}

void DrawAllChess(void)
{
	int x = 0, y = 0;
	for (x = 0; x <25; x++)
	{
		for (y = 0; y < 25; y++)
		{
			DrawChess(m_ChessPos[x][y], x * 20 + 10, y * 20 + 10);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void PlayChess(int player)
{
	//ȡ����
	POINT pt;

	while (1)
	{
		//��ȡ������û���������
		if (!GetCursorPos(&pt))
		{
			MessageBox(m_hWnd, "void PlayChess(void) is error !", "ERROR", NULL);
		}
		ScreenToClient(m_hWnd, &pt);//ת��Ϊ�ͻ�������

		//��ȡ�����Ӧ�������±�
		m_pt.x = pt.x / 20;
		m_pt.y = pt.y / 20;

		if (m_ChessPos[m_pt.x][m_pt.y] == 0)//�����λ��δ���£���д�����ӣ��˳�ѭ����������������
		{
			//EnableWindow(m_hWnd, TRUE);

			//д�����飬�����»�������
			m_ChessPos[m_pt.x][m_pt.y] = player;
			SendMessage(m_hWnd, WM_PAINT, 0, 0);
			break;
		}

		//EnableWindow(m_hWnd, FALSE);
		MessageBox(NULL, "��λ���������ӣ������»�һλ�ã�", "ERROR", NULL);
	}

	////�������ݻ�ȡ���
	//char str[100];
	//std::sprintf(str, "pt.x = %d, pt.y = %d\nx = %d, y = %d", pt.x, pt.y, x, y);
	//MessageBox(m_hWnd, str, "TEXT", NULL);
}

////////////////////////////////////////////////////////////////////////////////
void isWinner(void)
{
	int delta = 0;		//��б��

	int LeftDiagonal_sum = 0;		//��б�������Ӹ���
	int RightDiagonal_sum = 0;		//��б�������Ӹ���
	int HorizontalLine_sum = 0;		//���������Ӹ���
	int VerticalLine_sum = 0;		//���������Ӹ���
	
	//------------------------------------------��б��
	//���
	for (delta = 1; m_pt.x - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y + delta])
		{
			LeftDiagonal_sum++;
		}
		else
		{
			break;
		}
	}
	//�ұ�
	for (delta = 1; m_pt.y - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y - delta])
		{
			LeftDiagonal_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------��б��
	//���
	for (delta = 1; m_pt.y - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y - delta])
		{
			RightDiagonal_sum++;
		}
		else
		{
			break;
		}
	}
	//�ұ�
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y + delta])
		{
			RightDiagonal_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------����
	//���
	for (delta = 1; m_pt.x - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		else
		{
			break;
		}
	}
	//�ұ�
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------����
	//�ϱ�
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y - delta])
		{
			VerticalLine_sum++;
		}
		else
		{
			break;
		}
	}
	//�±�

	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y + delta])
		{
			VerticalLine_sum++;
		}
		else
		{
			break;
		}
	}

	////�������ݻ�ȡ���
	//char str[200];
	//std::sprintf(str, "x = %d, y = %d\nLeftDiagonal_sum = %d, RightDiagonal_sum = %d\nHorizontalLine_sum = %d, VerticalLine_sum = %d", 
	//	m_pt.x,m_pt.y, LeftDiagonal_sum, RightDiagonal_sum, HorizontalLine_sum, VerticalLine_sum);
	//MessageBox(NULL, str, "TEXT", NULL);

	if (LeftDiagonal_sum >= 4 || RightDiagonal_sum >= 4 || HorizontalLine_sum >= 4 || VerticalLine_sum >= 4)
	{
		MessageBox(NULL, "Winner !", "TEXT", NULL);
	}

}

void ChessRule(void)
{
	static int player = 1;//��һ�����

	//ѡ���������
	switch (player)
	{
	case 1:
		PlayChess(1);
		player++;
		break;
	case 2:
		PlayChess(2);
		player--;
		break;
	default :
		MessageBox(NULL, "ChessRule() is error.\nNot the player !", "ERROR", NULL);
	}

	//�ж��Ƿ�ʤ��
	isWinner();
}