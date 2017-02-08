#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include "VkDeleter.class.hh"

using namespace std;

// Structs
struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete ()
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription ()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof ( Vertex );
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions ()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof ( Vertex, pos );

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof ( Vertex, color );

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof ( Vertex, texCoord );

		return attributeDescriptions;
	}
};

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
class Window
{

private:
	const std::vector<const char *> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };

	uint32_t xSize = 800;
	uint32_t ySize = 600;

	GLFWwindow *window;

	VkDeleter<VkInstance> instance{ vkDestroyInstance };
	VkDeleter<VkDebugReportCallbackEXT> callback{ instance, DestroyDebugReportCallbackEXT };
	VkDeleter<VkSurfaceKHR> surface{ instance, vkDestroySurfaceKHR };

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDeleter<VkDevice> device{ vkDestroyDevice };

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkDeleter<VkSwapchainKHR> swapChain{ device, vkDestroySwapchainKHR };
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkDeleter<VkImageView>> swapChainImageViews;
	std::vector<VkDeleter<VkFramebuffer>> swapChainFramebuffers;

	VkDeleter<VkRenderPass> renderPass{ device, vkDestroyRenderPass };
	VkDeleter<VkDescriptorSetLayout> descriptorSetLayout{ device, vkDestroyDescriptorSetLayout };
	VkDeleter<VkPipelineLayout> pipelineLayout{ device, vkDestroyPipelineLayout };
	VkDeleter<VkPipeline> graphicsPipeline{ device, vkDestroyPipeline };

	VkDeleter<VkCommandPool> commandPool{ device, vkDestroyCommandPool };

	VkDeleter<VkImage> depthImage{ device, vkDestroyImage };
	VkDeleter<VkDeviceMemory> depthImageMemory{ device, vkFreeMemory };
	VkDeleter<VkImageView> depthImageView{ device, vkDestroyImageView };

	VkDeleter<VkImage> textureImage{ device, vkDestroyImage };
	VkDeleter<VkDeviceMemory> textureImageMemory{ device, vkFreeMemory };
	VkDeleter<VkImageView> textureImageView{ device, vkDestroyImageView };
	VkDeleter<VkSampler> textureSampler{ device, vkDestroySampler };

	VkDeleter<VkBuffer> vertexBuffer{ device, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> vertexBufferMemory{ device, vkFreeMemory };
	VkDeleter<VkBuffer> indexBuffer{ device, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> indexBufferMemory{ device, vkFreeMemory };

	VkDeleter<VkBuffer> uniformStagingBuffer{ device, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> uniformStagingBufferMemory{ device, vkFreeMemory };
	VkDeleter<VkBuffer> uniformBuffer{ device, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> uniformBufferMemory{ device, vkFreeMemory };

	VkDeleter<VkDescriptorPool> descriptorPool{ device, vkDestroyDescriptorPool };
	VkDescriptorSet descriptorSet;

	std::vector<VkCommandBuffer> commandBuffers;

	VkDeleter<VkSemaphore> imageAvailableSemaphore{ device, vkDestroySemaphore };
	VkDeleter<VkSemaphore> renderFinishedSemaphore{ device, vkDestroySemaphore };


	const std::vector<Vertex> vertices =
	{ { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		 { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		 { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		 { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

		 { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		 { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		 { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		 { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } } };

	const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
	VkResult CreateDebugReportCallbackEXT ( VkInstance instance,
	                                        const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
	                                        const VkAllocationCallbacks *pAllocator,
	                                        VkDebugReportCallbackEXT *pCallback );

	void initWindow ();
	void initVulkan ();
	void mainLoop ();
	void recreateSwapChain ();
	void createInstance ();
	void setupDebugCallback ();
	void createSurface ();
	void pickPhysicalDevice ();
	void createLogicalDevice ();
	void createSwapChain ();
	void createImageViews ();
	void createRenderPass ();
	void createDescriptorSetLayout ();
	void createGraphicsPipeline ();
	void createFramebuffers ();
	void createCommandPool ();
	void createDepthResources ();
	VkFormat findSupportedFormat ( const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features );
	VkFormat findDepthFormat ();
	bool hasStencilComponent ( VkFormat format );
	void createTextureImage ();
	void createTextureImageView ();
	void createTextureSampler ();
	void createImageView ( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDeleter<VkImageView> &imageView );
	void createImage ( uint32_t width,
	                   uint32_t height,
	                   VkFormat format,
	                   VkImageTiling tiling,
	                   VkImageUsageFlags usage,
	                   VkMemoryPropertyFlags properties,
	                   VkDeleter<VkImage> &image,
	                   VkDeleter<VkDeviceMemory> &imageMemory );
	void transitionImageLayout ( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout );
	void copyImage ( VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height );
	void createVertexBuffer ();
	void createIndexBuffer ();
	void createUniformBuffer ();
	void createDescriptorPool ();
	void createDescriptorSet ();
	void createBuffer ( VkDeviceSize size,
	                    VkBufferUsageFlags usage,
	                    VkMemoryPropertyFlags properties,
	                    VkDeleter<VkBuffer> &buffer,
	                    VkDeleter<VkDeviceMemory> &bufferMemory );
	VkCommandBuffer beginSingleTimeCommands ();
	void endSingleTimeCommands ( VkCommandBuffer commandBuffer );
	void copyBuffer ( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size );
	uint32_t findMemoryType ( uint32_t typeFilter, VkMemoryPropertyFlags properties );
	void createCommandBuffers ();
	void createSemaphores ();
	void updateUniformBuffer ();
	void drawFrame ();
	void createShaderModule ( const std::vector<char> &code, VkDeleter<VkShaderModule> &shaderModule );
	VkSurfaceFormatKHR chooseSwapSurfaceFormat ( const std::vector<VkSurfaceFormatKHR> &availableFormats );
	VkPresentModeKHR chooseSwapPresentMode ( const std::vector<VkPresentModeKHR> availablePresentModes );
	VkExtent2D chooseSwapExtent ( const VkSurfaceCapabilitiesKHR &capabilities );
	SwapChainSupportDetails querySwapChainSupport ( VkPhysicalDevice device );
	bool isDeviceSuitable ( VkPhysicalDevice device );
	bool checkDeviceExtensionSupport ( VkPhysicalDevice device );
	QueueFamilyIndices findQueueFamilies ( VkPhysicalDevice device );
	std::vector<const char *> getRequiredExtensions ();
	bool checkValidationLayerSupport ();

	// Statics
	static std::vector<char> readFile ( const std::string &filename );
	static void onWindowResized ( GLFWwindow *window, int width, int height );
	static void DestroyDebugReportCallbackEXT ( VkInstance instance,
	                                            VkDebugReportCallbackEXT callback,
	                                            const VkAllocationCallbacks *pAllocator );
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback ( VkDebugReportFlagsEXT flags,
	                                                      VkDebugReportObjectTypeEXT objType,
	                                                      uint64_t obj,
	                                                      size_t location,
	                                                      int32_t code,
	                                                      const char *layerPrefix,
	                                                      const char *msg,
	                                                      void *userData );

public:
	void run ();

	// Setters
	void setXSize ( uint32_t s ) { this->xSize = s; }
	void setYSize ( uint32_t s ) { this->ySize = s; }
};

#endif // WINDOW_CLASS_H
