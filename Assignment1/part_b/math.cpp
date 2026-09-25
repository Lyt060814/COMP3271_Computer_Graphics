#include "math.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Vec3 operator*(const Vec3 &vector, float scalar) {
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

Vec3 operator/(const Vec3 &vector, float scalar) {
    if (std::abs(scalar) < 1.0e-8F) { throw std::runtime_error("Division by zero"); }
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

float Dot(const Vec3 &lhs, const Vec3 &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 Cross(const Vec3 &lhs, const Vec3 &rhs) {
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x,
    };
}

float Length(const Vec3 &vector) { return std::sqrt(Dot(vector, vector)); }

Vec3 Normalize(const Vec3 &vector) {
    const float length = Length(vector);
    if (length < 1.0e-8F) { return {}; }
    return vector / length;
}

float Radians(float degrees) {
    constexpr float pi = 3.14159265358979323846F;
    return degrees * pi / 180.0F;
}

Mat4 Mat4::Identity() {
    Mat4 result;
    result.at(0, 0) = 1.0F;
    result.at(1, 1) = 1.0F;
    result.at(2, 2) = 1.0F;
    result.at(3, 3) = 1.0F;
    return result;
}

float &Mat4::at(int row, int column) { return values[static_cast<std::size_t>(column * 4 + row)]; }

const float &Mat4::at(int row, int column) const {
    return values[static_cast<std::size_t>(column * 4 + row)];
}

const float *Mat4::data() const { return values.data(); }

Mat4 operator*(const Mat4 &lhs, const Mat4 &rhs) {
    Mat4 result;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            for (int k = 0; k < 4; ++k) {
                result.at(row, column) += lhs.at(row, k) * rhs.at(k, column);
            }
        }
    }
    return result;
}

Vec3 TransformPoint(const Mat4 &matrix, const Vec3 &point) {
    const float x = matrix.at(0, 0) * point.x + matrix.at(0, 1) * point.y
                    + matrix.at(0, 2) * point.z + matrix.at(0, 3);
    const float y = matrix.at(1, 0) * point.x + matrix.at(1, 1) * point.y
                    + matrix.at(1, 2) * point.z + matrix.at(1, 3);
    const float z = matrix.at(2, 0) * point.x + matrix.at(2, 1) * point.y
                    + matrix.at(2, 2) * point.z + matrix.at(2, 3);
    const float w = matrix.at(3, 0) * point.x + matrix.at(3, 1) * point.y
                    + matrix.at(3, 2) * point.z + matrix.at(3, 3);
    if (std::abs(w) < 1.0e-8F || std::abs(w - 1.0F) < 1.0e-8F) { return {x, y, z}; }
    return {x / w, y / w, z / w};
}

Mat4 MakeTranslationMatrix(const Vec3 &translation) {
    // ===== STUDENT_TASK_BEGIN: part_b_translation_matrix =====
    // Identity with the offset in the last column, so a point (x, y, z, 1)
    // becomes (x + tx, y + ty, z + tz, 1). Directions (w = 0) are unaffected.
    Mat4 result = Mat4::Identity();
    result.at(0, 3) = translation.x;
    result.at(1, 3) = translation.y;
    result.at(2, 3) = translation.z;
    return result;
    // ===== STUDENT_TASK_END: part_b_translation_matrix =====
}

Mat4 MakeRotationXMatrix(float radians) {
    // ===== STUDENT_TASK_BEGIN: part_b_rotation_x_matrix =====
    // Rotation about +X (right-handed, counter-clockwise looking down -X):
    // [1  0   0 ]
    // [0  c  -s ]   y' = c*y - s*z
    // [0  s   c ]   z' = s*y + c*z
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = Mat4::Identity();
    result.at(1, 1) = c;
    result.at(1, 2) = -s;
    result.at(2, 1) = s;
    result.at(2, 2) = c;
    return result;
    // ===== STUDENT_TASK_END: part_b_rotation_x_matrix =====
}

Mat4 MakeRotationYMatrix(float radians) {
    // ===== STUDENT_TASK_BEGIN: part_b_rotation_y_matrix =====
    // Rotation about +Y (up axis). Note the sign pattern differs from X/Z because
    // the cyclic order is Z -> X, i.e. x' = c*x + s*z and z' = -s*x + c*z:
    // [ c  0  s ]
    // [ 0  1  0 ]
    // [-s  0  c ]
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = Mat4::Identity();
    result.at(0, 0) = c;
    result.at(0, 2) = s;
    result.at(2, 0) = -s;
    result.at(2, 2) = c;
    return result;
    // ===== STUDENT_TASK_END: part_b_rotation_y_matrix =====
}

Mat4 MakeRotationZMatrix(float radians) {
    // ===== STUDENT_TASK_BEGIN: part_b_rotation_z_matrix =====
    // Rotation about +Z:
    // [c  -s  0]   x' = c*x - s*y
    // [s   c  0]   y' = s*x + c*y
    // [0   0  1]
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat4 result = Mat4::Identity();
    result.at(0, 0) = c;
    result.at(0, 1) = -s;
    result.at(1, 0) = s;
    result.at(1, 1) = c;
    return result;
    // ===== STUDENT_TASK_END: part_b_rotation_z_matrix =====
}

Mat4 MakeScaleMatrix(const Vec3 &scale) {
    // ===== STUDENT_TASK_BEGIN: part_b_scale_matrix =====
    // Scale factors on the diagonal: (x, y, z) -> (sx*x, sy*y, sz*z).
    Mat4 result = Mat4::Identity();
    result.at(0, 0) = scale.x;
    result.at(1, 1) = scale.y;
    result.at(2, 2) = scale.z;
    return result;
    // ===== STUDENT_TASK_END: part_b_scale_matrix =====
}

Quat EulerToQuat(const Vec3 &rotation_degrees) {
    // ===== STUDENT_TASK_BEGIN: part_b_euler_to_quat =====
    // Matches the matrix order Rz * Ry * Rx: build one quaternion per axis and
    // multiply them in the same order, q = qz * qy * qx (qx is applied first).
    const Quat qx = MakeAxisAngleQuat({1.0F, 0.0F, 0.0F}, rotation_degrees.x);
    const Quat qy = MakeAxisAngleQuat({0.0F, 1.0F, 0.0F}, rotation_degrees.y);
    const Quat qz = MakeAxisAngleQuat({0.0F, 0.0F, 1.0F}, rotation_degrees.z);
    return QuatNormalize(QuatMultiply(qz, QuatMultiply(qy, qx)));
    // ===== STUDENT_TASK_END: part_b_euler_to_quat =====
}

Quat MakeAxisAngleQuat(const Vec3 &axis, float degrees) {
    const Vec3 normalized_axis = Normalize(axis);
    if (Length(normalized_axis) < 1.0e-8F) { return {}; }

    const float half_angle = Radians(degrees) * 0.5F;
    const float sine = std::sin(half_angle);
    return QuatNormalize({
        std::cos(half_angle),
        normalized_axis.x * sine,
        normalized_axis.y * sine,
        normalized_axis.z * sine,
    });
}

Quat QuatMultiply(const Quat &lhs, const Quat &rhs) {
    // ===== STUDENT_TASK_BEGIN: part_b_quat_multiply =====
    // Hamilton product lhs * rhs (rhs is applied first when rotating a vector).
    // With q = (w, v): lhs * rhs = (w1*w2 - v1.v2, w1*v2 + w2*v1 + v1 x v2).
    return {
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
        lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
    };
    // ===== STUDENT_TASK_END: part_b_quat_multiply =====
}

Quat QuatNormalize(const Quat &quat) {
    // ===== STUDENT_TASK_BEGIN: part_b_quat_normalize =====
    // Divide by the 4D length so the quaternion represents a pure rotation.
    // A degenerate (near-zero) quaternion falls back to the identity rotation.
    const float length =
        std::sqrt(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
    if (length < 1.0e-8F) { return {}; }
    return {quat.w / length, quat.x / length, quat.y / length, quat.z / length};
    // ===== STUDENT_TASK_END: part_b_quat_normalize =====
}

Mat4 MakeRotationMatrix(const Quat &quat) {
    // ===== STUDENT_TASK_BEGIN: part_b_quat_to_matrix =====
    // Standard conversion of a unit quaternion (w, x, y, z) to a 3x3 rotation
    // matrix, placed in the upper-left block of a homogeneous 4x4 matrix:
    // [1-2(y2+z2)   2(xy-wz)     2(xz+wy)  ]
    // [2(xy+wz)     1-2(x2+z2)   2(yz-wx)  ]
    // [2(xz-wy)     2(yz+wx)     1-2(x2+y2)]
    const Quat q = QuatNormalize(quat);
    const float xx = q.x * q.x;
    const float yy = q.y * q.y;
    const float zz = q.z * q.z;
    const float xy = q.x * q.y;
    const float xz = q.x * q.z;
    const float yz = q.y * q.z;
    const float wx = q.w * q.x;
    const float wy = q.w * q.y;
    const float wz = q.w * q.z;

    Mat4 result = Mat4::Identity();
    result.at(0, 0) = 1.0F - 2.0F * (yy + zz);
    result.at(0, 1) = 2.0F * (xy - wz);
    result.at(0, 2) = 2.0F * (xz + wy);
    result.at(1, 0) = 2.0F * (xy + wz);
    result.at(1, 1) = 1.0F - 2.0F * (xx + zz);
    result.at(1, 2) = 2.0F * (yz - wx);
    result.at(2, 0) = 2.0F * (xz - wy);
    result.at(2, 1) = 2.0F * (yz + wx);
    result.at(2, 2) = 1.0F - 2.0F * (xx + yy);
    return result;
    // ===== STUDENT_TASK_END: part_b_quat_to_matrix =====
}

Mat4 ComposeModelMatrixEuler(const Transform &transform) {
    // ===== STUDENT_TASK_BEGIN: part_b_compose_model_matrix_euler =====
    // M = T * Rz * Ry * Rx * S. With column vectors the rightmost matrix acts
    // first: the object is scaled, then rotated about X, Y, Z, then translated.
    const Vec3 &r = transform.rotation_degrees;
    return MakeTranslationMatrix(transform.translation) * MakeRotationZMatrix(Radians(r.z))
           * MakeRotationYMatrix(Radians(r.y)) * MakeRotationXMatrix(Radians(r.x))
           * MakeScaleMatrix(transform.scale);
    // ===== STUDENT_TASK_END: part_b_compose_model_matrix_euler =====
}

Mat4 ComposeModelMatrixQuat(const Transform &transform) {
    // ===== STUDENT_TASK_BEGIN: part_b_compose_model_matrix_quat_transform =====
    // Same rotation as the Euler path, since EulerToQuat uses q = qz * qy * qx.
    return ComposeModelMatrixQuat(
        transform.translation, EulerToQuat(transform.rotation_degrees), transform.scale);
    // ===== STUDENT_TASK_END: part_b_compose_model_matrix_quat_transform =====
}

Mat4 ComposeModelMatrixQuat(const Vec3 &translation, const Quat &rotation, const Vec3 &scale) {
    // ===== STUDENT_TASK_BEGIN: part_b_compose_model_matrix_quat =====
    // M = T * Q * S: scale first, then rotate by the quaternion, then translate.
    return MakeTranslationMatrix(translation) * MakeRotationMatrix(rotation)
           * MakeScaleMatrix(scale);
    // ===== STUDENT_TASK_END: part_b_compose_model_matrix_quat =====
}

Mat4 MakePerspective(float vertical_fov_radians, float aspect, float near_plane, float far_plane) {
    Mat4 result;
    const float focal = 1.0F / std::tan(vertical_fov_radians * 0.5F);
    result.at(0, 0) = focal / std::max(aspect, 1.0e-6F);
    result.at(1, 1) = focal;
    result.at(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
    result.at(2, 3) = (2.0F * far_plane * near_plane) / (near_plane - far_plane);
    result.at(3, 2) = -1.0F;
    return result;
}

Mat4 MakeLookAt(const Vec3 &eye, const Vec3 &target, const Vec3 &up) {
    const Vec3 forward = Normalize(target - eye);
    const Vec3 right = Normalize(Cross(forward, up));
    const Vec3 camera_up = Cross(right, forward);

    Mat4 result = Mat4::Identity();
    result.at(0, 0) = right.x;
    result.at(0, 1) = right.y;
    result.at(0, 2) = right.z;
    result.at(0, 3) = -Dot(right, eye);
    result.at(1, 0) = camera_up.x;
    result.at(1, 1) = camera_up.y;
    result.at(1, 2) = camera_up.z;
    result.at(1, 3) = -Dot(camera_up, eye);
    result.at(2, 0) = -forward.x;
    result.at(2, 1) = -forward.y;
    result.at(2, 2) = -forward.z;
    result.at(2, 3) = Dot(forward, eye);
    return result;
}
