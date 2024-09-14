#include "intersection.h"

internal Ray2
ray(Vec2 start, Vec2 direction)
{
    Ray2 result;
    result.start     = start;
    result.direction = direction;
    return result;
}

internal Bounds
p_quad_get_bounds(Quad a)
{
    Bounds result = (Bounds){.bl = vec2(FLOAT32_MAX, FLOAT32_MAX), .tr = vec2(FLOAT32_MIN, FLOAT32_MIN)};
    for (uint32 i = 0; i < QuadVertexIndexCOUNT; i++)
    {
        result.bl.x = min(result.bl.x, a.vertices[i].x);
        result.bl.y = min(result.bl.y, a.vertices[i].y);
        result.tr.x = max(result.tr.x, a.vertices[i].x);
        result.tr.y = max(result.tr.y, a.vertices[i].y);
    }
    return result;
}

internal Intersection
intersects_rect_point(Rect a, Vec2 b)
{
    Intersection result;
    Vec2         bl = rect_bl(a);
    Vec2         tr = rect_tr(a);

    result.position   = b;
    result.intersects = !(b.x < bl.x || b.y < bl.y || b.x > tr.x || b.y > tr.y);
    return result;
}

internal Intersection
intersects_circle_point(Circle a, Vec2 b)
{
    Intersection result;
    result.position   = b;
    result.intersects = lensqr_vec2(sub_vec2(b, a.center)) < (a.radius / 2 * a.radius / 2);
    return result;
}

internal bool32
intersects_bounds_fast(Bounds a, Bounds b)
{
    return !(a.bl.x > b.tr.x || a.tr.x < b.bl.x || a.bl.y > b.tr.y || a.tr.y < b.bl.y);
}

internal bool32
intersects_rect_fast(Rect a, Rect b)
{
    return !(rect_left(a) > rect_right(b) || rect_right(a) < rect_left(b) || rect_bottom(a) > rect_top(b) || rect_top(a) < rect_bottom(b));
}

internal Intersection
intersects_circle(Circle a, Circle b)
{
    Vec2       normal = heading_to_vec2(a.center, b.center);
    Projection p_a    = project_circle(a, normal);
    Projection p_b    = project_circle(b, normal);

    Intersection result = {0};
    result.intersects   = projection_overlaps(p_a, p_b);
    if (result.intersects)
    {
        result.position = add_vec2(a.center, mul_vec2_f32(normal, projection_overlap_amount(p_a, p_b)));
        result.mtv      = minimum_translation_vector(normal, p_a, p_b, a.center, b.center);
    }
    return result;
}

internal Intersection
intersects_ray(Ray2 a, Ray2 b)
{
    Intersection result = {0};
    Vec2         d      = sub_vec2(b.start, a.start);
    float32      det    = det_vec2(b.direction, a.direction);
    if (fabs(det) < EPSILON_FLOAT32)
    {
        result.position = a.start;
        return result;
    }

    float32 u = (d.y * b.direction.x - d.x * b.direction.y) / det;
    float32 v = (d.y * a.direction.x - d.x * a.direction.y) / det;

    if (u < 0 || v < 0)
        return result;

    result.intersects = true;
    result.position   = add_vec2(a.start, mul_vec2_f32(a.direction, u));
    return result;
}

internal Intersection
intersects_rect(Rect a, Rect b)
{
    Intersection result   = {0};
    Vec2         normal_x = vec2(1, 0);
    Vec2         normal_y = vec2(0, 1);

    Projection projection_ax = project_rect(a, normal_x);
    Projection projection_bx = project_rect(b, normal_x);

    result.intersects = projection_overlaps(projection_ax, projection_bx);
    if (!result.intersects)
        return result;

    Projection projection_ay = project_rect(a, normal_y);
    Projection projection_by = project_rect(b, normal_y);
    result.intersects        = projection_overlaps(projection_ay, projection_by);
    if (!result.intersects)
        return result;

    bool32 x_smaller_than_y = projection_overlap_amount(projection_ax, projection_bx) < projection_overlap_amount(projection_ay, projection_by);
    result.mtv              = x_smaller_than_y
                                  ? minimum_translation_vector(normal_x, projection_ax, projection_bx, a.center, b.center)
                                  : minimum_translation_vector(normal_y, projection_ay, projection_by, a.center, b.center);

    result.position = lerp_vec2(a.center, b.center, 0.5);
    return result;
}

internal Intersection
intersects_polygon(P_Polygon a, P_Polygon b)
{
    Intersection result = {0};

    ArenaTemp temp         = scratch_begin(0, 0);
    int32     normal_count = a.normal_count + b.normal_count;
    Vec2*     normals      = arena_push_array(temp.arena, Vec2, normal_count);
    memory_copy_typed(normals, a.normals, a.normal_count);
    memory_copy_typed(normals + a.normal_count, b.normals, b.normal_count);

    Vec2    min_overlap_vector;
    float32 min_overlap_amount = FLOAT32_MAX;
    float32 min_sign;

    for (int32 i = 0; i < normal_count; i++)
    {
        Projection p1 = project_polygon(a, normals[i]);
        Projection p2 = project_polygon(b, normals[i]);
        if (!projection_overlaps(p1, p2))
            return result;

        float32 overlap_amount = projection_overlap_amount(p1, p2);

        float32 sign = (p1.max < p2.max ? -1 : 1);

        bool32 contains = (projection_contains(p1, p2) || projection_contains(p2, p1));
        if (contains && fabs(p1.min - p2.min) > fabs(p1.max - p2.max))
        {
            sign *= -1;
        }

        if (overlap_amount < min_overlap_amount)
        {
            min_overlap_vector = normals[i];
            min_overlap_amount = overlap_amount;
            min_sign           = sign;
        }
    }
    scratch_end(temp);

    result.intersects = true;
    result.mtv        = mul_vec2_f32(min_overlap_vector, min_overlap_amount * min_sign);

    // HACK(selim): I don't know how to get the contact points
    // so this i a buggy workaround for now.
    float32 min_d = FLOAT32_MAX;
    for (int32 i = 0; i < b.vertex_count; i++)
    {
        float32 d = dot_vec2(b.vertices[i], mul_vec2_f32(result.mtv, -1));
        if (d < min_d)
        {
            min_d           = d;
            result.position = b.vertices[i];
        }
    }

    return result;
}

internal Intersection
intersects_quad(Quad a, Quad b)
{
    P_Polygon ap;
    ap.vertices     = a.vertices;
    ap.vertex_count = 4;
    ap.normals      = a.normals;
    ap.normal_count = 2;

    P_Polygon bp;
    bp.vertices     = b.vertices;
    bp.vertex_count = 4;
    bp.normals      = b.normals;
    bp.normal_count = 2;

    return intersects_polygon(ap, bp);
}

internal Intersection
intersects_circle_rect(Circle a, Rect b)
{
    Intersection result = {0};

    bool32 should_check_corner = true;
    Vec2   normal_x            = vec2(1, 0);
    Vec2   normal_y            = vec2(0, 1);

    Projection projection_ax = project_circle(a, normal_x);
    Projection projection_bx = project_rect(b, normal_x);
    float32    dx            = dot_vec2(a.center, normal_x);
    should_check_corner      = should_check_corner && !projection_overlaps_point(projection_bx, dx);
    result.intersects        = projection_overlaps(projection_ax, projection_bx);
    if (!result.intersects)
        return result;

    Projection projection_ay = project_circle(a, normal_y);
    Projection projection_by = project_rect(b, normal_y);
    float32    dy            = dot_vec2(a.center, normal_y);
    should_check_corner      = should_check_corner && !projection_overlaps_point(projection_by, dy);
    result.intersects        = projection_overlaps(projection_ay, projection_by);
    if (!result.intersects)
        return result;

    if (should_check_corner)
    {
        Vec2       closest      = closest_point_rect(b, a.center);
        Vec2       n            = heading_to_vec2(closest, a.center);
        Projection projection_a = project_circle(a, n);
        Projection projection_b = project_rect(b, n);

        result.intersects = projection_overlaps(projection_a, projection_b);
        if (!result.intersects)
            return result;

        result.mtv = minimum_translation_vector(n, projection_a, projection_b, a.center, b.center);
    }
    else
    {
        bool32 x_smaller_than_y = projection_overlap_amount(projection_ax, projection_bx) < projection_overlap_amount(projection_ay, projection_by);
        result.mtv              = x_smaller_than_y
                                      ? minimum_translation_vector(normal_x, projection_ax, projection_bx, a.center, b.center)
                                      : minimum_translation_vector(normal_y, projection_ay, projection_by, a.center, b.center);
    }

    result.position = lerp_vec2(a.center, b.center, 0.5);
    return result;
}

internal Projection
project_circle(Circle c, Vec2 line)
{
    Projection result;
    float32    dot = dot_vec2(c.center, line);
    result.min     = dot - c.radius / 2.0f;
    result.max     = dot + c.radius / 2.0f;
    return result;
}

internal Projection
project_bounds(Bounds b, Vec2 line)
{
    Projection result;
    float32    d0 = dot_vec2(b.bl, line);
    float32    d1 = dot_vec2(vec2(b.left, b.top), line);
    float32    d2 = dot_vec2(b.tr, line);
    float32    d3 = dot_vec2(vec2(b.right, b.bottom), line);

    result.min = min(min(d0, d1), min(d2, d3));
    result.max = max(max(d0, d1), max(d2, d3));
    return result;
}

internal Projection
project_rect(Rect r, Vec2 line)
{
    Projection result;
    float32    d0 = dot_vec2(rect_bl(r), line);
    float32    d1 = dot_vec2(rect_tl(r), line);
    float32    d2 = dot_vec2(rect_tr(r), line);
    float32    d3 = dot_vec2(rect_br(r), line);

    result.min = min(min(d0, d1), min(d2, d3));
    result.max = max(max(d0, d1), max(d2, d3));
    return result;
}

internal Projection
project_quad(Quad q, Vec2 line)
{
    Projection result;
    float32    d0 = dot_vec2(q.vertices[QuadVertexIndexBottomLeft], line);
    float32    d1 = dot_vec2(q.vertices[QuadVertexIndexTopLeft], line);
    float32    d2 = dot_vec2(q.vertices[QuadVertexIndexTopRight], line);
    float32    d3 = dot_vec2(q.vertices[QuadVertexIndexBottomRight], line);

    result.min = min(min(d0, d1), min(d2, d3));
    result.max = max(max(d0, d1), max(d2, d3));
    return result;
}

internal Projection
project_polygon(P_Polygon a, Vec2 line)
{
    Projection result;
    float32    min = FLOAT32_MAX;
    float32    max = FLOAT32_MIN;
    for (int32 i = 0; i < a.vertex_count; i++)
    {
        Vec2    vertex = a.vertices[i];
        float32 d      = dot_vec2(vertex, line);
        min            = min(d, min);
        max            = max(d, max);
    }

    result.min = min;
    result.max = max;
    return result;
}

internal Projection
project_vertices(Vec2* vertices, uint32 vertex_count, Vec2 line)
{
    Projection result;
    float32    min = FLOAT32_MAX;
    float32    max = FLOAT32_MIN;
    for (uint32 i = 0; i < vertex_count; i++)
    {
        Vec2    vertex = vertices[i];
        float32 d      = dot_vec2(vertex, line);
        min            = min(d, min);
        max            = max(d, max);
    }

    result.min = min;
    result.max = max;
    return result;
}

internal bool32
projection_contains(Projection a, Projection b)
{
    return a.min < b.min && a.max > b.max;
}

internal bool32
projection_overlaps(Projection p0, Projection p1)
{
    return !(p0.min > p1.max || p1.min > p0.max);
}

internal bool32
projection_overlaps_point(Projection p, float32 v)
{
    return v > p.min && v < p.max;
}

internal float32
projection_overlap_amount(Projection p0, Projection p1)
{
    return min(p0.max - p1.min, p1.max - p0.min);
}

internal Vec2
closest_point_rect(Rect r, Vec2 v)
{
    Vec2    closest     = v;
    float32 min_dist_sq = FLOAT32_MAX;

    float32 dist_sq = distsqr_vec2(rect_bl(r), v);
    if (dist_sq < min_dist_sq)
    {
        min_dist_sq = dist_sq;
        closest     = rect_bl(r);
    }

    dist_sq = distsqr_vec2(rect_tr(r), v);
    if (dist_sq < min_dist_sq)
    {
        min_dist_sq = dist_sq;
        closest     = rect_tr(r);
    }

    dist_sq = distsqr_vec2(rect_br(r), v);
    if (dist_sq < min_dist_sq)
    {
        min_dist_sq = dist_sq;
        closest     = rect_br(r);
    }

    dist_sq = distsqr_vec2(rect_tl(r), v);
    if (dist_sq < min_dist_sq)
    {
        min_dist_sq = dist_sq;
        closest     = rect_tl(r);
    }

    return closest;
}

internal Vec2
minimum_translation_vector(Vec2 normal, Projection p0, Projection p1, Vec2 center_0, Vec2 center_1)
{
    float32 overlap_0 = p0.max - p1.min;
    float32 overlap_1 = p0.min - p1.max;
    float32 m         = min(fabs(overlap_0), fabs(overlap_1));
    Vec2    v         = sub_vec2(center_1, center_0);
    float32 d         = dot_vec2(v, normal);
    float32 sign      = d > 0 ? 1 : -1;
    return mul_vec2_f32(mul_vec2_f32(normal, sign), m);
}

internal Vec2
p_polygon_tr(P_Polygon p)
{
    Vec2 max = vec2(FLOAT32_MIN, FLOAT32_MIN);
    for (int32 i = 0; i < p.vertex_count; i++)
    {
        max.x = max(p.vertices[i].x, max.x);
        max.y = max(p.vertices[i].y, max.y);
    }
    return max;
}