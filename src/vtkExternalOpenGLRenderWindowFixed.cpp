//
// Created by yifei on 4/23/18.
//

#include <vtk_glew.h>

#include "dvrk_OpenGLViewer/vtkExternalOpenGLRenderWindowFixed.h"
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>

vtkStandardNewMacro(vtkExternalOpenGLRenderWindowFixed);

void vtkExternalOpenGLRenderWindowFixed::Start(void)
{
  // Make sure all important OpenGL options are set for VTK
  this->OpenGLInit();

  // Use hardware acceleration
  this->SetIsDirect(1);
}


