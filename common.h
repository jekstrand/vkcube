#include <sys/time.h>
#include <stdbool.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include <png.h>

#include <xcb/xcb.h>

#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>

#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_intel.h>

#include <gbm.h>

#include "esUtil.h"

#define MAX_NUM_IMAGES 4

struct vkcube_buffer {
   struct gbm_bo *gbm_bo;
   VkDeviceMemory mem;
   VkImage image;
   VkImageView view;
   VkFramebuffer framebuffer;
   uint32_t fb;
   uint32_t stride;
};

struct vkcube;

struct model {
   void (*init)(struct vkcube *vc);
   void (*render)(struct vkcube *vc, struct vkcube_buffer *b);
};

struct vkcube {
   struct model model;

   int fd;
   struct gbm_device *gbm_device;

   struct {
      xcb_connection_t *conn;
      xcb_window_t window;
      xcb_atom_t atom_wm_protocols;
      xcb_atom_t atom_wm_delete_window;
   } xcb;

   struct {
      struct wl_display *display;
      struct wl_compositor *compositor;
      struct xdg_shell *shell;
      struct wl_surface *surface;
      struct xdg_surface *xdg_surface;
   } wl;

   VkSwapchainKHR swap_chain;

   drmModeCrtc *crtc;
   drmModeConnector *connector;
   uint32_t width, height;

   VkInstance instance;
   VkPhysicalDevice physical_device;
   VkDevice device;
   VkRenderPass render_pass;
   VkQueue queue;
   VkPipelineLayout pipeline_layout;
   VkPipeline pipeline;
   VkDeviceMemory mem;
   VkBuffer buffer;
   VkDescriptorSet descriptor_set;
   VkFence fence;
   VkCommandPool cmd_pool;

   void *map;
   uint32_t vertex_offset, colors_offset, normals_offset;

   struct timeval start_tv;
   VkSurfaceKHR surface;
   VkFormat image_format;
   struct vkcube_buffer buffers[MAX_NUM_IMAGES];
   uint32_t image_count;
   int current;
};
