#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory>

using namespace std;

enum CameraType {
    THIRDPERSON,
    FIRSTPERSON,
    TOPDOWN
};

CameraType cameraType = TOPDOWN;

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

std::mt19937 rng(42); // seed for reproducibility
std::uniform_real_distribution<float> dist(-25.f, 25.f);

PerspectiveCamera thirdPerson(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
PerspectiveCamera firstPerson(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
OrthographicCamera topDown(glm::vec3(0.f, 0.0f, 1.f));

float windowWidth = 700;
float windowHeight = 700;
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

glm::vec3 topDownPosition = glm::vec3(0.f, 60.f, 0.f);

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

void processInput(GLFWwindow* window)
{
    static bool onePress = false;
    static bool twoPress = false;
    static bool fPress = false;
    static int lightIntensity = 0;
    static CameraType prevMode = TOPDOWN;

    const float lightIntensities[] = { 2.5f, 5.f, 7.5f };

    const float moveSpeed = 20.f;
    const float turnSpeed = 90.f;

    const float fpTurnSpeed = 60.f;
    const float fpMoveSpeed = 10.f;

    const float panSpeed = 20.f;

    //Exit Game
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //ThirdPerson or FirstPerson
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!onePress && cameraType != TOPDOWN)
        {
            cameraType = (cameraType == THIRDPERSON) ? FIRSTPERSON : THIRDPERSON;
            prevMode = cameraType;
        }
        onePress = true;
    }
    else onePress = false;

    //Perspective or Ortho
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (!twoPress)
        {
            if (cameraType != TOPDOWN)
            {
                prevMode = cameraType; // save current before switching
                cameraType = TOPDOWN;
                topDownPosition = modelPosition + glm::vec3(0.f, 40.f, 0.f);
            }
            else
            {
                cameraType = prevMode; // return to previous
            }
        }
        twoPress = true;
    }
    else twoPress = false;

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


    //Move
    if (cameraType == THIRDPERSON)
    {
        // --- Rotate tank in place (A = turn left, D = turn right) -----------
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            modelYaw += turnSpeed * deltaTime;
            tankMoved = true;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            modelYaw -= turnSpeed * deltaTime;
            tankMoved = true;
        }

        // --- Compute tank's local forward vector from its current yaw -------
        glm::vec3 tankForward = glm::normalize(glm::vec3(
            cos(glm::radians(-modelYaw)),
            0.f,
            sin(glm::radians(-modelYaw))
        ));

        // --- Drive forward / backward (W / S) --------------------------------
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            modelPosition += tankForward * moveSpeed * deltaTime;
            tankMoved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            modelPosition -= tankForward * moveSpeed * deltaTime;
            tankMoved = true;
        }

        if (tankMoved)
        {
            firstPersonYaw = -modelYaw;
            firstPersonPitch = 0.f;
        }
    }
    else if (cameraType == FIRSTPERSON)
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            firstPersonYaw -= fpTurnSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            firstPersonYaw += fpTurnSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            firstPersonPitch += fpMoveSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            firstPersonPitch -= fpMoveSpeed * deltaTime;

        // Clamp pitch
        if (firstPersonPitch > 89.f) firstPersonPitch = 89.f;
        if (firstPersonPitch < -89.f) firstPersonPitch = -89.f;

        // Q/E - zoom in/out
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            firstPersonFOV -= fpMoveSpeed * deltaTime * 10.f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            firstPersonFOV += fpMoveSpeed * deltaTime * 10.f;

        if (firstPersonFOV < 10.f) firstPersonFOV = 10.f;
        if (firstPersonFOV > 120.f) firstPersonFOV = 120.f;

    }

    else {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            topDownPosition.x -= panSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            topDownPosition.x += panSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            topDownPosition.z -= panSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            topDownPosition.z += panSpeed * deltaTime;
    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    switch (cameraType) {
    case THIRDPERSON:

        orbitYawOffset += xoffset * sensitivity;
        orbitPitch += yoffset * sensitivity;

        if (orbitPitch < 5.f)  orbitPitch = 5.f;
        if (orbitPitch > 80.f) orbitPitch = 80.f;

        thirdPerson.ProcessMouseMovement(orbitYawOffset, orbitPitch);
        break;
    }
}

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


//MAIN
int main(void)
{
    constexpr std::chrono::nanoseconds timestep(6944444);


    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 8);

    window = glfwCreateWindow(windowWidth, windowHeight, "PC01 Paul Nikko Ragudo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress); // if using CMAKE
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Camera instances
    topDown.Projection = glm::ortho(
        -700.f, //L
        700.f, //R
        -700.f, //Bottom
        700.f, //Top
        -700.f, //Near
        700.f //Far
    );

    Shader noobShader("Shaders/noobShader.vert", "Shaders/noobShader.frag");

    std::list<RenderParticle*> RenderParticles;
    /*PhysicsWorld* pWorld = new PhysicsWorld();*/
    auto pWorld = std::make_unique<PhysicsWorld>();
    float edge = 700.f;

    //Skybox skybox("cubemap1_right", "cubemap1_left", "cubemap1_top", "cubemap1_bottom", "cubemap1_front", "cubemap1_back");
    //skybox.InitSky();
    //skybox.InitTextures();

    //Model shipmentObj = Model("Sci-fi Large container", "Sci-fi Container _Base_Color", ".png", "Sci-fi Container _Normal_OpenGL", ".png", "", "", "3D/");
    /*Model* sphereModel = new Model("sphere", "", "");*/
    auto sphereModel = std::make_unique<Model>("sphere", "", "");
    sphereModel->InitModel();
    sphereModel->Scale(glm::vec3(50.f));
    sphereModel->AssignShader(&noobShader);

    glm::vec3 v = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 f = glm::vec3(60000, 0, 0);
    glm::vec3 accel = glm::vec3(1.f, 0.f, 0.f);;

    //Red
    auto p1 = std::make_unique<Particle>();
    p1->Position = glm::vec3(-edge, 250.f, 201.f);
    p1->Velocity = v;
    p1->Acceleration = accel;
    p1->damping = 1.f;
    p1->ApplyForce(f);
    p1->useGravity = true;
    pWorld->AddParticle(p1.get());

    //Green
    auto p2 = std::make_unique<Particle>();
    p2->Position = glm::vec3(-edge, 0.f, 173.f);
    p2->Velocity = v;
    p2->Acceleration = accel;
    p2->damping = 0.5f;
    p2->ApplyForce(f);
    p2->useGravity = true;
    pWorld->AddParticle(p2.get());

    //Yellow
    auto p3 = std::make_unique<Particle>();
    p3->Position = glm::vec3(-edge, -250.f, -150.f);
    p3->Velocity = v;
    p3->Acceleration = accel;
    p3->damping = 0.25f;
    p3->ApplyForce(f);
    p3->useGravity = true;
    pWorld->AddParticle(p3.get());


    RenderParticle* rp1 = new RenderParticle(p1.get(), sphereModel.get(), glm::vec3(0.5f, 0.f, 0.f));
    RenderParticle* rp2 = new RenderParticle(p2.get(), sphereModel.get(), glm::vec3(0.f, 0.5f, 0.f));
    RenderParticle* rp3 = new RenderParticle(p3.get(), sphereModel.get(), glm::vec3(0.5f, 0.5f, 0.f));
    RenderParticles.push_back(rp1);
    RenderParticles.push_back(rp2);
    RenderParticles.push_back(rp3);

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

    ParticleResult results[4];
    results[0] = { "Red", glm::vec3(1,0,0), 0, 0, glm::vec3(0.f), 0, false, p1->GetPosition() };
    results[1] = { "Green", glm::vec3(0,1,0), 0, 0, glm::vec3(0.f), 0, false, p2->GetPosition() };
    results[2] = { "Blue", glm::vec3(0,0,1), 0, 0, glm::vec3(0.f), 0, false, p3->GetPosition() };
    //results[3] = { "Yellow", glm::vec3(1,1,0), 0, 0, glm::vec3(0.f), 0, false, p4->GetPosition() };

    int rankCounter = 1;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;

        auto checkFinish = [&](Particle& p, ParticleResult& r, float t, float threshold = 5.f) {
            if (r.finished) return;

            r.elapsedTime += t;

            if (glm::length(glm::vec2(p.Position.x, p.Position.y)) <= threshold) {
                p.Position.x = 0.f;
                p.Position.y = 0.f;

                r.finished = true;
                r.rank = rankCounter++;
                r.time = r.elapsedTime;
                r.finalSpeed = glm::length(p.Velocity);

                //float totalDist = glm::length(glm::vec2(r.startPos.x, r.startPos.y));
                glm::vec3 displacement = glm::vec3(0.f) - r.startPos;
                r.avgVelocity = displacement / r.time;

                p.Velocity = glm::vec3(0.f);
                p.Acceleration = glm::vec3(0.f);

                p.Destroy();
            }

            };

        curr_ns += dur;
        if (curr_ns >= timestep) {
            constexpr float timestep_sec = timestep.count() / (float)(1E09);
            curr_ns -= timestep;

            //Physics Update
            pWorld->Update(timestep_sec);

            /*
            checkFinish(*p1, results[0], timestep_sec);
            checkFinish(*p2, results[1], timestep_sec);
            checkFinish(*p3, results[2], timestep_sec);
            checkFinish(*p4, results[3], timestep_sec);
            */

            if (p1->IsDestroyed() && p2->IsDestroyed() && p3->IsDestroyed()) {
                glfwSetWindowShouldClose(window, true);
            }
        }

        //Normal Update

        processInput(window);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        noobShader.use();
        noobShader.passOrthoCamera(topDown);

        for (list<RenderParticle*>::iterator i = RenderParticles.begin(); i != RenderParticles.end(); i++) {
            (*i)->Draw();
        }

        /*
        //Skybox
        skyboxShader.use();
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.setInt("skybox", 0);
        if (cameraType == THIRDPERSON) skyboxShader.PassSkybox(thirdPerson, lookTarget);
        else skyboxShader.PassOrthoSkybox(topDown);


        skybox.DrawSky();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //EOF Skybox
        */



        //sphereModel.Position(particle.Position);
        //sphereModel.DrawModel();

        /*
        defaultShader.use();
        if (cameraType == THIRDPERSON) defaultShader.passPerspectiveCamera(thirdPerson, lookTarget);
        else defaultShader.passOrthoCamera(topDown);

        //Pass lights to shader
        pointLight.Apply(defaultShader, "pointLight");
        dirLight.Apply(defaultShader, "dirLight");

        //Floor
        glm::mat4 floor = glm::mat4(1.0f);
        floor = glm::translate(floor, glm::vec3(0.f, 0.f, 0.f));
        floor = glm::scale(floor, glm::vec3(1000.0f));
        defaultShader.setFloat("tiling", 50.f);
        defaultShader.setMat4("transform", 1, floor);
        defaultShader.setBool("useAlphaClip", false);
        defaultShader.LoadTexture(floorModel.GetDiffuse());
        defaultShader.LoadNormal(floorModel.GetNormal());
        floorModel.DrawModel();
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    sphereModel->DeleteBuffers();
    std::cout << std::fixed << std::setprecision(2);

    /*
    std::vector<ParticleResult*> sorted = { &results[0], &results[1], &results[2], &results[3] };
    std::sort(sorted.begin(), sorted.end(), [](ParticleResult* a, ParticleResult* b) {
        return a->rank < b->rank;
        });

    for (auto* r : sorted) {
        std::cout << "-----------------------------\n";
        std::cout << "Rank:              #" << r->rank << " - " << r->name << "\n";
        std::cout << "Final Speed:       " << r->finalSpeed << " m/s\n";
        std::cout << "Average Velocity:  ("
            << r->avgVelocity.x << " m/s, "
            << r->avgVelocity.y << " m/s, "
            << r->avgVelocity.z << " m/s)\n";
        std::cout << "Time to Center:    " << r->time << " s\n";
    }
    std::cout << "=============================\n";
    */

    //Terminate gl
    glfwTerminate();
    return 0;
}

