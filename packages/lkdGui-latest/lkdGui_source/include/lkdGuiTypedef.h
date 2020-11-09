
#ifndef __LKDGUITYPEDEF_H
#define	__LKDGUITYPEDEF_H

#include <stdint.h>
#include "lkdGuiConfig.h"

#ifndef  NULL
#define NULL 0
#endif

/* lkdGui�汾�� */
#define LKGGUI_VERSION "V000.000.001"

#define CWHITLE	0x00/* ��ɫ */
#define CBLACK	0x01/* ��ɫ */

typedef int32_t lkdCoord;
typedef uint8_t lkdColour;

/* ����λͼ�ṹ�� */
typedef struct{
    uint8_t *bitmap;	/* λͼ���� */
    uint16_t wide;      /* λͼ�� */
    uint16_t high;      /* λͼ�� */
    uint16_t beginx;    /* λͼ��ʼ���� */
    uint16_t beginy;    /* λͼ��ʼ���� */
}lkdBitmap;


/* lkd���ڶ��� */
typedef struct{
    lkdCoord x;
    lkdCoord y;
    uint16_t wide;
    uint16_t hight;
    uint8_t   *title;   // ���������ָ��(NULLΪ����ʾ)
    void(*WindowFunction)(void *param); // ���ڶ�Ӧ�ķ������
    void * param;       //����
    uint8_t firstflag;
}lkdWin;

/* lkd���ڹ��� */
struct GuiWinManage{
    uint8_t pWin;
    lkdWin *winStack[GUIWINMANAGE_NUM];
};

/* ������ */
typedef struct{
    int16_t x;
    int16_t y;
    uint16_t hight;
    int16_t max;
    int16_t lump;
}lkdScroll;

/* ��ť */
typedef struct{
    lkdCoord x;     /* ���� */
    lkdCoord y;
    uint8_t wide;
    uint8_t high;
    uint8_t flag;   /* 0 ̧��״̬ 1 ����״̬ */
    uint8_t *name;  /* �������� */
	uint8_t linesize;  /* �»��߳��� */
}lkdButton;
enum ButtonState{
    BUTTON_UP,
    BUTTON_DOWN,
};

/* ������ */
typedef struct{
    lkdCoord x;     /* ���� */
    lkdCoord y;
    uint8_t wide;
    uint8_t high;
    uint8_t ratio; /* ��ǰ���� 0~100 */
}lkdProgress;

/* lkd�˵��ڵ㶨�� */
typedef struct lkdMenuNode{
	uint16_t nodeId;            /* �ڵ�� */
	uint8_t	*name;              /* �ڵ����� */
	struct lkdMenuNode *pBro;   /* ��һ���ֵܽڵ�� */
	struct lkdMenuNode *pSon;   /* �ӽڵ�� */
	void *user;
}lkdMenuNode;

/* lkd�˵�ջ�ṹ���� */
typedef struct {
	uint8_t	index;
	lkdCoord cy;
	lkdMenuNode *pNode;
}MenuStack;

/* lkd�˵��ṹ���� */
typedef struct{
	lkdCoord x;         /* �˵���ʼ���� */
	lkdCoord y;
	uint16_t wide;
	uint16_t hight;
	uint8_t ItemsWide;  /* �˵���� */
	uint8_t Itemshigh;  /* �˵���� */
	uint8_t index;      /* �˵���ǰitem */
	uint8_t count;      /* �˵��������� */
	uint8_t stackNum;   /* �˵�ջ��С ��MenuStackΪ��λ */
	MenuStack *stack;   /* �˵�ջ */
	lkdMenuNode *Root;  /* �˵����ڵ� */
	void (* MenuDealWithFun)(void);/* �˵������� */

	/* ��ǰ�ڵ�滭��ʱ���� */
	lkdCoord cx;        /* ��ʼ���� */
	lkdCoord cy;
	lkdMenuNode *pDraw; /* �滭�ڵ� */
	uint8_t nodeNum;    /* ��ǰ���ڵ��µĽڵ��� */
	uint8_t cNum;       /* Ҫ�滭�Ľڵ��� */
}lkdMenu;

/* ����������� */
typedef enum{
	FONT_LEFT,  /* ����� */
	FONT_MID,   /* ���� */
	FONT_RIGHT,	/* �Ҷ��� */
}FontFlag;

/* ����ṹ */
typedef struct{
	uint8_t *name;      /* �������� */
	uint8_t swide;      /* ASCIIȡģ�� */
	uint8_t shigh;      /* ASCIIȡģ�� */
	uint8_t dwide;      /* ����ȡģ�� */
	uint8_t dhigh;      /* ����ȡģ�� */
	uint8_t *pZmBuff;   /* ��ŵ�����ģbuff */
	/* ASCIIȡģ���� */
	uint8_t (*getSfont)(uint8_t code1, uint8_t *pBuff);
	/* ����ȡģ���� */
	uint8_t (*getDfont)(uint8_t code1, uint8_t code2,uint8_t *pBuff);
}lkdFont;

/* �ı���Ϣ */
typedef struct{
	lkdCoord x;         /* ���� */
	lkdCoord y;
	uint16_t wide;
	uint16_t high;
	uint16_t beginOffset;
	uint8_t wInterval;  /* ��ȼ�� */
	uint8_t hInterval;  /* �߶ȼ�� */
	uint8_t flag;       /* 0 ������ʾ 1���� */
}fontTextInfo;

#endif /* __LKDGUITYPEDEF_H */

/* END */
