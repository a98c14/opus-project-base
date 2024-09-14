#pragma once
#include "defines.h"
#include <corecrt_math.h>

// NOTE(selim): Delete this if you don't wanna use SIMD
#define OPUS_USE_SSE
#ifdef OPUS_USE_SSE
#include <xmmintrin.h>
#endif

/* Macros */
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#define mix(a, b, t)   ((a * (1.0 - t) + b * t))
#define clamp(a, x, b) (((x) < (a)) ? (a) : ((b) < (x)) ? (b) \
                                                        : (x))
#define clamp_top(a, x) min(a, x)
#define clamp_bot(x, b) max(x, b)

/* Constants */
global float32 EPSILON_FLOAT32 = 1.1920929e-7f;
global float32 PI_FLOAT32      = 3.14159265359f;
global float32 TAU_FLOAT32     = 6.28318530718f;
// TODO: Find the actual min/max float values
global float32 FLOAT32_MIN = -1000000;
global float32 FLOAT32_MAX = 1000000;

global uint64 MAX_UINT64 = 0xffffffffffffffffull;
global uint32 MAX_UINT32 = 0xffffffff;
global uint16 MAX_UINT16 = 0xffff;
global uint8  MAX_UINT8  = 0xff;

global int64 MAX_INT64 = (int64)0x7fffffffffffffffull;
global int32 MAX_INT32 = (int32)0x7fffffff;
global int16 MAX_INT16 = (int16)0x7fff;
global int8  MAX_INT8  = (int8)0x7f;

global int64 MIN_INT64 = (int64)0xffffffffffffffffull;
global int32 MIN_INT32 = (int32)0xffffffff;
global int16 MIN_INT16 = (int16)0xffff;
global int8  MIN_INT8  = (int8)0xff;

/* Vectors */
typedef struct
{
    int32 x;
    int32 y;
} IVec2;
typedef struct
{
    int32 x;
    int32 y;
    int32 z;
} IVec3;
typedef struct
{
    int32 x;
    int32 y;
    int32 z;
    int32 w;
} IVec4;

typedef union
{
    struct
    {
        float32 x;
        float32 y;
    };
    struct
    {
        float32 u;
        float32 v;
    };
    struct
    {
        float32 w;
        float32 h;
    };
    float32 elements[2];
} Vec2;

typedef union
{
    struct
    {
        float32 x;
        float32 y;
        float32 z;
    };
    struct
    {
        float32 r;
        float32 g;
        float32 b;
    };
    struct
    {
        Vec2    xy;
        float32 _ignored;
    };
    float32 elements[3];
} Vec3;

typedef union
{
    struct
    {
        float32 x;
        float32 y;
        float32 z;
        float32 w;
    };
    struct
    {
        float32 r;
        float32 g;
        float32 b;
        float32 a;
    };
    struct
    {
        Vec3    xyz;
        float32 _ignored0;
    };
    struct
    {
        Vec2    xy;
        float32 _ignored1;
        float32 _ignored2;
    };
    float32 elements[4];
#ifdef OPUS_USE_SSE
    __m128 SSE;
#endif
} Vec4;

/* Matrices */
typedef union
{
    float32 v[4];
    float32 m[2][2];
    Vec2    columns[2];
} Mat2;

typedef union
{
    float32 v[9];
    float32 m[3][3];
    Vec3    columns[3];
} Mat3;

typedef union
{
    float32 v[16];
    float32 m[4][4];
    Vec4    columns[4];
} Mat4;

/* Misc */
// TODO(selim): delete this, use rect instead
typedef union
{
    struct
    {
        float32 left;
        float32 bottom;
        float32 right;
        float32 top;
    };
    struct
    {
        Vec2 bl;
        Vec2 tr;
    };
    Vec4 v;
} Bounds;

typedef union
{
    struct
    {
        float32 x;
        float32 y;
        float32 w;
        float32 h;
    };
    struct
    {
        Vec2 center;
        Vec2 size;
    };
    Vec4 v;
} Rect;

typedef struct
{
    Vec2    center;
    float32 radius;
} Circle;

typedef struct
{
    float32 min;
    float32 max;
} Range;

typedef struct
{
    int32 min;
    int32 max;
} IRange;

typedef enum
{
    QuadVertexIndexBottomLeft  = 0,
    QuadVertexIndexTopLeft     = 1,
    QuadVertexIndexBottomRight = 2,
    QuadVertexIndexTopRight    = 3,
    QuadVertexIndexCOUNT,
} QuadVertexIndex;

typedef enum
{
    QuadNormalIndexHorizontal = 0,
    QuadNormalIndexVertical   = 1,
    QuadNormalIndexCOUNT,
} QuadNormalIndex;

typedef struct
{
    Vec2 vertices[QuadVertexIndexCOUNT];
    Vec2 normals[QuadVertexIndexCOUNT];
} Quad;

/* Constructors */
internal Vec2 vec2(float32 x, float32 y);
internal Vec2 vec2_zero();
internal Vec2 vec2_one();
internal Vec2 vec2_right();
internal Vec2 vec2_up();
internal Vec3 vec3(float32 x, float32 y, float32 z);
internal Vec3 vec3_zero();
internal Vec3 vec3_one();
internal Vec3 vec3_x_y(float32 x, float32 y);
internal Vec3 vec3_xy(Vec2 xy);
internal Vec3 vec3_xy_z(Vec2 xy, float32 z);
internal Vec4 vec4(float32 x, float32 y, float32 z, float32 w);

internal Rect rect_from_xy_wh(float32 x, float32 y, float32 w, float32 h);
internal Rect rect_from_bl_tr(Vec2 bl, Vec2 tr);
internal Rect rect_from_wh(float32 w, float32 h);

internal Circle circle(Vec2 center, float32 radius);
internal Range  range(float32 min, float32 max);
internal IRange irange(int32 min, int32 max);

/* Basic Operations */
internal Vec2 add_vec2(Vec2 a, Vec2 b);
internal Vec2 add_vec2_f32(Vec2 a, float32 b);
internal Vec3 add_vec3(Vec3 a, Vec3 b);
internal Vec4 add_vec4(Vec4 a, Vec4 b);
internal Vec2 sub_vec2(Vec2 a, Vec2 b);
internal Vec3 sub_vec3(Vec3 a, Vec3 b);
internal Vec4 sub_vec4(Vec4 a, Vec4 b);
internal Vec2 mul_vec2_f32(Vec2 a, float32 b);
internal Vec2 mul_vec2(Vec2 a, Vec2 b);
internal Vec3 mul_vec3_f32(Vec3 a, float32 b);
internal Mat3 mul_mat3(Mat3 a, Mat3 b);
internal Vec2 mul_mat2_vec2(Mat2 m, Vec2 v);
internal Vec3 mul_mat3_vec3(Mat3 m, Vec3 v);
internal Mat4 mul_mat4(Mat4 a, Mat4 b);
internal Vec4 mul_mat4_vec4(Mat4 m, Vec4 v);
internal Vec2 div_vec2_f32(Vec2 a, float32 b);

internal float32 lerp_f32(float32 a, float32 b, float32 t);
internal Vec2    lerp_vec2(Vec2 a, Vec2 b, float32 t);
internal Vec3    lerp_vec3(Vec3 a, Vec3 b, float32 t);
internal Vec4    lerp_vec4(Vec4 a, Vec4 b, float32 t);
internal Rect    lerp_rect(Rect a, Rect b, float32 t);

// NOTE(selim): These can be used as a frame rate independent
// alternative to lerp smoothing. [Reference](https://youtu.be/LSNQuFEDOyQ)
const float32    _default_decay = 16;
internal float32 exp_decay_f32_(float32 a, float32 b, float32 decay, float32 dt);
internal Vec2    exp_decay_vec2_(Vec2 a, Vec2 b, float32 decay, float32 dt);
internal Vec3    exp_decay_vec3_(Vec3 a, Vec3 b, float32 decay, float32 dt);
internal float32 exp_decay_f32(float32 a, float32 b, float32 dt);
internal Vec2    exp_decay_vec2(Vec2 a, Vec2 b, float32 dt);
internal Vec3    exp_decay_vec3(Vec3 a, Vec3 b, float32 dt);

/* Vector Operations */
internal float32 dot_vec2(Vec2 a, Vec2 b);
internal float32 det_vec2(Vec2 a, Vec2 b);
internal float32 dot_vec3(Vec3 a, Vec3 b);
internal Vec2    fabs_vec2(Vec2 a);
internal Vec2    norm_vec2(Vec2 a);
internal Vec2    norm_vec2_safe(Vec2 a);
internal Vec3    norm_vec3(Vec3 a);
internal Vec2    clamp_vec2_length(float32 min, Vec2 a, float32 max);
internal Vec2    clamp_vec2(Vec2 a, Rect r);
internal float32 lensqr_vec2(Vec2 a);
internal float32 lensqr_vec3(Vec3 a);
internal float32 len_vec2(Vec2 a);
internal float32 len_vec3(Vec3 a);
internal float32 distsqr_vec2(Vec2 a, Vec2 b);
internal float32 dist_vec2(Vec2 a, Vec2 b);
internal float32 angle_vec2(Vec2 v);
internal float32 angle_between_vec2(Vec2 a, Vec2 b);
internal Vec2    rotate_vec2(Vec2 v, float angle);
internal Vec2    rotate90_vec2(Vec2 v);
internal Vec2    rotate90i_vec2(Vec2 v);
internal Vec2    heading_vec2(float32 angle);
internal Vec2    heading_inverse_vec2(float32 angle);
internal Vec2    direction_vec2(float32 angle, float32 scale);
internal Vec2    direction_inverse_vec2(float32 angle, float32 scale);
internal Vec2    heading_to_vec2(Vec2 start, Vec2 end);
internal Vec2    inverse_heading_to_vec2(Vec2 start, Vec2 end);
internal Vec2    direction_to_vec2(Vec2 start, Vec2 end, float32 scale);
internal Vec2    inverse_direction_to_vec2(Vec2 start, Vec2 end, float32 scale);
internal Vec2    move_towards_vec2(Vec2 from, Vec2 to, float32 length);
internal Vec2    move_vec2(Vec2 from, Vec2 heading, float32 length);

/** Quad */
internal Quad quad_at(Vec2 pos, Vec2 size, float32 rotation);
internal Quad quad_from_rect(Rect r, float32 rotation);

/* Matrix Operations*/
internal Mat2 mat2_identity(void);
internal Mat2 mat2_rotation(float angle);
internal Mat2 mat2_transpose(Mat2 m);

internal Mat3 mat3_identity(void);
internal Mat3 mat3_rotation(float angle);
internal Mat3 mat3_rotate(Mat3 m, float32 angle);
internal Mat3 mat3_transpose(Mat3 m);

internal Mat4 mat4_identity(void);
internal Mat4 mat4_translation(Vec3 v);
internal Mat4 mat4_inverse(Mat4 m);
internal Mat4 mat4_scale(Vec3 v);
internal Mat4 mat4_rotation(float angle);
internal Mat4 mat4_transpose(Mat4 m);
internal Mat4 mat4_transform(Mat4 translation, Mat4 rotation, Mat4 scale);
internal Mat4 mat4_ortho(float32 width, float32 height, float32 near_plane, float32 far_plane);

internal Vec4 linear_combine_v4_m4(Vec4 v, Mat4 m);

/* Utils */
internal Vec2 closest_point_on_line(Vec2 start, Vec2 end, Vec2 point);
internal Vec2 vec2_intersection_fast(Vec2 a, Vec2 heading_a, Vec2 b, Vec2 heading_b);
internal Vec2 vec2_scale(Vec2 a, float32 s);

internal float32 rect_left(Rect rect);
internal float32 rect_right(Rect rect);
internal float32 rect_bottom(Rect rect);
internal float32 rect_top(Rect rect);

internal Vec2 rect_bl(Rect rect);
internal Vec2 rect_br(Rect rect);
internal Vec2 rect_tr(Rect rect);
internal Vec2 rect_tl(Rect rect);
internal Vec2 rect_cl(Rect rect);
internal Vec2 rect_cr(Rect rect);
internal Vec2 rect_cb(Rect rect);
internal Vec2 rect_ct(Rect rect);

internal Vec2 bounds_bl(Bounds rect);
internal Vec2 bounds_br(Bounds rect);
internal Vec2 bounds_tr(Bounds rect);
internal Vec2 bounds_tl(Bounds rect);
internal Vec2 bounds_cl(Bounds rect);
internal Vec2 bounds_cr(Bounds rect);
internal Vec2 bounds_cb(Bounds rect);
internal Vec2 bounds_ct(Bounds rect);

internal Vec2   bounds_center(Bounds bounds);
internal Bounds bounds_shrink(Bounds bounds, Vec2 amount);
internal Bounds bounds_from_circle(Circle c);
internal Bounds bounds_from_rect(Rect r);

internal void b32_flip(bool32* b);

/** Rounding */
internal float32 floor_to(float32 v, float32 floor_to);
