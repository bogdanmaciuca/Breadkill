#pragma once
#include <exception>
#include <string>
#include <vector>
#include <span>
#include "glm/glm.hpp"

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUShader;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUBuffer;
struct SDL_GPUTransferBuffer;
struct SDL_GPUTexture;
struct SDL_GPUCommandBuffer;
struct SDL_GPUSampler;

enum {
    VERTEX_BUFFER_SIZE = 8192,
    NUM_SAMPLERS = 0,
    NUM_UNIFORM_BUFFERS = 1
};

class RendererException : public std::exception {
public:
    RendererException(const std::string& message)
        : m_message("Renderer exception: " + message + "\n") {}
    virtual const char* what() const {
        return m_message.c_str();
    }
private:
    const std::string m_message;
};

struct RendererVertex {
    float x, y;
    float u, v;
    unsigned int texIdx;
};

struct RendererTriangle {
    RendererVertex points[3];
};

class Renderer {
public:
    Renderer(SDL_Window* pWindow, const std::span<const std::string>& texturePaths);
    ~Renderer();
    void RenderScene();
    void PushTriangle(const RendererTriangle& triangle);
private:
    std::vector<RendererTriangle> m_triangles;
    glm::mat4 m_projection;
    SDL_Window* m_pWindow;
    SDL_GPUDevice* m_pDevice;
    SDL_GPUGraphicsPipeline* m_pPipeline;
    SDL_GPUBuffer* m_pVertBuffer;
    std::vector<SDL_GPUTexture*> m_textures;
    SDL_GPUSampler* m_pSampler;

    enum class ShaderStage { Vertex, Fragment };

    SDL_GPUShader* LoadShader(const std::string& path, ShaderStage shaderStage, unsigned int num_samplers, unsigned int num_uniform_buffers);
    void InitPipeline(const std::string& vertexPath, const std::string& fragmentPath);
    SDL_GPUTexture* CreateTexture(SDL_GPUCommandBuffer* pCommandBuffer, const std::string& path);
};
