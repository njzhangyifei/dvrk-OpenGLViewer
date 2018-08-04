//
// Created by yifei on 4/23/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKEXTERNALOPENGLRENDERWINDOWFIXED_H
#define DVRK_OPENGLVIEWER_VTKEXTERNALOPENGLRENDERWINDOWFIXED_H

#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>

class vtkExternalOpenGLRenderWindowFixed : public vtkExternalOpenGLRenderWindow{
public:
    static vtkExternalOpenGLRenderWindowFixed *New();
    void Start(void) override;
};


#endif //DVRK_OPENGLVIEWER_VTKEXTERNALOPENGLRENDERWINDOWFIXED_H
