


unsigned short int LCD_BGR2RGB(unsigned short int c);
void Gui_Circle(unsigned short int X,unsigned short int Y,unsigned short int R,unsigned short int fc); 
void Gui_DrawLine(unsigned short int x0, unsigned short int y0,unsigned short int x1, unsigned short int y1,unsigned short int Color);  
void Gui_box(unsigned short int x, unsigned short int y, unsigned short int w, unsigned short int h,unsigned short int bc);
void Gui_box2(unsigned short int x,unsigned short int y,unsigned short int w,unsigned short int h, unsigned char mode);
void DisplayButtonDown(unsigned short int x1,unsigned short int y1,unsigned short int x2,unsigned short int y2);
void DisplayButtonUp(unsigned short int x1,unsigned short int y1,unsigned short int x2,unsigned short int y2);
void Gui_DrawFont_GBK16(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_GBK18(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_GBK24(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_Num32(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned short int num) ;
void Gui_DrawFont_GBK12_8(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_GBK32_16(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_asc2_5x7(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);
void Gui_DrawFont_asc12x24(unsigned short int x, unsigned short int y, unsigned short int fc, unsigned short int bc, unsigned char *s);



