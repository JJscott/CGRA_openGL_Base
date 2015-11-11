//----------------------------------------------------------------------------
//
// OpenGL header
// Set up to include eveything nessesary for OpenGL functionality in one place
//
//----------------------------------------------------------------------------

// include glew.h before (instead of) gl.h, or anything that includes gl.h
// glew.h replaces gl.h and sets up OpenGL functions in a cross-platform manner
#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <GLFW/glfw3.h>