#ifndef _DISPLAY_SURFACE_H
#define _DISPLAY_SURFACE_H
enum SURFACE_DISPLAYMODE
{
	SURFACE_OVERLAY_PRIORITY=0,
	SURFACE_OVERLAY,
	SURFACE_OFFSCREEN
};
typedef struct _DS_CREATE_PARAM
{
	long lWidth;
	long lHeight;
	SURFACE_DISPLAYMODE DisplayMode;
	long lLeft;
	long lTop;
	long lRight;
	long lBottom;
}DS_CREATE_PARAM;
typedef struct _DS_IMAGE
{
	unsigned char *pImage;
	long  lWidth;
	long  lHeight;
	long  lPitch;
	long  lColorSpace;
}DS_IMAGE;

void * DisplaySurfaceCreate(DS_CREATE_PARAM *pDcp);
unsigned int DisplaySurfaceShowImage(void * pDisplay,DS_IMAGE *pDi);
unsigned int DisplaySurfaceDestroy(void * pDisplay);
#endif