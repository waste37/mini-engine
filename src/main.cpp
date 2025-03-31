#include <WorldRegistry.h>
#include <Platform.h>
#include <Geometry.h>
#include <iostream>

#include <chrono>

class Timer {
public:
    Timer()
    {
        std::cout << "start stopwatch\n";
        beg = std::chrono::steady_clock::now();
    }
    ~Timer()
    {
        const auto dt = elapsed();
        std::cout << "elapsed: " << dt.count() / 1e+6 << "ms" << std::endl;
    }

    std::chrono::nanoseconds elapsed()
    {
        end = std::chrono::steady_clock::now();
        return std::chrono::nanoseconds(end - beg);
    }

    void reset() { beg = std::chrono::steady_clock::now(); }
private:
    std::chrono::steady_clock::time_point beg, end;
};


struct Position {
    f32 x, y;
};

struct Velocity {
    f32 dx, dy;
};

struct Health {
    i32 health;
    i32 armor;
};

void BasicBenchmark()
{
    WorldRegistry world{};
    world.Create();

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Velocity>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 10.0f + i;
        pos->y = 11.0f + i;

        Velocity* v = world.GetComponentData<Velocity>(e);
        v->dx = -10.0f + i;
        v->dy = -11.0f + i;
    }

    for (i32 i = 0; i < 10000; ++i) {
        Entity e = world.CreateEntity<Position, Health>();
        Position* pos = world.GetComponentData<Position>(e);
        pos->x = 20.0f + i;
        pos->y = 21.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    for (i32 i = 0; i < 1000; ++i) {
        Entity e = world.CreateEntity<Velocity, Health>();
        Velocity* pos = world.GetComponentData<Velocity>(e);
        pos->dx = 30.0f + i;
        pos->dy = 31.0f + i;
        Health* hp = world.GetComponentData<Health>(e);
        hp->health = 0;
    }

    {
        Timer t = Timer();
        for (WorldRegistry::ViewIterator it = world.View<Position>(); !it.AtEnd(); it.Next()) {
            Position* p = it.Get<Position>();
            p->x = 0.0f;
            p->y = 0.0f;
        }

        for (WorldRegistry::ViewIterator it = world.View<Health>(); !it.AtEnd(); it.Next()) {
            Health* p = it.Get<Health>();
            p->health++;
        }

        for (WorldRegistry::ViewIterator it = world.View<Position, Health>(); !it.AtEnd(); it.Next()) {
            Health* hp = it.Get<Health>();
            Position* p = it.Get<Position>();
            hp->health++;
            p->x = 0;
        }

        for (WorldRegistry::ViewIterator it = world.View<Velocity>(); !it.AtEnd(); it.Next()) {
            Velocity* p = it.Get<Velocity>();
            p->dx = 0.0;
            p->dy = 0.0;
        }
    }

    world.DebugRegisteredComponents();
    world.DebugRegisteredTypes();
    world.Destroy();
}

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

struct Vertex {
    Vec3 Position;
    Vec3 Normal;
    Vec2 UV;
};

struct Mesh {
    Vertex *Vertices;
    u32 *Indices;
    isize VertexCount;
    isize IndexCount;
};

struct ModelImportResult {
    Vector<Mesh> Meshes = {};
    b32 Ok = false;
};

Mesh ProcessMesh(const aiMesh *mesh) {
    Mesh m = {};
    m.VertexCount = mesh->mNumVertices;
    m.Vertices = new Vertex[mesh->mNumVertices]; 
    
    for (usize i = 0; i < mesh->mNumVertices; ++i) {
        m.Vertices[i].Position[0] = mesh->mVertices[i].x;
        m.Vertices[i].Position[1] = mesh->mVertices[i].y;
        m.Vertices[i].Position[2] = mesh->mVertices[i].z;

        m.Vertices[i].Normal[0] = mesh->mNormals[i].x;
        m.Vertices[i].Normal[1] = mesh->mNormals[i].y;
        m.Vertices[i].Normal[2] = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            m.Vertices[i].UV[0] = mesh->mTextureCoords[0][i].x;
            m.Vertices[i].UV[1] = mesh->mTextureCoords[0][i].y;
        } else {
            m.Vertices[i].UV = {0.0f, 0.0f};
        }
    }

    for (usize i = 0; i < mesh->mNumFaces; ++i) {
        m.IndexCount += mesh->mFaces[i].mNumIndices;
    }

    m.Indices = new u32[m.IndexCount];
    
    isize k = 0;
    for (usize i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (usize j = 0; j < face.mNumIndices; ++j) {
            m.Indices[k++] = face.mIndices[j];
        }
    }

    // std::cout << "mesh has material " << mesh->mMaterialIndex << std::endl;
    return m;
}

void ProcessNode(Vector<Mesh> *meshes, const aiNode *node, const aiScene *scene) 
{
    for (usize i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->Push(ProcessMesh(mesh));
    }

    for (usize i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(meshes, node->mChildren[i], scene);
    }
}

ModelImportResult ImportModel(const char *filename) {
    ModelImportResult result = {};
    // Create an instance of the Importer class..
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType); 
    if (nullptr == scene) {
        return result;
    }
    
    result.Meshes.Reserve(scene->mNumMeshes);
    ProcessNode(&result.Meshes, scene->mRootNode, scene);

    result.Ok = true;
    return result;
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

u32 CompileShader(const char *vertex_shader_path, const char *fragment_shader_path) 
{

    s8 vertex_shader_code = ReadFile(vertex_shader_path);
    s8 fragment_shader_code = ReadFile(fragment_shader_path);

    char *v = (char*)vertex_shader_code.Data;
    char *f = (char*)fragment_shader_code.Data;

    int vl = (int) vertex_shader_code.Length;
    int fl = (int) fragment_shader_code.Length;

    u32 program_id = glCreateProgram();

    u32 vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &v, &vl);
    glCompileShader(vertex_shader_id);

    char infoLog[512];
    b32 success;
    {
        success = true;
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };
    }


    glAttachShader(program_id, vertex_shader_id);
    u32 fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &f, &fl);
    glCompileShader(fragment_shader_id);

    {
        success = true;
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };
    }
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    {
        success = true;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(program_id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }

    return program_id;
}

void RenderSimpleScene(const Vector<Mesh> &meshes) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "test", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    u32 shader = CompileShader("assets/shaders/default_vertex.glsl", "assets/shaders/default_fragment.glsl");

    Vector<u32> VAOs(meshes.Size(), 0);
    Vector<u32> VBOs(meshes.Size(), 0);
    Vector<u32> EBOs(meshes.Size(), 0);

    glGenVertexArrays(meshes.Size(), &VAOs[0]);
    glGenBuffers(meshes.Size(), &VBOs[0]);
    glGenBuffers(meshes.Size(), &EBOs[0]);

    for (isize i = 0; i < meshes.Size(); ++i) {
        const Mesh &m = meshes[i];
        glBindVertexArray(VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, m.VertexCount * sizeof(Vertex), m.Vertices, GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.IndexCount * sizeof(u32), m.Indices, GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));

        glBindVertexArray(0);
    }

    glUseProgram(shader);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, -20.0f);
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, eye); 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    int modelLoc = glGetUniformLocation(shader, "ModelTransform");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(shader, "ViewTransform");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projectionLoc = glGetUniformLocation(shader, "ProjectionTransform");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    int viewvectorLoc = glGetUniformLocation(shader, "ViewVector");
    glUniform3fv(viewvectorLoc, 1, glm::value_ptr(eye));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        float time = glfwGetTime();
        glm::mat4 rotated = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rotated));

        for (isize i = 0; i < meshes.Size(); ++i) {
    
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, meshes[i].IndexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

i32 main()
{ 
    ModelImportResult result = ImportModel("assets/Mech_F_432/Material/mech_f_432.obj");
    if (result.Ok) {
        RenderSimpleScene(result.Meshes);
    }
}

