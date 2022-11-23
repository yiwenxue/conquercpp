#include <concepts>
#include <cstdint>
#include <iostream>
#include <stdint.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <variant>

struct ImageData {
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t format;
  uint32_t usage;
};

std::ostream &operator<<(std::ostream &os, const ImageData &data) {
  os << "width: " << data.width << ", height: " << data.height
     << ", depth: " << data.depth << ", format: " << data.format
     << ", usage: " << data.usage;
  return os;
}
std::istream &operator>>(std::istream &is, ImageData &data) {
  std::string token;
  is >> token >> data.width >> token >> data.height >> token >> data.depth >>
      token >> data.format >> token >> data.usage;
  return is;
}

struct BufferData {
  uint32_t size;
  uint32_t usage;
};
std::ostream &operator<<(std::ostream &os, const BufferData &data) {
  os << "size: " << data.size << ", usage: " << data.usage;
  return os;
}
std::istream &operator>>(std::istream &is, BufferData &data) {
  std::string token;
  is >> token >> data.size >> token >> data.usage;
  return is;
}

struct GPUResource {
  uint32_t handle;
};

struct Edge {
  std::string src;
  std::string dst;
};

struct EdgeRecorder {
  std::unordered_map<std::string, std::vector<Edge>> edges;
  void record(const std::string &src, const std::string &dst) {
    edges[src].push_back({src, dst});
  }
};

EdgeRecorder edge_recorder;

struct NamingPool {
  static std::string getName(const std::string &name) {
    uint32_t index = 0;
    auto it = mapping.find(name);
    if (it == mapping.end()) {
      mapping[name] = index;
    } else {
      index = it->second++;
    }
    return name + std::to_string(index);
  }

  static void reset() { mapping.clear(); }

private:
  static std::unordered_map<std::string, uint32_t> mapping;
};
std::unordered_map<std::string, uint32_t> NamingPool::mapping;

template <typename T>
concept ResourceData =
    std::is_same_v<T, ImageData> || std::is_same_v<T, BufferData>;

template <ResourceData T> struct ResourceNode {
  const T data;
  const std::string name;
  ResourceNode(const T &data, const std::string &name)
      : data(data), name(name) {}

  ResourceNode(const ResourceNode &other)
      : data(other.data), name(other.name) {}

  ResourceNode &operator=(const ResourceNode &other) {
    if (this != &other) {
      new (this) ResourceNode(other); // placement new
    }
    return *this;
  }
};

template <ResourceData T> struct IResourceNode : public ResourceNode<T> {
  using ResourceNode<T>::ResourceNode;
  const GPUResource resource;

  IResourceNode(const T &data, const std::string &name,
                const GPUResource &resource)
      : ResourceNode<T>(data, name), resource(resource) {}
};

// functor that can be used to construct relationships between nodes
template <typename T, typename U> struct Functor {
  using input_type = T;
  using output_type = U;
  virtual U operator()(const T &input) = 0;
};

// load functor
template <ResourceData T>
struct LoadFunctor : public Functor<T, IResourceNode<T>> {
  using Functor<T, IResourceNode<T>>::Functor;
  IResourceNode<T> operator()(const T &input) override {
    std::cout << "Loading resource " << input << std::endl;
    // TODO (yiwenxue): implement loading
    return IResourceNode<T>(input, NamingPool::getName("load"), {0});
  }
};

// store functor
template <ResourceData T>
struct StoreFunctor : public Functor<IResourceNode<T>, void> {
  using Functor<IResourceNode<T>, void>::Functor;
  void operator()(const IResourceNode<T> &input) override {
    std::cout << "Storing resource " << input.data << std::endl;
    // TODO (yiwenxue): implement store
  }
};

/**
    // deferred rendering
    const auto colorMap = loadTexture();
    const auto normalMap = loadTexture();
    const auto depthMap = loadTexture();

    // deferred shading, G-buffer pass
    const auto {position, normal, albedo, specular} = deferredShading(
        colorMap, normalMap, depthMap);

    // lighting pass
    const auto lightMap = lightingPass(position, normal, albedo, specular);

    // post processing
    const auto finalImage = postProcessing(lightMap);

    // render to screen
    renderToScreen(finalImage);
 */
using deferredShadingInputs =
    std::tuple<ResourceNode<ImageData>, ResourceNode<ImageData>,
               ResourceNode<ImageData>>;
using deferredShadingOutputs =
    std::tuple<IResourceNode<ImageData>, IResourceNode<ImageData>,
               IResourceNode<ImageData>, IResourceNode<ImageData>>;
struct DeferredShadingFunctor
    : public Functor<deferredShadingInputs, deferredShadingOutputs> {
  deferredShadingOutputs
  operator()(const deferredShadingInputs &input) override {
    std::cout << "deferred shading" << std::endl;
    // TODO (yiwenxue): implement deferred shading
    return std::make_tuple(
        IResourceNode<ImageData>(std::get<0>(input).data,
                                 NamingPool::getName("position"), {0}),
        IResourceNode<ImageData>(std::get<0>(input).data,
                                 NamingPool::getName("normal"), {0}),
        IResourceNode<ImageData>(std::get<0>(input).data,
                                 NamingPool::getName("albedo"), {0}),
        IResourceNode<ImageData>(std::get<0>(input).data,
                                 NamingPool::getName("specular"), {0}));
  }
};

using lightingPassInputs =
    std::tuple<IResourceNode<ImageData>, IResourceNode<ImageData>,
               IResourceNode<ImageData>, IResourceNode<ImageData>>;
using lightingPassOutputs = std::tuple<IResourceNode<ImageData>>;
struct LightingPassFunctor
    : public Functor<lightingPassInputs, lightingPassOutputs> {
  lightingPassOutputs operator()(const lightingPassInputs &input) override {
    std::cout << "lighting pass" << std::endl;
    // TODO (yiwenxue): implement lighting pass
    return std::make_tuple(IResourceNode<ImageData>(
        std::get<0>(input).data, NamingPool::getName("lightMap"), {0}));
  }
};

using postProcessingInputs = std::tuple<IResourceNode<ImageData>>;
using postProcessingOutputs = std::tuple<IResourceNode<ImageData>>;
struct PostProcessingFunctor
    : public Functor<postProcessingInputs, postProcessingOutputs> {
  postProcessingOutputs operator()(const postProcessingInputs &input) override {
    std::cout << "post processing" << std::endl;
    // TODO (yiwenxue): implement post processing
    return std::make_tuple(IResourceNode<ImageData>(
        std::get<0>(input).data, NamingPool::getName("post"), {0}));
  }
};

using presentInput = std::tuple<IResourceNode<ImageData>>;
using presentOutput = void;
struct PresentFunctor : public Functor<presentInput, void> {
  void operator()(const presentInput &input) override {
    std::cout << "present" << std::endl;
    // TODO (yiwenxue): implement present
  }
};

/**
    // hair dynamics simulation
    const auto hairInitPos = loadBuffer();
    const auto hairInitVel = loadBuffer();
    const auto hairDensity = loadBuffer();
    const auto hairMass = loadBuffer();

    // first step: advection
    const auto { hairPos1 } = advection(hairInitPos, hairInitVel, hairMass);

    // second step: constraints and collision
    const auto { hairPos2, hairVel1 } = constraintsAndCollision(
        hairPos1, hairDensity, hairMass);

    // third step: solve internal forces
    const auto { hairPos3, hairVel2, hairDensity1 } = solveInternalForces(
        hairPos2, hairVel1, hairDensity, hairMass);

    // save results
    saveBuffer(hairPos3);
    saveBuffer(hairVel2);
    saveBuffer(hairDensity1);
 */

// surrogate resource data generator, fill data with random values
template <ResourceData T> T surrogateResourceData(const std::string &name) {
#define random32 static_cast<uint32_t>(rand())
  if constexpr (std::is_same_v<T, ImageData>) {
    return ImageData{random32, random32, random32, random32, random32};
  } else if constexpr (std::is_same_v<T, BufferData>) {
    return BufferData{random32, random32};
  }
}

auto deferredShading() -> void {
  // deferred rendering
  const auto colorMap =
      LoadFunctor<ImageData>()(surrogateResourceData<ImageData>("colorMap"));
  const auto normalMap =
      LoadFunctor<ImageData>()(surrogateResourceData<ImageData>("normalMap"));
  const auto depthMap =
      LoadFunctor<ImageData>()(surrogateResourceData<ImageData>("depthMap"));

  // deferred shading, G-buffer pass
  const auto [position, normal, albedo, specular] =
      DeferredShadingFunctor()(std::make_tuple(colorMap, normalMap, depthMap));

  // lighting pass
  const auto [lightMap] = LightingPassFunctor()(
      std::make_tuple(position, normal, albedo, specular));

  // post processing
  const auto [finalImage] = PostProcessingFunctor()(std::make_tuple(lightMap));

  // render to screen
  PresentFunctor()(std::make_tuple(finalImage));
}

auto hairSimulation() -> void {
  const auto hairInitPos = LoadFunctor<BufferData>()(
      surrogateResourceData<BufferData>("hairInitPos"));
}

int main(int argc, char *argv[]) {
  std::cout << "Hello, world!" << std::endl;

  return 0;
}