#include "Scene.hpp"

std::shared_ptr<SkyBox> skyBox;

const char * rightPath = "res/sahara_rt.tga";
const char * leftPath = "res/sahara_lf.tga";
const char * topPath = "res/sahara_up.tga";
const char * bottomPath = "res/sahara_dn.tga";
const char * backPath = "res/sahara_bk.tga";
const char * frontPath = "res/sahara_ft.tga";

const char * vertPath = "shader/shader.vert";
const char * sbVertPath = "shader/sbshader.vert";
const char * solidColorVertPath = "shader/solidColor.vert";
const char * fragPath = "shader/shader.frag";
const char * sbFragPath = "shader/sbshader.frag";
const char * solidColorFragPath = "shader/solidColor.frag";

static std::shared_ptr<Transform> makeSky()
{
  auto skyBoxShader = std::make_shared<Shader>(sbVertPath, sbFragPath);

  auto skyBoxScaleMat = glm::scale(glm::mat4(),
                                   glm::vec3(500.0f, 500.0f, 500.0f));

  skyBox = std::make_shared<SkyBox>(skyBoxShader,
                                    GL_TEXTURE0,
                                    rightPath,
                                    leftPath,
                                    topPath,
                                    bottomPath,
                                    backPath,
                                    frontPath);

  return std::make_shared<Transform>(skyBox, skyBoxScaleMat);
}

static std::shared_ptr<Transform> makeGround()
{
  size_t cols = 1;
  size_t width = glm::pow(2, 5) + 1;
  std::vector<float> heights;

  for (size_t y = 0; y < width; ++y)
    {
      for (size_t x = 0; x < width; ++x)
        {
          std::cerr << "init = " << ((float)(y * width) + x)
            + (cos((float)(y * width) + x)) << std::endl;
          float val = mapRange(((float)(y * width) + x)
                               + 2.0f *(cos((float)(y * width) + x)),
                               0.0f, (((float)(width) * (width)) - 1.0f)
                                     + 2.0f * cos(((float)(width) * (width)) - 1.0f),
                               0, 1);
          std::cerr << "val = " << val <<std::endl;

          heights.push_back(val);
        }
    }

  auto patch = std::make_shared<LandscapeModel>(heights, cols, width);
  return std::make_shared<Transform>(patch,
                                     glm::rotate(glm::mat4(),
                                                 glm::pi<float>(),
                                                 glm::vec3(0.0f, 1.0f, 0.0f)));
}

std::shared_ptr<Group> getScene(std::shared_ptr<Camera> withCamera)
{
  auto root = std::make_shared<Group>();

  root->insert(makeSky());
  root->insert(makeGround());
  root->insert(withCamera);
  root->insert(std::make_shared<DirLight>(glm::vec3(0.0f, -5.0f, -25.0f),
                                          glm::vec3(1.0f, 1.0f, 1.0f)));

  return root;
}

DrawFn getDrawFn (const glm::mat4 & P)
{
  using namespace std;
  using namespace glm;

  return [=](const vector<pair<shared_ptr<Light>, mat4> > & lightP,
             const pair<shared_ptr<Camera>, mat4> & cameraP,
             const pair<shared_ptr<Drawable>, mat4> & modelP)
    {
      auto light = dynamic_pointer_cast<DirLight>(lightP[0].first);
      auto lightM = lightP[0].second;
      auto camera = cameraP.first;
      auto cameraM = cameraP.second;
      auto modelbase = modelP.first;
      auto modelM = modelP.second;

      if (typeid(*modelbase) == typeid(SkyBox)) // my kingdom for
        {                                       // proper ADTs
          auto model = dynamic_pointer_cast<SkyBox>(modelbase);
          auto uniformFn = [&](GLuint shaderProg)
            {
              auto M = modelM;
              auto PV = P * (cameraM) * mat4(mat3(camera->getV(cameraM)));

              // vertex shader uniforms

              GLuint PVID = glGetUniformLocation(shaderProg, "PV");
              glUniformMatrix4fv(PVID, 1, GL_FALSE, &PV[0][0]);

              GLuint MID = glGetUniformLocation(shaderProg, "M");
              glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);

              // fragment shader uniforms

            };
          glDepthMask(GL_FALSE);
          model->bind();
          model->shader->bind(uniformFn);

          glUniform1i(glGetUniformLocation(model->shader->getId(), "skybox"), 0);
          glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
          glDepthMask(GL_TRUE);
          model->shader->unbind();
          model->unbind();
        }
      else if (typeid(*modelbase) == typeid(LandscapeModel))
        {
          auto model = std::dynamic_pointer_cast<LandscapeModel>(modelbase);
          auto uniformFn = [=](GLuint shaderProg)
            {
              auto M = modelM;
              auto PV = P * camera->getV(cameraM);

              // vertex shader uniforms

              GLuint PVID = glGetUniformLocation(shaderProg, "PV");
              glUniformMatrix4fv(PVID, 1, GL_FALSE, &PV[0][0]);

              GLuint MID = glGetUniformLocation(shaderProg, "M");
              glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);

              // fragment shader uniforms

            };

          model->shader->bind(uniformFn);
          model->draw();
          model->shader->unbind();
        }
      else if (typeid(*modelbase) == typeid(OBJDrawable))
        {
          auto model = std::dynamic_pointer_cast<OBJDrawable>(modelbase);
          auto uniformFn = [=](GLuint shaderProg)
            {

              auto madeOf = model->material;
              auto M = modelM;
              auto PV = P * camera->getV(cameraM);

              // vertex shader uniforms

              // fragment shader uniforms

            };

          model->model->bind();
          model->shader->bind(uniformFn);
          model->model->draw();
          model->shader->unbind();
          model->model->unbind();
        }
    };
}
