#ifndef APPLICATION_T
#define APPLICATION_T

typedef struct Application Application;
struct Application
{
	char *res_name;
	char *res_class;
	ubyte geometry_mask;
	XRectangle geometry;
	Application *next;
};

#endif /* not APPLICATION_T */
