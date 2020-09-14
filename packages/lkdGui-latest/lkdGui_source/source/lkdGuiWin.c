/**
  * @file   lkdGuiWin.c
  * @author  guoweilkd
  * @version V0.0.0
  * @date    2018/06/03
  * @brief  lkdGui���ڹ���
  */

#include "lkdGui.h"

/* ���ڹ���ṹ */
static struct GuiWinManage winManage;

/**
  *@brief  ���ƴ���
  *@param  pWindow ����ָ��
  *@retval None
  */
void GuiWinDraw(lkdWin *pWindow)
{
	lkdColour forecolor,backcolor;

	forecolor = GuiGetForecolor();
	backcolor=  GuiGetbackcolor();

	if(pWindow->x + pWindow->wide - 1 > GUIXMAX || pWindow->y + pWindow->hight - 1 > GUIYMAX ||\
		 pWindow->x < 0 || pWindow->y < 0 || pWindow->wide == 0 || pWindow->hight == 0){
		   return;
	}
	if(pWindow->title != NULL){
		GuiFillRect(pWindow->x, pWindow->y, pWindow->x + pWindow->wide - 1,\
		11, forecolor);
		GuiExchangeColor();
		GuiRowText(pWindow->x + 4, pWindow->y+2, pWindow->wide - 4,\
			FONT_MID,pWindow->title);
		GuiExchangeColor();
	}
	GuiUpdateDisplayAll();
}

/**
  *@brief  ��Ӵ���
  *@param  pWindow ����ָ��
  *@retval 0 �ɹ� -1 ʧ��
  */
int8_t GuiWinAdd(lkdWin *pWindow)
{
	GuiWinDraw(pWindow);
	if(winManage.pWin >= GUIWINMANAGE_NUM){
		return -1;
	}
	winManage.winStack[winManage.pWin] = pWindow;
	winManage.pWin ++;
	return 0;
}

/**
  *@brief  ִ�ж��㴰��
  *@param  None
  *@retval None
  */
void GuiWinDisplay(void)
{
	if(winManage.pWin == 0 || winManage.pWin > GUIWINMANAGE_NUM){
		return;
	}
	lkdWin *pThis = winManage.winStack[winManage.pWin - 1];
	pThis->WindowFunction(pThis->param);
}

/**
  *@brief  ɾ����ǰ����
  *@param  None
  *@retval None
  */
void GuiWinDeleteTop(void)
{
	if(winManage.pWin <= 1 || winManage.pWin > GUIWINMANAGE_NUM){
		return;
	}
	winManage.pWin --;
	GuiWinDraw(winManage.winStack[winManage.pWin - 1]);
}

/**
  *@brief  ��ȡ���㴰��
  *@param  None
  *@retval ���㴰��
  */
lkdWin *GuiGetTopWin(void)
{
	lkdWin *pTopWin = NULL;

	if(winManage.pWin >= 1){
		pTopWin = winManage.winStack[winManage.pWin - 1];
	}
	return pTopWin;
}

void GuiWinInit(void)
{
	winManage.pWin = 0;
}
/* END */
