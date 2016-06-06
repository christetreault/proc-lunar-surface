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

std::shared_ptr<LandscapeBuilder> builder = nullptr;
std::shared_ptr<Group> lsRoot = nullptr;

void permuteDoodads()
{
  lsRoot->clear();
  builder->permuteDoodads();
  lsRoot->insert(builder->landscape);
}

void permuteCity()
{
  lsRoot->clear();
  builder->permuteCity();
  lsRoot->insert(builder->landscape);
}

void permuteLandscape()
{
  lsRoot->clear();
  builder->permuteLandscape();
  lsRoot->insert(builder->landscape);
}

void swapLandscape()
{
  lsRoot->clear();
  builder->swapLandscape();
  lsRoot->insert(builder->landscape);
}

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

  builder = std::make_shared<LandscapeBuilder>(seed);
  builder->finalize();

  lsRoot = std::make_shared<Group>();
  lsRoot->insert(builder->landscape);
  std::cerr << "finalized" << std::endl;
  return lsRoot;
}

auto lightRotateFn = [](glm::mat4 & rotate, double time)
{
  static float tPrev = (float) time;

  if (time < 0.01) return;

  float tCurr = (float) time;
  float delta = (tCurr - tPrev) / 8.0f;

  rotate = glm::rotate(glm::mat4(),
                       delta,
                       glm::vec3(0.0f, 1.0f, 0.0f)) * rotate;
  tPrev = tCurr;
};

static std::shared_ptr<Transform> makeLight()
{
  auto light = std::make_shared<DirLight>(glm::vec3(0.0f, -5.0f, -25.0f),
                                          glm::vec3(1.0f, 0.8f, 0.8f));
  return std::make_shared<Transform>(light,
                                     glm::mat4(),
                                     lightRotateFn);
}

std::shared_ptr<Group> getScene(std::shared_ptr<Transform> withCamera)
{
  auto root = std::make_shared<Group>();

  root->insert(makeSky());
  root->insert(makeGround());
  root->insert(withCamera);
  root->insert(makeLight());

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
              auto lightPosDir = lightM * glm::vec4(light->dir.x,
                                                    light->dir.y,
                                                    light->dir.z,
                                                    0.0f);

              // vertex shader uniforms

              GLuint PVID = glGetUniformLocation(shaderProg, "PV");
              glUniformMatrix4fv(PVID, 1, GL_FALSE, &PV[0][0]);

              GLuint MID = glGetUniformLocation(shaderProg, "M");
              glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);

              // fragment shader uniforms

            GLuint lightPosDirID = glGetUniformLocation(shaderProg,
                                                        "lightDir");
            glUniform4f(lightPosDirID,
                        lightPosDir.x,
                        lightPosDir.y,
                        lightPosDir.z,
                        lightPosDir.w);

            GLuint lightColorID = glGetUniformLocation(shaderProg,
                                                       "lightColor");
            glUniform3f(lightColorID,
                        light->color.x,
                        light->color.y,
                        light->color.z);

            };

          model->shader->bind(uniformFn);
          model->draw();
          model->shader->unbind();
        }
            else if (typeid(*modelbase) == typeid(Segment))
        {
          auto model = std::dynamic_pointer_cast<Segment>(modelbase);
          auto uniformFn = [=](GLuint shaderProg)
            {
              auto M = modelM;
              auto V = camera->getV(cameraM);
              auto lightPosDir = lightM * glm::vec4(light->dir.x,
                                                    light->dir.y,
                                                    light->dir.z,
                                                    0.0f);
              auto cameraPos = camera->getPos(cameraM);

              // vertex shader uniforms

              GLuint PID = glGetUniformLocation(shaderProg, "P");
              glUniformMatrix4fv(PID, 1, GL_FALSE, &P[0][0]);

              GLuint VID = glGetUniformLocation(shaderProg, "V");
              glUniformMatrix4fv(VID, 1, GL_FALSE, &V[0][0]);

              GLuint MID = glGetUniformLocation(shaderProg, "M");
              glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);

              // fragment shader uniforms

              GLuint cameraPosID = glGetUniformLocation(shaderProg,
                                                        "cameraPos");
              glUniform3f(cameraPosID,
                          cameraPos.x,
                          cameraPos.y,
                          cameraPos.z);

              GLuint lightPosDirID = glGetUniformLocation(shaderProg,
                                                          "lightDir");
              glUniform4f(lightPosDirID,
                          lightPosDir.x,
                          lightPosDir.y,
                          lightPosDir.z,
                          lightPosDir.w);

              GLuint lightColorID = glGetUniformLocation(shaderProg,
                                                         "lightColor");
              glUniform3f(lightColorID,
                          light->color.x,
                          light->color.y,
                          light->color.z);

              GLuint timeID =glGetUniformLocation(shaderProg,
                                                  "time");
              glUniform1f(timeID,
                          fmod((float) glfwGetTime() / 2.0f,
                               2.0f * glm::pi<float>()));
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
      else if (typeid(*modelbase) == typeid(RoadNetwork)) {
        std::cout << "Drawing roads" << std::endl;
        auto model = std::dynamic_pointer_cast<RoadNetwork>(modelbase);
        auto uniformFn = [=](GLuint shaderProg) {
          auto M = modelM;
          auto PV = P * camera->getV(cameraM);

          // vertex shader uniforms

          GLuint PVID = glGetUniformLocation(shaderProg, "PV");
          glUniformMatrix4fv(PVID, 1, GL_FALSE, &PV[0][0]);

          GLuint MID = glGetUniformLocation(shaderProg, "M");
          glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
        };
        RoadNetwork::shader->bind(uniformFn);
        model->draw();
        RoadNetwork::shader->unbind();
      }
    };
}
