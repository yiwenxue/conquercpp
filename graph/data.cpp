#include <optional>
#include <string>
#include <variant>
#include <vector>

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Object {};

struct Vec3 {};
struct Vec4 {};
struct Mat3 {};
struct Mat4 {};
struct Quaternion {};

struct Ray {
  Vec3 origin;
  Vec3 direction;
};

struct Triangle {
  Vec3 a;
  Vec3 b;
  Vec3 c;
};

struct Quad {
  Vec3 a;
  Vec3 b;
  Vec3 c;
  Vec3 d;
};

struct Plate {
  // xz plane
  float radius;
};

struct Plane {
  // xz plane
};

struct Sphere {
  // center at origin
  float radius;
};

struct Box {
  // center at origin, aligned with axes
  Vec3 extent;
};

struct Cylinder {
  // center at origin, aligned with y axis
  float radius;
  float height;
};

struct Cone {
  // center (height) at origin, aligned with y axis
  float radius;
  float height;
};

struct Torus {
  // center at origin, aligned with y axis
  float radius;
  float thickness;
};

struct Capsule {
  // center at origin, aligned with y axis
  float radius;
  float height;
};

struct Ellipsoid {
  // center at origin, aligned with axes
  Vec3 radius;
};

using Shape = std::variant<Triangle, Quad, Plate, Plane, Sphere, Box, Cylinder,
                           Cone, Torus, Capsule, Ellipsoid>;

std::optional<Vec3> rayIntersect(const Ray &ray, const Shape &shape) {
  auto res = std::nullopt;
  std::visit(overloaded{[&](const Triangle &t) {}, [&](const Quad &q) {},
                        [&](const Plate &p) {}, [&](const Plane &p) {},
                        [&](const Sphere &s) {}, [&](const Box &b) {},
                        [&](const Cylinder &c) {}, [&](const Cone &c) {},
                        [&](const Torus &t) {}, [&](const Capsule &c) {},
                        [&](const Ellipsoid &e) {}},
             shape);
  return res;
}

std::optional<Vec3> collide(const Shape &a, const Shape &b) {
  auto res = std::nullopt;
  std::visit(overloaded{
                 [](const Sphere &v1, const Sphere &v2) {},
                 [](const Sphere &v1, const Box &v2) {},
                 [](const Sphere &v1, const Cylinder &v2) {},
                 [](const Sphere &v1, const Cone &v2) {},
                 [](const Sphere &v1, const Torus &v2) {},
                 [](const Sphere &v1, const Capsule &v2) {},
                 [](const Sphere &v1, const Ellipsoid &v2) {},
                 [](const Box &v1, const Box &v2) {},
                 [](const Box &v1, const Cylinder &v2) {},
                 [](const Box &v1, const Cone &v2) {},
                 [](const Box &v1, const Torus &v2) {},
                 [](const Box &v1, const Capsule &v2) {},
                 [](const Box &v1, const Ellipsoid &v2) {},
                 [](const Cylinder &v1, const Cylinder &v2) {},
                 [](const Cylinder &v1, const Cone &v2) {},
                 [](const Cylinder &v1, const Torus &v2) {},
                 [](const Cylinder &v1, const Capsule &v2) {},
                 [](const Cylinder &v1, const Ellipsoid &v2) {},
                 [](const Cone &v1, const Cone &v2) {},
                 [](const Cone &v1, const Torus &v2) {},
                 [](const Cone &v1, const Capsule &v2) {},
                 [](const Cone &v1, const Ellipsoid &v2) {},
                 [](const Torus &v1, const Torus &v2) {},
                 [](const Torus &v1, const Capsule &v2) {},
                 [](const Torus &v1, const Ellipsoid &v2) {},
                 [](const Capsule &v1, const Capsule &v2) {},
                 [](const Capsule &v1, const Ellipsoid &v2) {},
                 [](const Ellipsoid &v1, const Ellipsoid &v2) {},
             },
             a, b);
  return res;
}

struct Rigid {
  float mass;
  Vec3 position;
  Quaternion rotation;
  Mat3 inertia;
  Vec3 velocity;
  Vec3 angularVelocity;
  Shape shape;
};

int main() {}