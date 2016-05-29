#include "Scene.hpp"

std::shared_ptr<SkyBox> skyBox;

const char * rightPath = "res/sky/bkg2_right1.png";
const char * leftPath = "res/sky/bkg2_left2.png";
const char * topPath = "res/sky/bkg2_top3.png";
const char * bottomPath = "res/sky/bkg2_bottom4.png";
const char * backPath = "res/sky/bkg2_back6.png";
const char * frontPath = "res/sky/bkg2_front5.png";

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
                                    frontPath,
                                    backPath);

  return std::make_shared<Transform>(skyBox, skyBoxScaleMat);
}

static std::shared_ptr<Group> makeGround()
{
  auto seed = getRandomSeed();

  std::cerr << "Seed used: " << seed << std::endl;

  LandscapeBuilder builder(seed);

  return builder.finalize();
}

std::shared_ptr<Group> getScene(std::shared_ptr<Transform> withCamera)
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
