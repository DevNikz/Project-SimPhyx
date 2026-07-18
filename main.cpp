#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory>
#include <iomanip>
#include <imgui_impl_glfw.h>

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

float orbitYawOffset = -180.f;
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

    //orthoCam.Position = perspectiveCam.Position;
    //orthoCam.Front = perspectiveCam.Front;
    //orthoCam.Right = perspectiveCam.Right;
    //orthoCam.Up = perspectiveCam.Up;
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

struct ParticleConfig {
    float angleOffsetDeg;
    glm::vec3 color;
    string colorName;
    string reward;
};

struct LineConfig {
    glm::vec3 start;
    glm::vec3 end;
};

std::vector<ParticleConfig> particles
{
    {90.f, glm::vec3(1.f, 0.1f, 0.1f), "Red", "$5 Steam Gift Card"},
    {162.f, glm::vec3(1.f, 0.55f, 0.05f), "Orange", "RTX 5090"},
    {234.0f, glm::vec3(0.15f, 0.85f, 0.15f), "Green", "2x32 GB Ram"},
    {306.0f, glm::vec3(0.15f, 0.35f, 1.0f), "Blue", "Free Spin"},
    {18.0f, glm::vec3(0.65f, 0.10f, 0.85f), "Purple", "JACKPOT - Ace Combat 8: Wings of Thieve"},
};

//spawn tick
const float spawnInterval = 0.025f;


std::mt19937 rng(42); // seed for reproducibility
bool spaceWasDown = false;

static const int numParticles = 5;
static const float mass = 50.f;
static const float wheelRad = 500.0f;

static const float spinGain = 5.0f;
static const float maxClamp = 25.0f;

static const float friction = 0.5f;
static const float visDamp = 0.1f;
static const float stopEps = 0.02f;

float spinForceMag = 0.f;
static const float brakeStrength = 5.f;

Particle* hubParticle = nullptr;
float mainWheelOrbitRad = 0.f;

bool braking = false;
bool wasSpinning = false;
bool isSnapping = false;
float snapTargetRotZ = 0.f;
static const float snapSpeed = glm::radians(45.f);

//Result
bool stopped = false;
int rewardIndex = -1;

int GetTopIndex(float hubRotationRad)
{
    int best = 0;
    float bestY = -1e9f;
    for (int i = 0; i < (int)particles.size(); i++)
    {
        float angleRad = glm::radians(particles[i].angleOffsetDeg) + hubRotationRad;
        float y = sinf(angleRad); //world "up" component
        if (y > bestY) { bestY = y; best = i; }
    }
    return best;
}

void DisplayMenu() {
    cout << "==============================================" << endl;
    cout << "               ROULETTE                       " << endl;
    cout << "==============================================" << endl;
    for (int i = 0; i < particles.size(); i++) {
        cout << setw(8) << left << particles[i].colorName << " = " << particles[i].reward << endl;
    }
    cout << "\n----------------------------------------------\n";
    cout << "Apply Force: ";
    cin >> spinForceMag;
}

void ShowFPSOverlay(bool* p_open) {
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    if (corner != -1) {
        const float pad = 10.f;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 pos = viewport->WorkPos;
        ImVec2 size = viewport->Size;
        ImVec2 windowPos, windowPosPivot;

        windowPos.x = (corner & 1) ? (pos.x + size.x - pad) : (pos.x + pad);
        windowPos.y = (corner & 2) ? (pos.y + size.y - pad) : (pos.y + pad);
        windowPosPivot.x = (corner & 1) ? 1.0f : 0.0f;
        windowPosPivot.y = (corner & 2) ? 1.0f : 0.0f;

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
        windowFlags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowBgAlpha(0.35f);

    if (ImGui::Begin("FPS Overlay", p_open, windowFlags)) {

        ImGui::SeparatorText("Debug");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);

        ImGui::SeparatorText("Input");
        /*ImGui::Text("Braking: %d", braking);
        ImGui::Text("Stopped: %d", stopped);
        ImGui::Text("Speed: %.2f", hp->AngularVelocity.z);*/

        /*ImGui::SeparatorText("Particle");
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", p->Position.x, p->Position.y, p->Position.z);
        ImGui::Text("Rotation: (%.1f, %.1f, %.1f)", p->Rotation.x, p->Rotation.y, p->Rotation.z);*/
        

        //if (ImGui::BeginChild("Particle"), ImVec2(0, 0), ImGuiChildFlags_Borders) {
        //    ImGui::Text("Rotation: (%.1f, %.1f, %.1f)", rot.x, rot.y, rot.z);
        //}
        //ImGui::EndChild();

        /*
        if (ImGui::IsMousePosValid()) {
            ImGui::Text("Mouse: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
        }
        else {
            ImGui::Text("Mouse: <off screen>");
        }
        */

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Top-left", nullptr, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", nullptr, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", nullptr, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", nullptr, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) corner = -1;
            ImGui::EndPopup();
        }

        
    }

    ImGui::End();
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
    bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceDown) {
        if (!spacePressed) {
            cout << "Test" << endl;
        }
        spacePressed = true;
    }
    else {
        spacePressed = false;
    }

    //Ortho or perspective
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        cameraType = ORTHOGRAPHIC;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        cameraType = PERSPECTIVE;

    //Orbit cam
    const float orbitSpeed = 60.f;

    if (cameraType == PERSPECTIVE) {
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
    }
    updateOrbitCameras();
}

//MAIN
int main(void)
{
    //60fps Physics Update
    constexpr std::chrono::nanoseconds timestep(16666666);

    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 8);

    window = glfwCreateWindow(windowWidth, windowHeight, "PC02 Ragudo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Camera instances
    //Orthographic Projection
    orthoCam.Projection = glm::ortho(
        -800.f,
        800.f,
        -800.f,
        800.f,
        -5000.f,
        5000.f
    );

    //Perspective projection
    perspectiveCam.Projection = glm::perspective(
        glm::radians(PERSPECTIVE_FOV),
        windowWidth / windowHeight,
        0.1f,
        5000.f
    );
    updateOrbitCameras();

    //Load Shader
    Shader unlit("Shaders/unlit.vert", "Shaders/unlit.frag");
    Shader lit("Shaders/lit.vert", "Shaders/lit.frag");
    Shader lineShader("Shaders/lineShader.vert", "Shaders/lineShader.frag");
    Shader spriteShader("Shaders/spriteShader.vert", "Shaders/spriteShader.frag");

    std::list<RenderParticle*> RenderParticles;
    std::list<RenderParticle*> mRender;
    auto pWorld = std::make_unique<PhysicsWorld>();
    float edge = 750.f; //edge of the window

    //Load Model
    auto sphereModel = std::make_unique<Model>("sphere", "", &unlit);
    auto hubModel = std::make_unique<Model>("sphere", "", &unlit);

    Quad owlet;
    owlet.loadTexture("3D/Owlet_Monster_Idle_4.png", /*sheetColumns=*/4, /*sheetRows=*/1);
    owlet.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    owlet.setScale(glm::vec2(200.0f, 200.0f)); // pixel-art sprite, scaled up

    //Quad quad;
    //quad.useMipmaps = false;
    //quad.AssignShader(&spriteShader);
    //quad.Init();
    //quad.LoadDiffuse("3D/Owlet_Monster_Idle_4.png", true);
    //quad.Position(glm::vec3(0.f, 0.f, 0.f));
    //quad.Scale(glm::vec3(250.f, 250.f, 250.f));
    //quad.Rotation(glm::vec3(0.f, 0.f, 0.f));
    //quad.SetSpriteSheet(4, 1, 4, 8);

    //std::vector<unique_ptr<Particle>> rouletteParticles;
    //std::vector<unique_ptr<Line>> Lines;
    //std::vector<LineConfig> renderLines;

    const float particleRadius = 50.f;
    const float gravityMod = 1.f;
    const float rest = 0.9f;

    //pWorld->ModifyGravity(gravityMod);
    const float wheelOrbitRadius = wheelRad * 1.f;
    mainWheelOrbitRad = wheelOrbitRadius;

    //HUB
    //auto hp = std::make_unique<Particle>();
    //hp->Position = glm::vec3(0.f, 0.f, 0.f);
    //hp->mass = 10.0f;
    //hp->restitution = rest;
    //hp->radius = wheelRad;
    //hp->useGravity = false;

    //RenderParticle* rp = new RenderParticle(hp.get(), hubModel.get(), glm::vec3(0.25f, 0.f, 0.f), glm::vec3(wheelRad));
    //mRender.push_back(rp);
    //pWorld->AddParticle(hp.get());

    ////Center Particle
    //hubParticle = hp.get();

    //Spawn Particles
    /*
    for (int i = 0; i < 5; i++) {
        auto p = std::make_unique<Particle>();
        p->Position = glm::vec3(0.f, 0.f, 1.f);
        p->Rotation = glm::vec3(0.f);
        p->mass = mass;
        p->radius = particleRadius * 0.5f;
        p->useGravity = false;

        float angleRad = glm::radians(particles[i].angleOffsetDeg) + p->Rotation.z;
        p->Position = p->Position + mainWheelOrbitRad * glm::vec3(cosf(angleRad), sinf(angleRad), 0.f);
        

        RenderParticle* rp = new RenderParticle(p.get(), sphereModel.get(), particles[i].color, glm::vec3(p->radius));
        RenderParticles.push_back(rp);

        auto l = std::make_unique<Line>(glm::vec3(0.f), p->Position, &lineShader);
        rouletteParticles.push_back(move(p));
        Lines.push_back(move(l));
    }
    */

    //IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    //Enable anti-aliasing and blend
    glEnable(GL_MULTISAMPLE);
    //glBlendEquation(GL_FUNC_ADD);

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Normal Update
        processInput(window);

        //IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static bool show_overlay = true;
        if (show_overlay) {
            ShowFPSOverlay(&show_overlay);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
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
                //pWorld->Update(timestep_sec);

                
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Render here */

        //glDisable(GL_DEPTH_TEST);
        ////Draw Particles.
        //for (auto* rp : RenderParticles) {
        //    rp->Draw();
        //}
        //glEnable(GL_DEPTH_TEST);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        spriteShader.use();
        if (cameraType == ORTHOGRAPHIC)
            spriteShader.passOrthoCamera(orthoCam);
        else
            spriteShader.passPerspectiveCamera(perspectiveCam, orbitTarget);

        /*quad.UpdateAnimation(deltaTime);
        quad.Draw();*/
        owlet.updateAnimation(deltaTime, /*frameCount=*/4, /*frameDuration=*/0.15f);
        owlet.draw(spriteShader);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        

        //glDisable(GL_DEPTH_TEST);
        ////Line Shader
        //lineShader.use();
        //if (cameraType == ORTHOGRAPHIC)
        //    lineShader.passOrthoCamera(orthoCam);
        //else
        //    lineShader.passPerspectiveCamera(perspectiveCam, orbitTarget);
        //
        ////Render Line
        //for (int i = 0; i < Lines.size(); i++)
        //    Lines[i]->Draw(glm::vec3(0.f), rouletteParticles[i].get()->Position);
        //glEnable(GL_DEPTH_TEST);
    }

    sphereModel->DeleteBuffers();
    hubModel->DeleteBuffers();
    //quad.Destroy();
    //for (auto& l : lines) l->DeleteBuffers();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //Terminate gl
    glfwTerminate();

    return 0;
}

