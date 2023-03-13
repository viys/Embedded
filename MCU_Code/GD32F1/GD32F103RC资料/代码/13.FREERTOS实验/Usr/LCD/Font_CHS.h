#ifndef		__FONT_CHS_H__
#define		__FONT_CHS_H__

//--------------------- include files ---------------------
//--------------------- macro define ----------------------
#define hz16_num   82
#define hz18_num   50
#define hz24_num   9
//--------------------- macro define ----------------------
typedef struct typFNT_GB162
{
       unsigned char Index[2];	
       char Msk[32];
}typFNT_GB162_t;

typedef struct typFNT_GB242
{
       unsigned char Index[2];	
       char Msk[72];
}typFNT_GB242_t;

typedef struct typFNT_GB18
{
       unsigned char Index[2];	
       char Msk[54];
}typFNT_GB18_t;
//--------------------- variable declare ------------------
extern const typFNT_GB162_t hz16[];
extern const typFNT_GB242_t hz24[];
extern const typFNT_GB18_t hz18[];
//--------------------- function declare ------------------


#endif	//__FONT_CHS_H__

