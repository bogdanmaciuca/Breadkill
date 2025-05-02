#include "renderer.h"

#include <array>
#include <span>
#include <string>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Renderer::Renderer(SDL_Window* pWindow, const std::span<const std::string>& texturePaths) : m_pWindow(pWindow) {
    // Set up orthographic projection
    int wndWidth, wndHeight;
    SDL_GetWindowSize(pWindow, &wndWidth, &wndHeight);
    float projWidth = 10.0f;
    float projHeight = projWidth / (float)wndWidth * (float)wndHeight;
    m_projection = glm::ortho(0.0f, projWidth, projHeight, 0.0f, 0.0f, 100.0f);

    // GPU Device
    m_pDevice = SDL_CreateGPUDevice( SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (m_pDevice == nullptr)
        throw RendererException("Could not create device.");

    bool result = SDL_ClaimWindowForGPUDevice(m_pDevice, pWindow);
    if (result == false)
        throw RendererException("Could not claim window for device");

    // Vertex buffer
    SDL_GPUBufferCreateInfo vertBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size  = VERTEX_BUFFER_SIZE,
        .props = 0
    };
    m_pVertBuffer = SDL_CreateGPUBuffer(m_pDevice, &vertBufferCreateInfo);
    if (m_pVertBuffer == nullptr)
        throw RendererException("Could not create vertex buffer");

    // Sampler
    SDL_GPUSamplerCreateInfo samplerCreateInfo = {
        .min_filter = SDL_GPU_FILTER_LINEAR,
        .mag_filter = SDL_GPU_FILTER_LINEAR,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    };
    m_pSampler = SDL_CreateGPUSampler(m_pDevice, &samplerCreateInfo);
    if (m_pSampler == nullptr)
        throw RendererException("Could not create sampler");

    // Textures
    stbi_set_flip_vertically_on_load(true);
    SDL_GPUCommandBuffer* pCommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);
    for (const auto& path : texturePaths) {
        m_textures.push_back(CreateTexture(pCommandBuffer, path));
    }
    SDL_SubmitGPUCommandBuffer(pCommandBuffer);

    // Pipeline & shaders
    InitPipeline(
        "C:/Users/Bogdan/Documents/C_Projects/breadkill/shaders_compiled/shader.vert.spv",
        "C:/Users/Bogdan/Documents/C_Projects/breadkill/shaders_compiled/shader.frag.spv"
    );
}

Renderer::~Renderer() {
    SDL_ReleaseWindowFromGPUDevice(m_pDevice, m_pWindow);
    SDL_DestroyGPUDevice(m_pDevice);
}

void Renderer::RenderScene() {
    SDL_GPUCommandBuffer* pCommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);

    // Get swapchain texture and return early if window is minimized
    SDL_GPUTexture* pSwapchainTexture;
    bool result = SDL_WaitAndAcquireGPUSwapchainTexture(pCommandBuffer, m_pWindow, &pSwapchainTexture, nullptr, nullptr);
    if (result == false)
        throw RendererException("Could not acquire swapchain texture");
    if (pSwapchainTexture == nullptr) {
        SDL_SubmitGPUCommandBuffer(pCommandBuffer);
        return;
    }

    // Create transfer buffer with vertex data
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size  = (Uint32)(m_triangles.size() * sizeof(RendererTriangle)),
        .props = 0
    };
    SDL_GPUTransferBuffer* pTransferBuffer = SDL_CreateGPUTransferBuffer(m_pDevice, &transferBufferCreateInfo);
    if (pTransferBuffer == nullptr)
        throw RendererException("Could not create transfer buffer");

    void* pMappedData = (RendererTriangle*)SDL_MapGPUTransferBuffer(m_pDevice, pTransferBuffer, false);
    SDL_memcpy(pMappedData, m_triangles.data(), transferBufferCreateInfo.size);
    SDL_UnmapGPUTransferBuffer(m_pDevice, pTransferBuffer);

    // Transfer buffer -> vertex buffer
    SDL_GPUTransferBufferLocation transferBufferLocation = {
        .transfer_buffer = pTransferBuffer,
        .offset          = 0
    };
    SDL_GPUBufferRegion bufferRegion = {
        .buffer = m_pVertBuffer,
        .offset = 0,
        .size = transferBufferCreateInfo.size
    };
    SDL_GPUCopyPass* pCopyPass = SDL_BeginGPUCopyPass(pCommandBuffer);
    SDL_UploadToGPUBuffer(pCopyPass, &transferBufferLocation, &bufferRegion, false);
    SDL_EndGPUCopyPass(pCopyPass);

    // Render
    SDL_GPUColorTargetInfo colorTargetInfo = {
        .texture = pSwapchainTexture,
        .clear_color = SDL_FColor{ .r = 0.1f, .g = 0.15f, .b = 0.2f, .a = 1.0f },
        .load_op     = SDL_GPU_LOADOP_CLEAR,
        .store_op    = SDL_GPU_STOREOP_STORE
    };
    SDL_GPURenderPass* pRenderPass = SDL_BeginGPURenderPass(
        pCommandBuffer, &colorTargetInfo, 1, nullptr);

    SDL_BindGPUGraphicsPipeline(pRenderPass, m_pPipeline);
    SDL_PushGPUVertexUniformData(pCommandBuffer, 0, &m_projection, sizeof(m_projection));

    // Sampler bindings
    std::vector<SDL_GPUTextureSamplerBinding> samplerBindings;
    for (SDL_GPUTexture* pTexture : m_textures) {
        SDL_GPUTextureSamplerBinding binding = {
            .texture = pTexture,
            .sampler = m_pSampler
        };
        samplerBindings.push_back(binding);
    }
    SDL_BindGPUFragmentSamplers(pRenderPass, 0, samplerBindings.data(), m_textures.size());

    SDL_GPUBufferBinding bufferBinding = {
        .buffer = m_pVertBuffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(pRenderPass, 0, &bufferBinding, 1);
    SDL_DrawGPUPrimitives(pRenderPass, m_triangles.size() * 3, 1, 0, 0);
    SDL_EndGPURenderPass(pRenderPass);

    SDL_SubmitGPUCommandBuffer(pCommandBuffer);
    SDL_ReleaseGPUTransferBuffer(m_pDevice, pTransferBuffer);

    m_triangles.clear();
}

void Renderer::PushTriangle(const RendererTriangle& triangle) {
    m_triangles.push_back(triangle);
}

SDL_GPUShader* Renderer::LoadShader(const std::string& path, ShaderStage shaderStage, Uint32 num_samplers, Uint32 num_uniform_buffers) {
    size_t codeSize;
    Uint8* pCode = (Uint8*)SDL_LoadFile(path.c_str(), &codeSize);
    if (pCode == nullptr)
        throw RendererException("Could not read shader source");
    SDL_GPUShaderStage sdlShaderStage = 
        (shaderStage == ShaderStage::Vertex ?
         SDL_GPU_SHADERSTAGE_VERTEX : SDL_GPU_SHADERSTAGE_FRAGMENT);
    SDL_GPUShaderCreateInfo vertShaderCreateInfo = {
        .code_size            = codeSize,
        .code                 = pCode,
        .entrypoint           = "main",
        .format               = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage                = sdlShaderStage,
        .num_samplers         = num_samplers,
        .num_storage_textures = 0,
        .num_storage_buffers  = 0,
        .num_uniform_buffers  = num_uniform_buffers
    };

    SDL_GPUShader* pShader = SDL_CreateGPUShader(m_pDevice, &vertShaderCreateInfo);
    SDL_free(pCode);

    if (pShader == nullptr)
        throw RendererException("Could not create shader");

    return pShader;
}

void Renderer::InitPipeline(const std::string& vertexPath, const std::string& fragmentPath) {
    SDL_GPUShader* pVertShader = LoadShader(vertexPath, ShaderStage::Vertex, 0, 1);
    SDL_GPUShader* pFragShader = LoadShader(fragmentPath, ShaderStage::Fragment, m_textures.size(), 0);

    SDL_GPUColorTargetDescription colorTargetDesc = {};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_pDevice, m_pWindow);
    colorTargetDesc.blend_state.enable_blend = true;
    colorTargetDesc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDesc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDesc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDesc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDesc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDesc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    SDL_GPUVertexBufferDescription vertBufferDesc = {};
    vertBufferDesc.slot = 0;
    vertBufferDesc.pitch = sizeof(RendererVertex);
    vertBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    std::array<SDL_GPUVertexAttribute, 3> vertexAttribs = {
        SDL_GPUVertexAttribute{
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = 0
        },
        SDL_GPUVertexAttribute{
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = sizeof(float) * 2
        },
        SDL_GPUVertexAttribute{
            .location = 2,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
            .offset = sizeof(float) * 4
        }
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.target_info.num_color_targets                 = 1;
    pipelineCreateInfo.target_info.color_target_descriptions         = &colorTargetDesc;
    pipelineCreateInfo.vertex_input_state.num_vertex_buffers         = 1;
    pipelineCreateInfo.vertex_input_state.vertex_buffer_descriptions = &vertBufferDesc;
    pipelineCreateInfo.vertex_input_state.num_vertex_attributes      = vertexAttribs.size();
    pipelineCreateInfo.vertex_input_state.vertex_attributes          = vertexAttribs.data();
    pipelineCreateInfo.primitive_type                                = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineCreateInfo.vertex_shader                                 = pVertShader;
    pipelineCreateInfo.fragment_shader                               = pFragShader;

    m_pPipeline = SDL_CreateGPUGraphicsPipeline(m_pDevice, &pipelineCreateInfo);

    SDL_ReleaseGPUShader(m_pDevice, pVertShader);
    SDL_ReleaseGPUShader(m_pDevice, pFragShader);

    if (m_pPipeline == nullptr)
        throw RendererException("Could not create pipeline");
}

SDL_GPUTexture* Renderer::CreateTexture(SDL_GPUCommandBuffer* pCommandBuffer, const std::string& path) {
    SDL_GPUTexture* pTexture;

    int width, height;
    int numChannels;
    void* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, 4);
    //assert(numChannels == 4);

    SDL_GPUTextureCreateInfo createInfo = {
        .type   = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage  = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width  = (Uint32)width,
        .height = (Uint32)height,
        .layer_count_or_depth = 1,
        .num_levels = 1
    };
    pTexture = SDL_CreateGPUTexture(m_pDevice, &createInfo);
    if (pTexture == nullptr)
        throw RendererException("Could not create texture: " + path);

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size  = (Uint32)(width * height * 4),
        .props = 0
    };
    SDL_GPUTransferBuffer* pTransferBuffer = SDL_CreateGPUTransferBuffer(
        m_pDevice, &transferBufferCreateInfo);
    if (pTransferBuffer == nullptr)
        throw RendererException("Could not create transfer buffer");

    void* pMappedData = SDL_MapGPUTransferBuffer(m_pDevice, pTransferBuffer, false);

    //( (Uint32*)(pMappedData) )[pSurface->w * pSurface->h - 1] = 0;
    //SDL_Log("%x", ((Uint32*)(pSurface->pixels))[1200*1200 - 1000]);
    memcpy(pMappedData, pixels, width * height * 4);
    SDL_UnmapGPUTransferBuffer(m_pDevice, pTransferBuffer);

    stbi_image_free(pixels);

    SDL_GPUCopyPass* pCopyPass = SDL_BeginGPUCopyPass(pCommandBuffer);

    SDL_GPUTextureTransferInfo transferInfo = {
        .transfer_buffer = pTransferBuffer,
        .offset          = 0,
        .pixels_per_row  = 0,
        .rows_per_layer  = 0
    };

    SDL_GPUTextureRegion textureRegion = {
        .texture   = pTexture,
        .mip_level = 0,
        .layer     = 0,
        .x         = 0,
        .y         = 0,
        .w         = (Uint32)width,
        .h         = (Uint32)height,
        .d         = 1
    };

    SDL_UploadToGPUTexture(pCopyPass, &transferInfo, &textureRegion, false);

    SDL_EndGPUCopyPass(pCopyPass);
    SDL_ReleaseGPUTransferBuffer(m_pDevice, pTransferBuffer);

    return pTexture;
}
