#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "lkdGui.h"
#include "12864.h"

uint8_t x_to_page[GUIXMAX][8]={0};

void GuiUpdateDisplayAll(void)
{
		uint32_t x,y;
		for(y = 0; y <8; y++)
		{
			lcd_address(y,0);
			for(x = 0;x <128; x++)
			{
					WriteLcdData(x_to_page[x][y]);
			}
		}
}

void GuiRangeUpdateDisplay(lkdCoord beginx, lkdCoord beginy,lkdCoord endx, lkdCoord endy)
{

}

void GuiDrawPoint(lkdCoord x, lkdCoord y, lkdColour color)
{
  	if(color == CWHITLE)
		{
			x_to_page[x ][y/8] &= ~(1 << (y % 8));
		}
		else
		{
			x_to_page[x ][y/8] |= 1 << (y % 8);
		}
}

void GuiFlipPoint(lkdCoord x, lkdCoord y, lkdColour color)
{
  	if(color == CWHITLE)
		{
			x_to_page[y ][x/8] &= ~(1 << (x % 8));
		}
		else
		{
			x_to_page[y ][x/8] |= 1 << (x % 8);
		}
}

void  GuiReadPoint(lkdCoord x, lkdCoord y, lkdColour *pColor)
{
}

