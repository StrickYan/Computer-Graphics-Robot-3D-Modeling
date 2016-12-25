#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{ 
	DEFAULT_CAMERA_POINT,XPOS, YPOS, ZPOS,  BODY_ROTATE, SWORD_LENGTH,HEAD_ROTATE,LEFT_ARM_ROTATE,RIGHT_ARM_ROTATE,LEFT_ARM_LINK,RIGHT_ARM_LINK,CAP,SHIELD,LIGHT_X_POSITION,LIGHT_Y_POSITION,LIGHT_Z_POSITION,LIGHT_Diffuse,HIERARCHY,RESET,Cloak,WALK_ON,JET_SYSTEM_ON, NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f
#define COLOR_YELLOW	1.0f, 0.9f, 0.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

#endif