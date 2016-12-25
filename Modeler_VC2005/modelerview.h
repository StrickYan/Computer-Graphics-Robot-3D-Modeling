// modelerview.h

// This is the base class for all your models.  It contains
// a camera control for your use.  The draw() function will 
// set up default lighting and apply the projection, so if you 
// inherit, you will probably want to call ModelerView::draw()
// to set up the camera.

#ifndef MODELERVIEW_H
#define MODELERVIEW_H

#include <FL/Fl_Gl_Window.H>

class Camera;
class ModelerView;
typedef ModelerView* (*ModelerViewCreator_f)(int x, int y, int w, int h, char *label);

class ModelerView : public Fl_Gl_Window
{
public:
    ModelerView(int x, int y, int w, int h, char *label=0);

	virtual ~ModelerView();
    virtual int handle(int event);
    virtual void draw();

    Camera *m_camera;
};


#endif
