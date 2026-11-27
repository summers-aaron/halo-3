// graphicslibxbox/render_patchy_fog.obj: from the records 0x180f18 0x4569e8 0x456b20
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "effects/effect_render_globals.h"
// @address 0x4569e8 ?set_rasterizer_state@c_patchy_fog@@CAXPBUs_tag_reference@@@Z
#define NOD3D
#include <xtl.h>
#include <d3d9.h>
#include "math/matrix_math.h"
#include "math/real_math.h"
#include "rasterizer/rasterizer_states.h"
#include "bitmaps/bitmap_data.h"
#include "math/xbox_math.h"
#include "rasterizer/xenon/xenon_matrix.h"
#include <math.h>
#include <string.h>
#include "rasterizer/rasterizer_textures_cache.h"
#include "bitmaps/bitmap_group.h"
#include "rasterizer/rasterizer_primitives.h"
#include "rasterizer/rasterizer.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render/views/patchy_fog.h"
#include "sky_atm/weighted_atmosphere_parameters.h"
#include "sky_atm/patchy_fog_atmosphere.h"

static __forceinline XMVECTOR patchy_fog_vector_set(float x,float y,float z,float w)
{
    XMVECTOR xy=__lvlx(&x,0);
    XMVECTOR vy=__lvlx(&y,0);
    XMVECTOR zw=__lvlx(&z,0);
    XMVECTOR vw=__lvlx(&w,0);
    xy=__vrlimi(xy,vy,4,3);
    zw=__vrlimi(zw,vw,4,3);
    return __vrlimi(xy,zw,3,2);
}
static __forceinline XMVECTOR patchy_fog_center_vector(float x,float y,float z,float w)
{
    XMVECTOR xy=__lvlx(&x,0);
    XMVECTOR vy=__lvlx(&y,0);
    XMVECTOR zw=__lvlx(&z,0);
    XMVECTOR vw=__lvlx(&w,0);
    zw=__vrlimi(zw,vw,4,3);
    return __vrlimi(__vrlimi(xy,vy,4,3),zw,3,2);
}
static __forceinline XMVECTOR patchy_fog_vector_scale(XMVECTOR value,float scale)
{
    return __vmulfp(value,__vspltw(__lvlx(&scale,0),0));
}



// @address 0x8c4828 ?ms_dt@c_patchy_fog@@0MA
float c_patchy_fog::ms_dt;


static __forceinline void patchy_fog_find_euler_angles(const real_vector3d *forward, const real_vector3d *up, real_euler_angles3d *result)
{
    real_matrix3x3 matrix;
    matrix.forward = *forward;
    matrix.up = *up;
    cross_product3d(&matrix.up, &matrix.forward, &matrix.left);
    real_quaternion quaternion;
    matrix3x3_rotation_to_quaternion(&matrix, &quaternion);
    matrix4x3_rotation_to_angles(&quaternion, result);
}

// @address 0x456b20 ?get_relative_movement@c_patchy_fog@@AAAXPBTreal_point3d@@PBTreal_vector3d@@1PAT3@PATreal_euler_angles3d@@@Z
void c_patchy_fog::get_relative_movement(const real_point3d *point, const real_vector3d *vector, const real_vector3d *vector2,
    real_vector3d *result, real_euler_angles3d *euler_angles)
{
    real_vector3d cross;
    cross_product3d(&m_data.left, &m_data.forward, &cross);
    real_vector3d forward_row;
    forward_row.i = dot_product3d(vector, &m_data.forward);
    forward_row.j = dot_product3d(vector, &cross);
    forward_row.k = dot_product3d(vector, &m_data.left);
    real_vector3d up_row;
    up_row.i = dot_product3d(vector2, &m_data.forward);
    up_row.j = dot_product3d(vector2, &cross);
    up_row.k = dot_product3d(vector2, &m_data.left);
    patchy_fog_find_euler_angles(&forward_row, &up_row, euler_angles);
    vector_from_points3d(&m_data.position, point, result);
}

// @address 0x4569e8 ?set_rasterizer_state@c_patchy_fog@@CAXPBUs_tag_reference@@@Z
void c_patchy_fog::set_rasterizer_state(const s_tag_reference *tag_reference)
{
    if (c_rasterizer::g_current_alpha_blend_mode != 7) {
        D3DDevice *device = c_rasterizer::get_device();
        c_rasterizer::g_current_alpha_blend_mode = (c_rasterizer::e_alpha_blend_mode)7;
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 7);
    }
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
    c_rasterizer::set_sampler_address_mode(0, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
    c_rasterizer::set_sampler_filter_mode(0, c_rasterizer::_sampler_filter_mode_bilinear);
    c_rasterizer_texture_ref texture;
    const bitmap_data *entry;
    texture.clear();
    bitmap_group_get_xenon_hardware_format(tag_reference->index, 0, &texture, &entry, 0);
    c_rasterizer::set_sampler_texture(0, texture);
    c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
    c_rasterizer::set_sampler_filter_mode(1, c_rasterizer::_sampler_filter_mode_point);
    c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)3);
    c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, false);
    c_rasterizer::set_explicit_shaders(56, _vertex_type_patchy_fog, _transfer_vector_vertex_none, _entry_point_default);
}

static __forceinline real_vector4d *patchy_fog_vector4_to_real(XMVECTOR value, real_vector4d *result)
{
    result->i=value.v[0];
    result->j=value.v[1];
    result->k=value.v[2];
    result->l=value.v[3];
    return result;
}

static __forceinline void patchy_fog_set_constant(long index, const real_vector4d *value)
{
    c_rasterizer::get_device()->SetPixelShaderConstantF(index, &value->i, 1);
}

static __forceinline void patchy_fog_set_vector(long index, XMVECTOR value, real_vector4d *scratch)
{
    patchy_fog_set_constant(index, patchy_fog_vector4_to_real(value,scratch));
}

static __forceinline XMMATRIX patchy_fog_matrix_multiply(XMMATRIX M1,XMMATRIX M2)
{

    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P;
    XMMATRIX Result;

    P.r[0] = __vmrghw(M2.r[0], M2.r[2]);
    P.r[1] = __vmrghw(M2.r[1], M2.r[3]);
    P.r[2] = __vmrglw(M2.r[0], M2.r[2]);
    P.r[3] = __vmrglw(M2.r[1], M2.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    XMVECTOR R0 = M1.r[0];
    M00 = __vmsum4fp(R0, M2T.r[0]);
    M02 = __vmsum4fp(R0, M2T.r[2]);
    M01 = __vmsum4fp(R0, M2T.r[1]);
    M03 = __vmsum4fp(R0, M2T.r[3]);

    M10 = __vmsum4fp(M1.r[1], M2T.r[0]);
    M12 = __vmsum4fp(M1.r[1], M2T.r[2]);
    M11 = __vmsum4fp(M1.r[1], M2T.r[1]);
    M13 = __vmsum4fp(M1.r[1], M2T.r[3]);

    M20 = __vmsum4fp(M1.r[2], M2T.r[0]);
    M22 = __vmsum4fp(M1.r[2], M2T.r[2]);
    M21 = __vmsum4fp(M1.r[2], M2T.r[1]);
    M23 = __vmsum4fp(M1.r[2], M2T.r[3]);

    M30 = __vmsum4fp(M1.r[3], M2T.r[0]);
    M32 = __vmsum4fp(M1.r[3], M2T.r[2]);
    M31 = __vmsum4fp(M1.r[3], M2T.r[1]);
    M33 = __vmsum4fp(M1.r[3], M2T.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Result.r[0] = __vmrghw(M00M02, M01M03);
    Result.r[1] = __vmrghw(M10M12, M11M13);
    Result.r[2] = __vmrghw(M20M22, M21M23);
    Result.r[3] = __vmrghw(M30M32, M31M33);

    return Result;

}

static __forceinline XMVECTOR patchy_fog_vector_transform(XMVECTOR V,const XMMATRIX &M)
{

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR Result;

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    R0 = __vmsum4fp(V, MT.r[0]);
    R1 = __vmsum4fp(V, MT.r[1]);
    R2 = __vmsum4fp(V, MT.r[2]);
    R3 = __vmsum4fp(V, MT.r[3]);

    R0R2 = __vmrghw(R0, R2);
    R1R3 = __vmrghw(R1, R3);

    Result = __vmrghw(R0R2, R1R3);

    return Result;

}

static __forceinline void patchy_fog_build_matrices(const real_matrix4x3 *view,
    const float (*projection)[4], XMMATRIX *view_matrix, XMMATRIX *projection_matrix)
{
    const float zero=0.0f;
    view_matrix->_11=view->forward.i; view_matrix->_12=view->forward.j; view_matrix->_13=view->forward.k;
    view_matrix->_21=view->left.i; view_matrix->_22=view->left.j; view_matrix->_23=view->left.k;
    view_matrix->_31=view->up.i; view_matrix->_32=view->up.j; view_matrix->_33=view->up.k;
    view_matrix->_41=view->position.x; view_matrix->_42=view->position.y; view_matrix->_43=view->position.z;
    view_matrix->_14=zero; view_matrix->_24=zero; view_matrix->_34=0.0f; view_matrix->_44=1.0f;
    memcpy(projection_matrix,projection,sizeof(*projection_matrix));
}

static __forceinline void patchy_fog_scale_vector(const real_vector3d *v,float scale,real_vector3d *out)
{
    out->i=v->i*scale; out->j=v->j*scale; out->k=v->k*scale;
}

static __forceinline XMVECTOR patchy_fog_sheet_offset(unsigned long index)
{
    unsigned long cell=index*77%100;
    return patchy_fog_vector_set((float)(cell%10)*0.1f,(float)(cell/10)*0.1f,0.0f,0.0f);
}

static __forceinline bool patchy_fog_populate(patchy_fog_atmosphere **atmosphere)
{
    *atmosphere=0;
    bool enabled=tls_effect_render_globals()->camera_relative;
    if(enabled)
    {
        long index=global_scenario->sky_parameters.index;
        if(index!=-1)
            *atmosphere=TAG_GET('skya', patchy_fog_atmosphere, index);
    }
    return enabled;
}

// @address 0x180f18 ?render_patchy_fog@c_patchy_fog@@QAAXPBTreal_point3d@@PBTreal_vector3d@@1PBTrectangle2d@@2PBUreal_matrix4x3@@MQAY03$$CBM@Z
void c_patchy_fog::render_patchy_fog(const real_point3d *point, const real_vector3d *forward,
    const real_vector3d *up, const rectangle2d *viewport, const rectangle2d *surface,
    const real_matrix4x3 *view, float near_distance, const float (*const projection)[4])
{
    patchy_fog_atmosphere *atmosphere;
    bool enabled=patchy_fog_populate(&atmosphere);
    if(enabled && c_atmosphere_fog_interface::m_default_parameters.valid && atmosphere->bitmap.index!=-1
        && c_atmosphere_fog_interface::m_default_parameters.patchy_density>0.001f)
    {
        set_rasterizer_state(&atmosphere->bitmap);
        real_vector3d movement;
        real_euler_angles3d angles;
        get_relative_movement(point,forward,up,&movement,&angles);
        float yaw=-angles.yaw;
        float roll=-angles.roll;
        float relative_j=yaw, relative_k=yaw;
        m_rotation-=angles.pitch;
        XMMATRIX view_matrix, projection_matrix;
        patchy_fog_build_matrices(view,projection,&view_matrix,&projection_matrix);

        XMMATRIX combined=patchy_fog_matrix_multiply(view_matrix,projection_matrix);
        XMVECTOR determinant;
        XMMATRIX inverse_projection=XMMatrixInverse(&determinant,projection_matrix);
        XMMATRIX inverse_combined=XMMatrixInverse(&determinant,combined);
        float half_fov_x=(float)atan(1.0f/projection_matrix._11);
        float half_fov_y=(float)atan(1.0f/projection_matrix._22);
        real_vector3d wind;
        patchy_fog_scale_vector(&c_atmosphere_fog_interface::m_default_parameters.patchy_velocity,-ms_dt,&wind);
        real_vector3d relative;
        relative.i=wind.i+movement.i;
        relative_j=wind.j+movement.j;
        relative_k=wind.k+movement.k;
        real_vector3d left;
        left.i=forward->j*up->k-forward->k*up->j;
        left.j=forward->k*up->i-forward->i*up->k;
        left.k=forward->i*up->j-forward->j*up->i;
        float along=relative.i*forward->i+relative_j*forward->j+relative_k*forward->k;
        float vertical=relative.i*up->i+relative_j*up->j+relative_k*up->k;
        float horizontal=relative.i*left.i+relative_j*left.j+relative_k*left.k;
        m_sheet_distance-=along;
        if(m_sheet_distance<near_distance)
        {
            float count=(float)ceil((near_distance-m_sheet_distance)/atmosphere->sheet_spacing);
            m_sheet_distance+=count*atmosphere->sheet_spacing;
            m_sheet_index=(m_sheet_index+(long)((double)count+0.000001))%100;
        }
        else if(m_sheet_distance>near_distance+atmosphere->sheet_spacing)
        {
            float count=(float)floor((m_sheet_distance-near_distance)/atmosphere->sheet_spacing);
            m_sheet_distance-=count*atmosphere->sheet_spacing;
            m_sheet_index=(m_sheet_index-(long)((double)count+0.000001))%100;
        }
        unsigned long first_sheet;
        if(!(m_near_distance>m_sheet_distance))
            first_sheet=0;
        else
            first_sheet=(unsigned long)(float)ceil((m_near_distance-m_sheet_distance)/atmosphere->sheet_spacing);
        if(!(near_distance>m_near_distance)) near_distance=m_near_distance;
        float fade;
        if((first_sheet*atmosphere->sheet_spacing+m_sheet_distance-near_distance)/atmosphere->sheet_spacing>1.0f)
            fade=1.0f;
        else
            fade=(first_sheet*atmosphere->sheet_spacing+m_sheet_distance-near_distance)/atmosphere->sheet_spacing;
        float last_fade=1.0f-fade;
        real_vector4d scratch;
        patchy_fog_set_vector(32,patchy_fog_vector_set(inverse_projection._33,inverse_projection._34,inverse_projection._43,inverse_projection._44),&scratch);
        XMVECTOR rotation=patchy_fog_vector_set((float)cos(m_rotation),(float)sin(m_rotation),-(float)sin(m_rotation),(float)cos(m_rotation));
        patchy_fog_set_vector(33,rotation,&scratch);
        float decay = c_atmosphere_fog_interface::m_default_parameters.patchy_end_distance
                == c_atmosphere_fog_interface::m_default_parameters.patchy_start_distance
            ? 10000.0f
            : 0.693147f / (c_atmosphere_fog_interface::m_default_parameters.patchy_end_distance
                - c_atmosphere_fog_interface::m_default_parameters.patchy_start_distance);
        patchy_fog_set_vector(34,
            patchy_fog_vector_set(
                c_atmosphere_fog_interface::m_default_parameters.rayleigh_height
                    + c_atmosphere_fog_interface::m_default_parameters.patchy_start_distance,
                decay,
                atmosphere->texture_scale, atmosphere->sheet_spacing),
            &scratch);
        real_vector4d camera={point->x,point->y,point->z,1.0f};
        patchy_fog_set_constant(35,&camera);
        real_vector4d screen;
        screen.i=(((float)(viewport->x0+viewport->x1)+1.0f)*0.5f)/(float)surface->x1;
        screen.j=(((float)(viewport->y0+viewport->y1)+1.0f)*0.5f)/(float)surface->y1;
        screen.k=((float)(viewport->x1-viewport->x0)*0.5f)/(float)surface->x1;
        screen.l=((float)(viewport->y1-viewport->y0)*-0.5f)/(float)surface->y1;
        patchy_fog_set_constant(36,&screen);
        const s_weighted_atmosphere_parameters *parameters=&c_atmosphere_fog_interface::m_default_parameters;
        XMVECTOR near_opacity=patchy_fog_vector_scale(patchy_fog_vector_set(fade,1.0f,1.0f,1.0f),parameters->patchy_density);
        XMVECTOR far_opacity=patchy_fog_vector_scale(patchy_fog_vector_set(1.0f,1.0f,1.0f,last_fade),parameters->patchy_density);
        patchy_fog_set_vector(40,near_opacity,&scratch);
        patchy_fog_set_vector(41,far_opacity,&scratch);
        patchy_fog_set_vector(42,
            patchy_fog_vector_set(m_sheet_distance + 0.0f * atmosphere->sheet_spacing, m_sheet_distance + 1.0f * atmosphere->sheet_spacing,
                m_sheet_distance + 2.0f * atmosphere->sheet_spacing, m_sheet_distance + 3.0f * atmosphere->sheet_spacing),
            &scratch);
        patchy_fog_set_vector(43,
            patchy_fog_vector_set(m_sheet_distance + 4.0f * atmosphere->sheet_spacing, m_sheet_distance + 5.0f * atmosphere->sheet_spacing,
                m_sheet_distance + 6.0f * atmosphere->sheet_spacing, m_sheet_distance + 7.0f * atmosphere->sheet_spacing),
            &scratch);
        XMVECTOR swapped=__vpermwi(rotation,0xb1);
        for(unsigned long sheet_index=0;sheet_index<8;sheet_index++)
        {
            unsigned long sheet=first_sheet+sheet_index;
            unsigned long index=(m_sheet_index+sheet)%100;
            XMVECTOR offset=patchy_fog_sheet_offset(index);
            float depth=(m_sheet_distance+sheet*atmosphere->sheet_spacing)/atmosphere->sheet_spacing;
            m_sheet_u[index]+=horizontal/atmosphere->sheet_spacing*rotation.v[0]+vertical/atmosphere->sheet_spacing*rotation.v[2];
            m_sheet_v[index]+=horizontal/atmosphere->sheet_spacing*rotation.v[1]+vertical/atmosphere->sheet_spacing*rotation.v[3];
            float sheet_u=m_sheet_u[index]+yaw*depth*rotation.v[0]+roll*depth*rotation.v[2];
            m_sheet_u[index]=sheet_u;
            float sheet_v=m_sheet_v[index]+yaw*depth*rotation.v[1]+roll*depth*rotation.v[3];
            m_sheet_v[index]=sheet_v;
            float u=(atmosphere->rotation*0.159154943f)*sheet_u;
            float sx=depth*(atmosphere->rotation*0.159154943f)*half_fov_x;
            float v=(atmosphere->rotation*0.159154943f)*sheet_v;
            float sy=depth*(atmosphere->rotation*0.159154943f)*half_fov_y;
            XMVECTOR center=patchy_fog_center_vector(projection_matrix._31,projection_matrix._32,0.0f,0.0f);
            XMVECTOR shift = __vmaddfp(patchy_fog_vector_scale(rotation, center.v[0]), __vspltw(__lvlx(&sx, 0), 0),
                patchy_fog_vector_scale(patchy_fog_vector_scale(swapped, center.v[1]), sy));
            XMVECTOR value=patchy_fog_vector_set(u+shift.v[0],v+shift.v[1],sx,sy);
            value.v[0]+=offset.v[0]; value.v[1]+=offset.v[1];
            patchy_fog_set_vector(44+sheet_index,value,&scratch);
        }
        XMVECTOR depth_point=patchy_fog_vector_transform(patchy_fog_vector_set(0.0f,0.0f,-1.0f,1.0f),projection_matrix);
        float z=depth_point.v[2]/depth_point.v[3];
        XMVECTOR vertices[4];
        vertices[0]=patchy_fog_vector_transform(patchy_fog_vector_set(-1.0f,1.0f,z,1.0f),inverse_combined);
        vertices[1]=patchy_fog_vector_transform(patchy_fog_vector_set(-1.0f,-1.0f,z,1.0f),inverse_combined);
        vertices[2]=patchy_fog_vector_transform(patchy_fog_vector_set(1.0f,-1.0f,z,1.0f),inverse_combined);
        vertices[3]=patchy_fog_vector_transform(patchy_fog_vector_set(1.0f,1.0f,z,1.0f),inverse_combined);
        c_rasterizer::get_device()->DrawPrimitiveUP(g_primitive_types[c_rasterizer_index_buffer::_primitive_type_quad_list],1,vertices,16);
        m_data.position=*point;
        m_data.forward=*forward;
        m_data.left=*up;
    }
}
