//
// Created by yifei on 4/23/18.
//

#ifndef DVRK_OPENGLVIEWER_GLERROR_H
#define DVRK_OPENGLVIEWER_GLERROR_H

#include <vtk_glew.h>

void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)


#endif //DVRK_OPENGLVIEWER_GLERROR_H
