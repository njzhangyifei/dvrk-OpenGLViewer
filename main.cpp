#include <iostream>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <memory>
#include <thread>
#include <vtk_glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "ShadowMapQuad.h"
#include "ImageProvider.h"

#define SHADOW_MAP_QUAD_V_SHADER_PATH "./debug_shadow_map.vert"
#define SHADOW_MAP_QUAD_F_SHADER_PATH "./debug_shadow_map.frag"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int, char *[])
{
//    ImageProvider img (400, 400);
//    cv::namedWindow("test", CV_WINDOW_AUTOSIZE);
//    cv::imshow("test", img.image);
//    cv::waitKey(0);
    glfwInit();
    GLFWwindow * window_L = glfwCreateWindow(400, 400, "test_l", NULL, NULL);
    GLFWwindow * window_R = glfwCreateWindow(400, 400, "test_r", NULL, window_L);


    glfwSetKeyCallback(window_L, key_callback);


    glfwMakeContextCurrent(window_L);
    glfwSwapInterval(1);
    glfwMakeContextCurrent(window_R);
    glfwSwapInterval(1);

    glfwMakeContextCurrent(window_R);
    glewInit();
    auto shadow_map_quad_shader = LoadShaders(SHADOW_MAP_QUAD_V_SHADER_PATH, SHADOW_MAP_QUAD_F_SHADER_PATH);
    auto shadow_map_quad = std::unique_ptr<ShadowMapQuad>(new ShadowMapQuad({0.5, 1.0}, {1.0, 0.5}));
    glfwMakeContextCurrent(window_L);
    shadow_map_quad->load_data();


    std::thread t([&]() {
        while (!glfwWindowShouldClose(window_R)) {
            glfwMakeContextCurrent(window_R);
            int width, height;
            glfwGetFramebufferSize(window_R, &width, &height);
            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shadow_map_quad->draw(0);

            glfwSwapBuffers(window_R);
        }
    });
    while (!glfwWindowShouldClose(window_L)){
        glfwMakeContextCurrent(window_L);
        int width, height;
        glfwGetFramebufferSize(window_L, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shadow_map_quad->draw(0);

        glfwSwapBuffers(window_L);
        glfwPollEvents();
    }
    t.join();

    glfwDestroyWindow(window_L);
    glfwDestroyWindow(window_R);
    glfwTerminate();
    glDeleteProgram(shadow_map_quad_shader);
    return EXIT_SUCCESS;
    // This creates a polygonal cylinder model with eight circumferential facets
    // (i.e, in practice an octagonal prism).
    vtkSmartPointer<vtkCylinderSource> cylinder =
            vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetResolution(8);

    // The mapper is responsible for pushing the geometry into the graphics library.
    // It may also do color mapping, if scalars or other attributes are defined.
    vtkSmartPointer<vtkPolyDataMapper> cylinderMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // The actor is a grouping mechanism: besides the geometry (mapper), it
    // also has a property, transformation matrix, and/or texture map.
    // Here we set its color and rotate it around the X and Y axes.
    vtkSmartPointer<vtkActor> cylinderActor =
            vtkSmartPointer<vtkActor>::New();
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(-45.0);

    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(cylinderActor);
    renderer->SetBackground(0.1, 0.2, 0.4);
    // Zoom in a little by accessing the camera and invoking its "Zoom" method.
    renderer->ResetCamera();
    renderer->GetActiveCamera()->UseOffAxisProjectionOn();
    renderer->GetActiveCamera()->Zoom(1.5);

    // The render window is the actual GUI window
    // that appears on the computer screen
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(200, 200);
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderer> renderer_R =
        vtkSmartPointer<vtkRenderer>::New();
    renderer_R->AddActor(cylinderActor);
    renderer_R->SetBackground(0.1, 0.2, 0.4);
    renderer_R->SetActiveCamera(renderer->GetActiveCamera());

    // The render window is the actual GUI window
    // that appears on the computer screen
    vtkSmartPointer<vtkRenderWindow> renderWindow_R =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow_R->SetSize(200, 200);
    renderWindow_R->AddRenderer(renderer_R);

    renderWindow->StereoCapableWindowOn();
    renderWindow->SetStereoTypeToLeft();
    renderWindow->StereoRenderOn();

    renderWindow_R->StereoCapableWindowOn();
    renderWindow_R->SetStereoTypeToRight();
    renderWindow_R->StereoRenderOn();

    // The render window interactor captures mouse events
    // and will perform appropriate camera or actor manipulation
    // depending on the nature of the events.
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

     vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor_R =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor_R->SetRenderWindow(renderWindow_R);

    // This starts the event loop and as a side effect causes an initial render.
//    renderWindow_R->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
