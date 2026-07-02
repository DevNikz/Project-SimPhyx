#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory>

using namespace std;

enum CameraType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

CameraType cameraType = ORTHOGRAPHIC;

#ifndef PERSPECTIVE_CAM_H

class PerspectiveCamera : public Physics::Camera
{
public:
    PerspectiveCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = Physics::YAW,
        float pitch = Physics::PITCH)
        : Camera(position, up, yaw, pitch)
    {
        updateCameraVectors();
    }
};

#endif

#ifndef ORTHOGRAPHIC_CAM_H

class OrthographicCamera : public Physics::Camera
{
public:
    OrthographicCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = Physics::YAW,
        float pitch = Physics::PITCH)
        : Camera(position, up, yaw, pitch)
    {
        updateCameraVectors();
    }
};

#endif

PerspectiveCamera perspectiveCam(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
OrthographicCamera orthoCam(glm::vec3(0.f, 0.0f, 1.f));

float windowWidth = 800.f;
float windowHeight = 800.f;
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

//Global variables
float deltaTime = 0.f;
float lastFrame = 0.f;
bool allowLightControl = false;
bool perspectiveSwitch = false;
bool cameraSwitch = false;
bool tankMoved = false;

float orbitYawOffset = 180.f;
float orbitPitch = 25.f;
const float sensitivity = 0.1f;

glm::vec3 orbitTarget = glm::vec3(0.f, 0.f, 0.f);
bool simulationPaused = false;

const float START_VIEW_HALF_HEIGHT = 800.f;
const float PERSPECTIVE_FOV = 45.f;

float orbitRadius =
START_VIEW_HALF_HEIGHT / tan(glm::radians(PERSPECTIVE_FOV * 0.5f));

// Third-person shoulder camera state
glm::vec3 modelPosition = glm::vec3(0.f, 0.f, 10.f);
float modelYaw = 0.f;
float firstPersonYaw = 0.f;
float firstPersonPitch = 0.f;
float firstPersonFOV = 45.f;

// Shoulder camera offsets (tweak these to taste)
const float CAM_DIST = 8.f;
const float CAM_HEIGHT = 2.5f;
const float CAM_SIDE = 0.0f;

glm::vec3 orthoCamPosition = glm::vec3(0.f, 60.f, 0.f);

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 color;
    float strength;

    float ambientStr;
    glm::vec3 ambientColor;

    float specularStr;
    float specularPhong;

    float constant;
    float linear;
    float quadratic;
    float range;

    PointLight(glm::vec3 position = glm::vec3(0.f), glm::vec3 color = glm::vec3(1.f),
        float strength = 1.f, float ambientStr = 0.1f, glm::vec3 ambientColor = glm::vec3(1.f),
        float specularStr = 0.5f, float specularPhong = 32.f, float constant = 1.f,
        float linear = 0.09f, float quadratic = 0.032f, float range = 2.f)
        : position(position), color(color), strength(strength),
        ambientStr(ambientStr), ambientColor(ambientColor),
        specularStr(specularStr), specularPhong(specularPhong),
        constant(constant), linear(linear), quadratic(quadratic), range(range)
    {
    }

    void Apply(Physics::Shader& shader, const std::string& name = "pointLight")
    {
        shader.passPointLight(name, position, color, strength,
            ambientStr, ambientColor, specularStr,
            specularPhong, constant, linear, quadratic,
            range);
    }
};

class DirectionalLight
{
public:
    glm::vec3 direction;
    glm::vec3 color;
    float strength;

    float ambientStr;
    glm::vec3 ambientColor;

    float specularStr;
    float specularPhong;

    DirectionalLight(glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f), glm::vec3 color = glm::vec3(1.f),
        float strength = 1.f, float ambientStr = 0.1f, glm::vec3 ambientColor = glm::vec3(1.f),
        float specularStr = 0.5f, float specularPhong = 32.f)
        : direction(direction), color(color), strength(strength),
        ambientStr(ambientStr), ambientColor(ambientColor),
        specularStr(specularStr), specularPhong(specularPhong)
    {
    }

    void Apply(Physics::Shader& shader, const std::string& name = "dirLight")
    {
        shader.passDirLight(name, direction, color, strength,
            ambientStr, ambientColor, specularStr, specularPhong);
    }
};

PointLight pointLight(
    glm::vec3(0.f, 3.f, 5.f), //Pos
    glm::vec3(1.0f, 1.0f, 1.0f),
    2.5f, //strength
    0.1f, //ambientstr
    glm::vec3(1.f, 1.f, 1.f), //ambientColor
    0.3f, //specularStr
    32.0f, //specularPhong
    1.0f, //constant
    0.007f, //linear
    0.0002f, //quadratic
    15.0f //range
);

DirectionalLight dirLight(
    glm::vec3(-0.2f, -1.0f, -0.6f), // direction: high angle, slightly angled like a moon
    glm::vec3(0.6f, 0.7f, 1.0f),    // color: cool blue-white moonlight
    0.2f,                            // strength: dim, moon is much weaker than sun
    //1.f,
    0.08f,                            // ambientStr: dark night ambient
    glm::vec3(0.05f, 0.05f, 0.15f), // ambientColor: deep blue night tint
    0.1f,                             // specularStr: subtle glint
    64.f                              // specularPhong
);

void updateOrbitCameras()
{
    float yaw = glm::radians(orbitYawOffset);
    float pitch = glm::radians(orbitPitch);

    glm::vec3 orbitOffset;
    orbitOffset.x = orbitRadius * cos(pitch) * sin(yaw);
    orbitOffset.y = orbitRadius * sin(pitch);
    orbitOffset.z = orbitRadius * cos(pitch) * cos(yaw);

    perspectiveCam.Position = orbitTarget + orbitOffset;
    perspectiveCam.Front = glm::normalize(orbitTarget - perspectiveCam.Position);
    perspectiveCam.Right = glm::normalize(glm::cross(perspectiveCam.Front, perspectiveCam.WorldUp));
    perspectiveCam.Up = glm::normalize(glm::cross(perspectiveCam.Right, perspectiveCam.Front));

    orthoCam.Position = perspectiveCam.Position;
    orthoCam.Front = perspectiveCam.Front;
    orthoCam.Right = perspectiveCam.Right;
    orthoCam.Up = perspectiveCam.Up;
}

void processInput(GLFWwindow* window)
{
    static bool onePress = false;
    static bool twoPress = false;
    static bool fPress = false;
    static bool spacePressed = false;
    static int lightIntensity = 0;
    static CameraType prevMode = ORTHOGRAPHIC;

    const float lightIntensities[] = { 2.5f, 5.f, 7.5f };

    const float moveSpeed = 20.f;
    const float turnSpeed = 90.f;

    const float fpTurnSpeed = 60.f;
    const float fpMoveSpeed = 10.f;

    const float panSpeed = 20.f;

    //Exit Game
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Play/pause sim
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!spacePressed) {
            simulationPaused = !simulationPaused;
            //threader->TogglePaused();
        }
        spacePressed = true;
    }
    else spacePressed = false;

    //Ortho or perspective
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        cameraType = ORTHOGRAPHIC;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        cameraType = PERSPECTIVE;

    //Point light
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!fPress)
        {
            lightIntensity = (lightIntensity + 1) % 3;
            pointLight.strength = lightIntensities[lightIntensity];
        }
        fPress = true;
    }
    else fPress = false;


    //Orbit cam
    const float orbitSpeed = 60.f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        orbitYawOffset -= orbitSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        orbitYawOffset += orbitSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        orbitPitch += orbitSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        orbitPitch -= orbitSpeed * deltaTime;

    if (orbitPitch < -85.f) orbitPitch = -85.f;
    if (orbitPitch > 85.f) orbitPitch = 85.f;

    updateOrbitCameras();
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos;
//
//    lastX = xpos;
//    lastY = ypos;
//
//    switch (cameraType) {
//    case THIRDPERSON:
//
//        orbitYawOffset += xoffset * sensitivity;
//        orbitPitch += yoffset * sensitivity;
//
//        if (orbitPitch < 5.f)  orbitPitch = 5.f;
//        if (orbitPitch > 80.f) orbitPitch = 80.f;
//
//        perspectiveCam.ProcessMouseMovement(orbitYawOffset, orbitPitch);
//        break;
//    }
//}

#ifndef SKYBOX_H
float skyboxVertices[]{
    -1.f, -1.f, 1.f, //0
    1.f, -1.f, 1.f,  //1
    1.f, -1.f, -1.f, //2
    -1.f, -1.f, -1.f,//3
    -1.f, 1.f, 1.f,  //4
    1.f, 1.f, 1.f,   //5
    1.f, 1.f, -1.f,  //6
    -1.f, 1.f, -1.f  //7
};

//Skybox Indices
unsigned int skyboxIndices[]{
    1,2,6, //R
    6,5,1,

    0,4,7, //L
    7,3,0,

    4,5,6, //T
    6,7,4,

    0,3,2, //B
    2,1,0,

    0,1,5, //Front
    5,4,0,

    3,7,6, //Back
    6,2,3
};

class Skybox {
private:
    GLuint skyVAO, skyboxTex;

public:
    string right, left, top, bottom, front, back;

    Skybox(string _right, string _left, string _top, string _bottom, string _front, string _back);
    void InitSky();
    void DrawSky();
    void InitTextures();
};

Skybox::Skybox(string _right, string _left, string _top, string _bottom, string _front, string _back) {
    right = "Skybox/" + _right + ".png";
    left = "Skybox/" + _left + ".png";
    top = "Skybox/" + _top + ".png";
    bottom = "Skybox/" + _bottom + ".png";
    front = "Skybox/" + _front + ".png";
    back = "Skybox/" + _back + ".png";
}

void Skybox::InitTextures() {
    std::string faceSky[]{
        right,
        left,
        top,
        bottom,
        front,
        back
    };

    //unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //if 3d this, else only s and t
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCh;
        stbi_set_flip_vertically_on_load(false);

        unsigned char* skybytes = stbi_load(faceSky[i].c_str(), &w, &h, &skyCh, 0);

        if (skybytes) {
            GLenum format = (skyCh == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, skybytes);
            stbi_image_free(skybytes);
        }
        else {
            std::cout << "Failed to load skybox face: " << faceSky[i] << std::endl;
            stbi_image_free(skybytes);
        }

    }
}

void Skybox::DrawSky() {
    glBindVertexArray(skyVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Skybox::InitSky() {
    unsigned int skyVBO, skyEBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glGenBuffers(1, &skyEBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(skyboxVertices),
        &skyboxVertices,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(int) * 36,
        &skyboxIndices,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);
}
#endif
struct ParticleResult {
    std::string name;
    glm::vec3 color;
    int rank = 0;
    float finalSpeed = 0.f;
    glm::vec3 avgVelocity = glm::vec3(0.f);
    float time = 0.f;
    bool finished = false;

    glm::vec3 startPos;
    float elapsedTime = 0.f;
};

bool AtCenter(Particle& p, ParticleResult& r, float t, float threshold = 5.f) {
    r.elapsedTime += t;

    if (glm::length(glm::vec2(p.Position.x, p.Position.y)) <= threshold) {
        p.Position.x = 0.f;
        p.Position.y = 0.f;
        return true;
    }
    else return false;
}

struct ParticleConfig {
    glm::vec3 position = glm::vec3(0.f, -700, 0.f);
    glm::vec3 velocity = glm::vec3(0.f);
    glm::vec3 accel = glm::vec3(1.f, 1.f, 1.f);
    float damping = 0.9f;
};

struct FountainParticle {
    RenderParticle* rp;
    Particle* p;
    float age;
    bool alive;
};

struct CableConfig {
    glm::vec3 position;
};

int spawnedCount = 0;
float spawnTimer = 0.f;
//spawn tick
const float spawnInterval = 0.025f;


std::mt19937 rng(42); // seed for reproducibility
bool spaceWasDown = false;

void PollInput(GLFWwindow* window, PhysicsThreadManager* threader) {
    //Play/pause sim

    bool spaceDown = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spaceDown && !spaceWasDown)      // rising edge only
        threader->TogglePaused();
    spaceWasDown = spaceDown;
}

//MAIN
int main(void)
{
    //constexpr std::chrono::nanoseconds timestep(6944444);
    constexpr std::chrono::nanoseconds timestep(16666666);

    //Random Gen Inits
    std::vector<FountainParticle> fountainParticles;
    std::uniform_real_distribution<float> colorGen(0.1f, 1.f);
    std::uniform_real_distribution<float> massGen(2.5f, 5.f);
    std::uniform_real_distribution<float> dampGen(0.3f, 0.9f);
    std::uniform_real_distribution<float> forceXGen(-50000.f, 50000.f);
    std::uniform_real_distribution<float> forceYGen(100000.f, 200000.f);
    std::uniform_real_distribution<float> lifespanGen(1.f, 10.f);
    std::uniform_real_distribution<float> scaleGen(2.f, 10.f);

    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 8);

    window = glfwCreateWindow(windowWidth, windowHeight, "Group 5 | SimPhyx (Phase1)", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Camera instances
    orthoCam.Projection = glm::ortho(
        -800.f,
        800.f,
        -800.f,
        800.f,
        -5000.f,
        5000.f
    );

    perspectiveCam.Projection = glm::perspective(
        glm::radians(PERSPECTIVE_FOV),
        windowWidth / windowHeight,
        0.1f,
        5000.f
    );
    updateOrbitCameras();

    //Load Shader
    Shader unlit("Shaders/unlit.vert", "Shaders/unlit.frag");
    Shader lineShader("Shaders/lineShader.vert", "Shaders/lineShader.frag");

    std::list<RenderParticle*> RenderParticles;
    auto pWorld = std::make_unique<PhysicsWorld>();
    float edge = 750.f;

    //Load Model
    auto sphereModel = std::make_unique<Model>("sphere", "", "");
    sphereModel->InitModel();
    sphereModel->AssignShader(&unlit);

    std::vector<unique_ptr<Particle>> cableParticles;
    std::vector<unique_ptr<Model>> lines;
    std::vector<CableConfig> cableLines;

    const float lineLength = 300.f;
    const float particleRadius = 35.f;
    const float seperator = 100.f;
    const float gravityMod = 15.f;
    const float mass = 50.f;
    const float rest = 0.9f;

    pWorld->ModifyGravity(gravityMod);

    //Spawn Particles
    for (int i = 0; i < 5; i++) {
        glm::vec3 color = { 0.5f, 0.f, 0.f };
        glm::vec3 anchor = glm::vec3(-i * seperator + 300.f, 250.f, 0.f);

        auto p = std::make_unique<Particle>();
        p->Position = glm::vec3(-i * seperator + 300.f, 200.f, 0.f);
        p->mass = mass;
        p->restitution = rest;
        p->radius = particleRadius;
        p->useGravity = true;
        
        //Renderer
        RenderParticle* rp = new RenderParticle(p.get(), sphereModel.get(), color, glm::vec3(particleRadius));
        RenderParticles.push_back(rp);
        
        //Cable
        Cable* cb = new Cable(p.get(), anchor, lineLength, 0.25f);
        
        //Cable* cable = new Cable(anchor, mass / 2 + 1.5f, lineLength, 5.f);
        //pWorld->forceRegistry.Add(p.get(), cable);

        //CableList
        cableLines.push_back({anchor});
        pWorld->AddParticle(p.get()); //Add to physics world
        pWorld->Cables.push_back(cb);
        cableParticles.push_back(move(p));

        //pWorld->Cables.push_back(chain.get());
    };

    for (int i = 0; i < 5; i++) {
        //Line
        auto line = std::make_unique<Model>();
        line->InitLine(cableLines[i].position, cableParticles[i]->Position);
        line->AssignShader(&lineShader);
        lines.push_back(move(line));

    };

    //Enable anti-aliasing and blend
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        //Normal Update
        processInput(window);

        
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        float framesec = dur.count() / 1E09f;
        deltaTime = framesec;
        

        //Spawn Particle per 0.025s tick | Spacebar for pasuing / resuming sim
        if (!simulationPaused) {
            
            //Physics Update Here
            curr_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
            if (curr_ns >= timestep) {
                constexpr float timestep_sec = timestep.count() / (float)(1E09);
                curr_ns -= timestep;

                //Physics Update
                pWorld->Update(timestep_sec);
            }
            
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unlit.use();
        unlit.passOrthoCamera(orthoCam);

        ////Switching camera using 1 or 2 keybind
        //if (cameraType == ORTHOGRAPHIC)
        //    unlit.passOrthoCamera(orthoCam);
        //else
        //    unlit.passPerspectiveCamera(perspectiveCam, orbitTarget);

        //Draw Particles. Auto disables if its "dead"
        for (auto* rp : RenderParticles) rp->Draw();

        lineShader.use();
        lineShader.passOrthoCamera(orthoCam);
        
        for (int i = 0; i < lines.size(); i++)
            lines[i]->DrawLine(cableLines[i].position, cableParticles[i]->Position);

    }

    sphereModel->DeleteBuffers();

    //Terminate gl
    glfwTerminate();
    return 0;
}

