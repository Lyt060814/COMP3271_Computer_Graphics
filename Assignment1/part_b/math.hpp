#pragma once

#include <array>

struct Vec3 {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs);
Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs);
Vec3 operator*(const Vec3 &vector, float scalar);
Vec3 operator/(const Vec3 &vector, float scalar);
float Dot(const Vec3 &lhs, const Vec3 &rhs);
Vec3 Cross(const Vec3 &lhs, const Vec3 &rhs);
float Length(const Vec3 &vector);
Vec3 Normalize(const Vec3 &vector);
float Radians(float degrees);

struct Mat4 {
    std::array<float, 16> values{};

    static Mat4 Identity();
    float &at(int row, int column);
    const float &at(int row, int column) const;
    const float *data() const;
};

Mat4 operator*(const Mat4 &lhs, const Mat4 &rhs);
Vec3 TransformPoint(const Mat4 &matrix, const Vec3 &point);

struct Quat {
    float w = 1.0F;
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

Quat EulerToQuat(const Vec3 &rotation_degrees);
Quat MakeAxisAngleQuat(const Vec3 &axis, float degrees);
Quat QuatMultiply(const Quat &lhs, const Quat &rhs);
Quat QuatNormalize(const Quat &quat);
Mat4 MakeRotationMatrix(const Quat &quat);

struct Transform {
    Vec3 translation{};
    Vec3 rotation_degrees{};
    Vec3 scale{1.0F, 1.0F, 1.0F};
};

Mat4 MakeTranslationMatrix(const Vec3 &translation);
Mat4 MakeRotationXMatrix(float radians);
Mat4 MakeRotationYMatrix(float radians);
Mat4 MakeRotationZMatrix(float radians);
Mat4 MakeScaleMatrix(const Vec3 &scale);
Mat4 ComposeModelMatrixEuler(const Transform &transform);
Mat4 ComposeModelMatrixQuat(const Transform &transform);
Mat4 ComposeModelMatrixQuat(const Vec3 &translation, const Quat &rotation, const Vec3 &scale);

Mat4 MakePerspective(float vertical_fov_radians, float aspect, float near_plane, float far_plane);
Mat4 MakeLookAt(const Vec3 &eye, const Vec3 &target, const Vec3 &up);
