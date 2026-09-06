# Programming Assignment 1

## Due date: 25 September 2026, 23:59

Assignment 1 contains two independent programs. Part A introduces basic interactive drawing with
OpenGL immediate mode. Part B applies homogeneous transformation matrices in two focused 3D scenes.

## Build and run

Run all commands from the repository root `COMP3271_Computer_Graphics`:

```shell
xmake -y
xmake run assignment1_part_a
xmake run assignment1_part_b
```

## Part A — Interactive Triangle Drawing

### Goal

Collect points from mouse input and draw colored triangles using OpenGL immediate mode.

### Student tasks

Implement the following functions in `part_a/triangle_app.cpp`:

```cpp
void MouseInteraction(double x, double y);
void DrawTriangles();
```

`MouseInteraction` stores each clicked point. Every three points form a new triangle.
`DrawTriangles` displays incomplete points in white and completed triangles using the provided
color palette. Drawing must use `glBegin`, `glColor`, `glVertex`, and `glEnd`.

### Controls

- Left-click: add a point when the button is released.
- `0`: clear all points and triangles.
- `1`: load the provided four-triangle example.
- `Esc`: close the program.

## Part B — 3D Transformations

### Goal

Construct translation, rotation, and scaling matrices and combine them to transform objects in 3D.
Use the ImGui **Load Scene** menu to switch between two independent examples:

- **Object Transforms** contains a cube, sphere, cuboid, and pyramid with editable transforms.
- **Airplane & Quaternions** demonstrates gimbal lock and persistent quaternion rotations.

### Student tasks

Implement the marked functions in `part_b/math.cpp`:

```cpp
Mat4 MakeTranslationMatrix(const Vec3& translation);
Mat4 MakeRotationXMatrix(float radians);
Mat4 MakeRotationYMatrix(float radians);
Mat4 MakeRotationZMatrix(float radians);
Mat4 MakeScaleMatrix(const Vec3& scale);
Mat4 ComposeModelMatrixEuler(const Transform& transform);
Quat EulerToQuat(const Vec3& rotation_degrees);
Quat MakeAxisAngleQuat(const Vec3& axis, float degrees);
Quat QuatMultiply(const Quat& lhs, const Quat& rhs);
Quat QuatNormalize(const Quat& quat);
Mat4 MakeRotationMatrix(const Quat& quat);
Mat4 ComposeModelMatrixQuat(const Transform& transform);
Mat4 ComposeModelMatrixQuat(
    const Vec3& translation, const Quat& rotation, const Vec3& scale);
```

The matrix convention is:

- column-major storage: `values[column * 4 + row]`;
- column vectors;
- right-handed coordinates with Y as the up axis;
- rotation values in the Inspector are in degrees;
- Euler model matrix order: `T * Rz * Ry * Rx * S`, applied to an object as scale
  first, then rotation, then translation;
- quaternion model matrix order: `T * Q * S`, where `Q` is the rotation matrix built
  from `EulerToQuat(rotation_degrees)`. Both compose functions must produce the same
  rotation, and `EulerToQuat` must follow the same order as `Rz * Ry * Rx`
  (`q = qz * qy * qx`).

The Assignment 1 implementation must not use GLM or another matrix library. In particular, do not
call a library translation, rotation, scaling, look-at, or perspective helper.

### Object Transforms controls

- `Load Scene > Object Transforms`: load the object scene.
- Hierarchy item: select an object.
- Inspector fields: edit the selected object's position, rotation, and scale.
- Right mouse drag: orbit the camera.
- Mouse wheel: zoom the camera.
- `Reset Transform`: restore the selected object.
- `Reset Scene`: restore all objects.
- `Esc`: close the program.

### Airplane & Quaternions controls

Load the scene with `Load Scene > Airplane & Quaternions`. The airplane points along local +X in a
Y-up world. Its Euler path
therefore uses the aircraft order `Ry(yaw) * Rz(pitch) * Rx(roll)`, making pitch the middle angle.
This demo-specific order is separate from the Inspector's required `Rz * Ry * Rx` order.

To compare the representations:

1. Select `Euler compose`, press `Reset`, then press `Pull Up!` to animate pitch toward 90 degrees.
2. With the airplane pointing vertically, drag `Yaw` or `Roll`: it only spins about its nose axis,
   and its forward direction no longer changes.
3. The panel shows a `GIMBAL LOCK` warning when `|pitch|` is near 90 degrees.
4. Select `Quaternion update`, reset and repeat. The controls now apply incremental rotations to a
   quaternion retained across frames; at vertical pitch, yaw can still change the flight direction.

In the Euler path, yaw and roll collapse onto the same axis and one degree of freedom is lost.
Merely converting the three Euler values to a quaternion would preserve that singularity. The
quaternion path avoids it by storing the quaternion itself and post-multiplying local-axis delta
rotations instead of reconstructing the orientation from Euler angles.

## Requirements and submission

- Keep the provided function signatures and matrix convention.
- Scale components must remain positive; the editor limits each component to `[0.1, 5.0]`.
- Add concise comments explaining the matrix entries and multiplication order.
- Submit `triangle_app.cpp` for part a and `math.cpp` for part b to moodle
