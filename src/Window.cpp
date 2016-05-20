#include "Window.hpp"

bool cameraEngaged;
bool firstUpdate = true;
bool frozen = false;
double atTime = 0.0;
size_t cameraIdx = 0;
bool resetCamera = false;

const char* window_title = "Petting a cat's tummy is dangerous, but nothing ventured, nothing gained";

glm::vec3 currTBPos;
glm::vec3 prevTBPos;

int windowX;
int windowY;

float currX;
float prevX;
float currZ;

float currY;
float prevY;
float prevZ;

int Window::width;
int Window::height;

glm::mat4 P;

std::shared_ptr<OBJObject> test;

auto camera = std::make_shared<OrbitalCamera>(glm::vec3(0.0f, 1.0f, 0.0f),
                                              glm::vec3(0.0f, 0.0f, 25.0f));

std::shared_ptr<Group> scene;

void Window::initialize_objects()
{
  cameraEngaged = false;

  scene = getScene(camera);

  atTime = 0.0f;

  currY = 0.0f;
  currX = 0.0f;
  currZ = 0.0f;

}

void Window::clean_up()
{ /* yay std::shared_ptr! */ }

GLFWwindow* Window::create_window(int width, int height)
{
  // Initialize GLFW
  if (!glfwInit())
    {
      fprintf(stderr, "Failed to initialize GLFW\n");
      return NULL;
    }

  // 4x antialiasing
  glfwWindowHint(GLFW_SAMPLES, 4);

  // Create the GLFW window
  GLFWwindow* window = glfwCreateWindow(width,
                                        height,
                                        window_title,
                                        NULL,
                                        NULL);

  // Check if the window could not be created
  if (!window)
    {
      fprintf(stderr, "Failed to open GLFW window.\n");
      glfwTerminate();
      return NULL;
    }

  // Make the context of the window
  glfwMakeContextCurrent(window);

  // Set swap interval to 1
  glfwSwapInterval(1);

  glfwGetFramebufferSize(window, &width, &height);

  // Call the resize callback to make sure things get drawn immediately
  Window::resize_callback(window, width, height);

  return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
  Window::width = width;
  Window::height = height;

  glViewport(0, 0, width, height);

  P = glm::perspective(glm::radians(60.0f),
                       (float) width / (float) height,
                       0.1f,
                       1000.0f);
}

void Window::idle_callback()
{
  if (firstUpdate)
    {
      glfwSetTime(atTime);
      firstUpdate = false;
    }
  else if (frozen)
    {
      glfwSetTime(atTime);
    }
  scene->update(glfwGetTime());
}

void Window::display_callback(GLFWwindow* window)
{
  if (resetCamera)
    {
      auto cameras = getCameras(*scene);

      for (auto & curr : cameras)
        {
          auto oc = std::dynamic_pointer_cast<OrbitalCamera>(curr.first);
          oc->reset();
        }

      resetCamera = false;
    }

  auto drawFn = getDrawFn(P);
  auto cameras = getCameras(*scene);
  draw(drawFn, *scene, cameras[cameraIdx]);

  // Gets events, including input such as keyboard and mouse or window resizing
  glfwPollEvents();
  // Swap buffers
  glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
    {
      switch (key)
        {
        case GLFW_KEY_R:
          resetCamera = true;
          break;
        case GLFW_KEY_SPACE:
          frozen = !frozen;
          if (frozen)
            {
              atTime = glfwGetTime();
            }
          break;
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(window, GL_TRUE);
          break;
        }
    }
}



void Window::mouseCallback(GLFWwindow * window,
                           int button,
                           int action,
                           int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      if (action == GLFW_RELEASE) cameraEngaged = false;
      else cameraEngaged = true;
    }
}

void Window::mousePosCallback(GLFWwindow * window,
                              double x,
                              double y)
{
  prevTBPos = currTBPos;
  currTBPos = trackBallMapping((float) width, (float) height,
                               (float) x, (float) y);

  windowX = (int) x;
  windowY = (int) y;

  prevX = currX;
  prevY = currY;
  currX = mapRange((float) x, 0.0f, (float) width, -1.0f, 1.0f);
  currY = mapRange((float) y, 0.0f, (float) height, -1.0f, 1.0f);

  if (cameraEngaged)
    {
      camera->orbit = (orbit(prevTBPos, currTBPos, 0.5f)) * camera->orbit;
    }
}

void Window::scrollCallback(GLFWwindow* window,
                             double xoffset,
                             double yoffset)
{
  prevZ = currZ;
  currZ = currZ - (((float) yoffset) * 0.01f);

  camera->move = move(glm::vec3(prevX, prevY, prevZ),
                      glm::vec3(prevX, prevY, currZ),
                      25.0f) * camera->move;
}
