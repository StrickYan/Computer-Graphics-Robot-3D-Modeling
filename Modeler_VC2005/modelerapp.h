// DO NOT mess with this file.  If you do, animator will not work with
// your model, and you'll have to write a new one.  If you really really
// really need to do something here (unlikely) then don't complain if and
// when animator doesn't work.  -- Eugene
#ifndef MODELERAPP_H
#define MODELERAPP_H

#include "modelerview.h"

struct ModelerControl
{
	ModelerControl();
	ModelerControl(const char* name, float minimum, float maximum, double stepsize, float value);
	ModelerControl(const ModelerControl &o);
	ModelerControl& operator=(const ModelerControl &o);
	void SetVals(const char* name, float minimum, float maximum, double stepsize, float value);

	char  m_name[128];
	float m_minimum;
	float m_maximum;
	double m_stepsize;
	float m_value;
};

// Forward declarations for ModelerApplication
class ModelerView;
class ModelerUserInterface;
class Fl_Box;
class Fl_Slider;
class Fl_Value_Slider;

// The ModelerApplication is implemented as a "singleton" design pattern,
// the purpose of which is to only allow one instance of it.
class ModelerApplication
{
public:
	~ModelerApplication();

	// Fetch the global ModelerApplication instance
	static ModelerApplication* Instance();

    // Initialize the application; see sample models for usage
	void Init(ModelerViewCreator_f createView, 
              const ModelerControl controls[], 
              unsigned numControls); 

    // Starts the application, returns when application is closed
	int  Run();

    // Get and set slider values.
    double GetControlValue(int controlNumber);
    void   SetControlValue(int controlNumber, double value);

private:
	// Private for singleton
	ModelerApplication() : m_numControls(-1) {}
	ModelerApplication(const ModelerApplication&) {}
	ModelerApplication& operator=(const ModelerApplication&) {}
	
	// The instance
	static ModelerApplication *m_instance;

	// I'll let my friend touch my private parts
	friend class ModelerUserInterface;

    void    ShowControl(int controlNumber);
    void    HideControl(int controlNumber);

	ModelerUserInterface *m_ui;
	int					  m_numControls;

    Fl_Box               **m_controlLabelBoxes;
    Fl_Value_Slider      **m_controlValueSliders;

    static void SliderCallback(Fl_Slider *, void*);
	static void RedrawLoop(void*);

	// Just a flag for updates
	bool m_animating;
};

#endif