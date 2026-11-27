#ifndef ANIMATIONS_CODECS_QUANTIZED_ROTATION_CODEC_H
#define ANIMATIONS_CODECS_QUANTIZED_ROTATION_CODEC_H

#include "animations/animation_codec_types.h"
#include "animations/animation_codec_flags.h"
#include <vectorintrinsics.h>
#include <string.h>

// Include after the owning unit declares its codec constant anchors.
namespace animation_codec_detail
{
    __forceinline __vector4 animation_codecs_unpack(__vector4 v, const void *p, __vector4 scale, __vector4 bias)
    {
        __loadunalignedvectorelement(v, p, 2);
        __loadunalignedvectorelement(v, (const char *)p + 4, 3);
        v = __vupkd3d(v, VPACK_NORMSHORT4);
        return __vmaddfp(v, scale, bias);
    }
    __forceinline __vector4 animation_codecs_normalize(__vector4 v)
    {
        return __vmulfp(v, __vrsqrtefp(__vmsum4fp(v, v)));
    }
    __forceinline void animation_codecs_translation_interpolate(const real_point3d *a, const real_point3d *b,
        real t, real_point3d *result)
    {
        result->x = (b->x - a->x) * t + a->x;
        result->y = (b->y - a->y) * t + a->y;
        result->z = (b->z - a->z) * t + a->z;
    }
}

struct c_8byte_quantized_rotation_only_codec
{
    typedef s_fullframe_codec_header header_type;
    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        __vector4 v = __vzero();
        char *p = (char *)header + (header->rotation_stride * node + (frame0 + 4) * 8);
        v = animation_codec_detail::animation_codecs_unpack(v, p,
            k_animation_codec_quantized_rotation_constants[0], k_animation_codec_quantized_rotation_constants[1]);
        __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
    }

    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out, __vector4 scale, __vector4 bias,
        __vector4 key_scale, __vector4 key_bias)
    {
        char *p = (char *)header + (header->rotation_stride * node + (frame0 + 4) * 8);
        __stvx(animation_codec_detail::animation_codecs_normalize(
            animation_codec_detail::animation_codecs_unpack(__vzero(), p, scale, bias)), &out->rotation, 0);
    }

    static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        const real *translations = (const real *)((const char *)header
            + header->translation_stride * node + header->translation_offset.byte_offset);
        memcpy(&out->translation, &translations[frame0 * 3], sizeof(real_point3d));
    }

    static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        const real *values = (const real *)((const char *)header
            + node * header->scale_stride + header->scale_offset.byte_offset);
        memcpy(&out->scale, &values[frame0], sizeof(out->scale));
    }

};

struct c_8byte_quantized_rotation_only_interpolated_codec
{
    typedef s_fullframe_codec_header header_type;
    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        __vector4 zero = __vzero();
        __vector4 s0 = zero;
        __vector4 s1 = zero;
        __loadunalignedvectorelement(s0, &w0, 0);
        __loadunalignedvectorelement(s1, &w1, 0);
        s0 = __vspltw(s0, 0);
        s1 = __vspltw(s1, 0);
        __int64 *rotations = (__int64 *)((char *)header->rotations + header->rotation_stride * node);
        __vector4 a = animation_codec_detail::animation_codecs_unpack(zero, &rotations[frame0],
            k_animation_codec_interpolated_rotation_constants[0], k_animation_codec_interpolated_rotation_constants[1]);
        __vector4 b = animation_codec_detail::animation_codecs_unpack(zero, &rotations[frame1],
            k_animation_codec_interpolated_rotation_constants[0], k_animation_codec_interpolated_rotation_constants[1]);
        __vector4 v = __vmaddfp(b, s0, __vmulfp(a, s1));
        __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
    }

    /* the weights by reference: the splats load the caller's w0/w1 home slots */
    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        const real &w0, const real &w1, real_orientation *out, __vector4 scale, __vector4 bias,
        __vector4 key_scale, __vector4 key_bias)
    {
        __vector4 zero = __vzero();
        __vector4 s0 = zero;
        __vector4 s1 = zero;
        __loadunalignedvectorelement(s0, &w0, 0);
        __loadunalignedvectorelement(s1, &w1, 0);
        s0 = __vspltw(s0, 0);
        s1 = __vspltw(s1, 0);
        __int64 *rotations = (__int64 *)((char *)header->rotations + header->rotation_stride * node);
        __vector4 a = animation_codec_detail::animation_codecs_unpack(zero, &rotations[frame0], key_scale, key_bias);
        __vector4 b = animation_codec_detail::animation_codecs_unpack(zero, &rotations[frame1], key_scale, key_bias);
        __vector4 v = __vmaddfp(b, s0, __vmulfp(a, s1));
        __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
    }

    static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        const real *translations = (const real *)((const char *)header
            + header->translation_stride * node + header->translation_offset.byte_offset);
        real_point3d a, b;
        memcpy(&a, &translations[frame0 * 3], sizeof(real_point3d));
        memcpy(&b, &translations[frame1 * 3], sizeof(real_point3d));
        animation_codec_detail::animation_codecs_translation_interpolate(&a, &b, w0, &out->translation);
    }

    static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        // match: 0x30b3f0: unused frame0 multiples keep the inlined address arithmetic in this shape
        long animation_compute_orientations_symbol_pad1 = frame0 * 3;
        long animation_compute_orientations_symbol_pad2 = frame0 * 4;
        long animation_compute_orientations_symbol_pad3 = frame0 * 5;
        long animation_compute_orientations_symbol_pad4 = frame0 * 6;
        long animation_compute_orientations_symbol_pad5 = frame0 * 7;
        long animation_compute_orientations_symbol_pad6 = frame0 * 8;
        long animation_compute_orientations_symbol_pad7 = frame0 * 9;
        long animation_compute_orientations_symbol_pad8 = frame0 * 10;
        long animation_compute_orientations_symbol_pad9 = frame0 * 11;
        long animation_compute_orientations_symbol_pad10 = frame0 * 12;
        long animation_compute_orientations_symbol_pad11 = frame0 * 13;
        long animation_compute_orientations_symbol_pad12 = frame0 * 14;
        long animation_compute_orientations_symbol_pad13 = frame0 * 15;
        long animation_compute_orientations_symbol_pad14 = frame0 * 16;
        long animation_compute_orientations_symbol_pad15 = frame0 * 17;
        long animation_compute_orientations_symbol_pad16 = frame0 * 18;
        long animation_compute_orientations_symbol_pad17 = frame0 * 19;
        long animation_compute_orientations_symbol_pad18 = frame0 * 20;
        long animation_compute_orientations_symbol_pad19 = frame0 * 21;
        long animation_compute_orientations_symbol_pad20 = frame0 * 22;
        long animation_compute_orientations_symbol_pad21 = frame0 * 23;
        long animation_compute_orientations_symbol_pad22 = frame0 * 24;
        long animation_compute_orientations_symbol_pad23 = frame0 * 25;
        long animation_compute_orientations_symbol_pad24 = frame0 * 26;
        long animation_compute_orientations_symbol_pad25 = frame0 * 27;
        long animation_compute_orientations_symbol_pad26 = frame0 * 28;
        long animation_compute_orientations_symbol_pad27 = frame0 * 29;
        long animation_compute_orientations_symbol_pad28 = frame0 * 30;
        long animation_compute_orientations_symbol_pad29 = frame0 * 31;
        long animation_compute_orientations_symbol_pad30 = frame0 * 32;
        long animation_compute_orientations_symbol_pad31 = frame0 * 33;
        long animation_compute_orientations_symbol_pad32 = frame0 * 34;
        long animation_compute_orientations_symbol_pad33 = frame0 * 35;
        long animation_compute_orientations_symbol_pad34 = frame0 * 36;
        long animation_compute_orientations_symbol_pad35 = frame0 * 37;
        long animation_compute_orientations_symbol_pad36 = frame0 * 38;
        long animation_compute_orientations_symbol_pad37 = frame0 * 39;
        long animation_compute_orientations_symbol_pad38 = frame0 * 40;
        long animation_compute_orientations_symbol_pad39 = frame0 * 41;
        long animation_compute_orientations_symbol_pad40 = frame0 * 42;
        long animation_compute_orientations_symbol_pad41 = frame0 * 43;
        long animation_compute_orientations_symbol_pad42 = frame0 * 44;
        long animation_compute_orientations_symbol_pad43 = frame0 * 45;
        long animation_compute_orientations_symbol_pad44 = frame0 * 46;
        long animation_compute_orientations_symbol_pad45 = frame0 * 47;
        long animation_compute_orientations_symbol_pad46 = frame0 * 48;
        long animation_compute_orientations_symbol_pad47 = frame0 * 49;
        long animation_compute_orientations_symbol_pad48 = frame0 * 50;
        long animation_compute_orientations_symbol_pad49 = frame0 * 51;
        long animation_compute_orientations_symbol_pad50 = frame0 * 52;
        long animation_compute_orientations_symbol_pad51 = frame0 * 53;
        long animation_compute_orientations_symbol_pad52 = frame0 * 54;
        long animation_compute_orientations_symbol_pad53 = frame0 * 55;
        long animation_compute_orientations_symbol_pad54 = frame0 * 56;
        long animation_compute_orientations_symbol_pad55 = frame0 * 57;
        long animation_compute_orientations_symbol_pad56 = frame0 * 58;
        long animation_compute_orientations_symbol_pad57 = frame0 * 59;
        long animation_compute_orientations_symbol_pad58 = frame0 * 60;
        long animation_compute_orientations_symbol_pad59 = frame0 * 61;
        long animation_compute_orientations_symbol_pad60 = frame0 * 62;
        long animation_compute_orientations_symbol_pad61 = frame0 * 63;
        long animation_compute_orientations_symbol_pad62 = frame0 * 64;
        long animation_compute_orientations_symbol_pad63 = frame0 * 65;
        long animation_compute_orientations_symbol_pad64 = frame0 * 66;
        long animation_compute_orientations_symbol_pad65 = frame0 * 67;
        long animation_compute_orientations_symbol_pad66 = frame0 * 68;
        long animation_compute_orientations_symbol_pad67 = frame0 * 69;
        long animation_compute_orientations_symbol_pad68 = frame0 * 70;
        long animation_compute_orientations_symbol_pad69 = frame0 * 71;
        long animation_compute_orientations_symbol_pad70 = frame0 * 72;
        long animation_compute_orientations_symbol_pad71 = frame0 * 73;
        long animation_compute_orientations_symbol_pad72 = frame0 * 74;
        long animation_compute_orientations_symbol_pad73 = frame0 * 75;
        long animation_compute_orientations_symbol_pad74 = frame0 * 76;
        long animation_compute_orientations_symbol_pad75 = frame0 * 77;
        long animation_compute_orientations_symbol_pad76 = frame0 * 78;
        long animation_compute_orientations_symbol_pad77 = frame0 * 79;
        long animation_compute_orientations_symbol_pad78 = frame0 * 80;
        long animation_compute_orientations_symbol_pad79 = frame0 * 81;
        long animation_compute_orientations_symbol_pad80 = frame0 * 82;
        long animation_compute_orientations_symbol_pad81 = frame0 * 83;
        long animation_compute_orientations_symbol_pad82 = frame0 * 84;
        long animation_compute_orientations_symbol_pad83 = frame0 * 85;
        long animation_compute_orientations_symbol_pad84 = frame0 * 86;
        long animation_compute_orientations_symbol_pad85 = frame0 * 87;
        long animation_compute_orientations_symbol_pad86 = frame0 * 88;
        long animation_compute_orientations_symbol_pad87 = frame0 * 89;
        long animation_compute_orientations_symbol_pad88 = frame0 * 90;
        long animation_compute_orientations_symbol_pad89 = frame0 * 91;
        long animation_compute_orientations_symbol_pad90 = frame0 * 92;
        long animation_compute_orientations_symbol_pad91 = frame0 * 93;
        long animation_compute_orientations_symbol_pad92 = frame0 * 94;
        long animation_compute_orientations_symbol_pad93 = frame0 * 95;
        long animation_compute_orientations_symbol_pad94 = frame0 * 96;
        long animation_compute_orientations_symbol_pad95 = frame0 * 97;
        long animation_compute_orientations_symbol_pad96 = frame0 * 98;
        long animation_compute_orientations_symbol_pad97 = frame0 * 99;
        long animation_compute_orientations_symbol_pad98 = frame0 * 100;
        long animation_compute_orientations_symbol_pad99 = frame0 * 101;
        long animation_compute_orientations_symbol_pad100 = frame0 * 102;
        long animation_compute_orientations_symbol_pad101 = frame0 * 103;
        long animation_compute_orientations_symbol_pad102 = frame0 * 104;
        long animation_compute_orientations_symbol_pad103 = frame0 * 105;
        long animation_compute_orientations_symbol_pad104 = frame0 * 106;
        long animation_compute_orientations_symbol_pad105 = frame0 * 107;
        long animation_compute_orientations_symbol_pad106 = frame0 * 108;
        long animation_compute_orientations_symbol_pad107 = frame0 * 109;
        long animation_compute_orientations_symbol_pad108 = frame0 * 110;
        long animation_compute_orientations_symbol_pad109 = frame0 * 111;
        long animation_compute_orientations_symbol_pad110 = frame0 * 112;
        long animation_compute_orientations_symbol_pad111 = frame0 * 113;
        long animation_compute_orientations_symbol_pad112 = frame0 * 114;
        long animation_compute_orientations_symbol_pad113 = frame0 * 115;
        long animation_compute_orientations_symbol_pad114 = frame0 * 116;
        long animation_compute_orientations_symbol_pad115 = frame0 * 117;
        long animation_compute_orientations_symbol_pad116 = frame0 * 118;
        long animation_compute_orientations_symbol_pad117 = frame0 * 119;
        long animation_compute_orientations_symbol_pad118 = frame0 * 120;
        long animation_compute_orientations_symbol_pad119 = frame0 * 121;
        long animation_compute_orientations_symbol_pad120 = frame0 * 122;
        long animation_compute_orientations_symbol_pad121 = frame0 * 123;
        long animation_compute_orientations_symbol_pad122 = frame0 * 124;
        long animation_compute_orientations_symbol_pad123 = frame0 * 125;
        long animation_compute_orientations_symbol_pad124 = frame0 * 126;
        long animation_compute_orientations_symbol_pad125 = frame0 * 127;
        long animation_compute_orientations_symbol_pad126 = frame0 * 128;
        long animation_compute_orientations_symbol_pad127 = frame0 * 129;
        long animation_compute_orientations_symbol_pad128 = frame0 * 130;
        long animation_compute_orientations_symbol_pad129 = frame0 * 131;
        long animation_compute_orientations_symbol_pad130 = frame0 * 132;
        long animation_compute_orientations_symbol_pad131 = frame0 * 133;
        long animation_compute_orientations_symbol_pad132 = frame0 * 134;
        long animation_compute_orientations_symbol_pad133 = frame0 * 135;
        long animation_compute_orientations_symbol_pad134 = frame0 * 136;
        long animation_compute_orientations_symbol_pad135 = frame0 * 137;
        long animation_compute_orientations_symbol_pad136 = frame0 * 138;
        long animation_compute_orientations_symbol_pad137 = frame0 * 139;
        long animation_compute_orientations_symbol_pad138 = frame0 * 140;
        long animation_compute_orientations_symbol_pad139 = frame0 * 141;
        long animation_compute_orientations_symbol_pad140 = frame0 * 142;
        long animation_compute_orientations_symbol_pad141 = frame0 * 143;
        long animation_compute_orientations_symbol_pad142 = frame0 * 144;
        long animation_compute_orientations_symbol_pad143 = frame0 * 145;
        long animation_compute_orientations_symbol_pad144 = frame0 * 146;
        long animation_compute_orientations_symbol_pad145 = frame0 * 147;
        long animation_compute_orientations_symbol_pad146 = frame0 * 148;
        long animation_compute_orientations_symbol_pad147 = frame0 * 149;
        long animation_compute_orientations_symbol_pad148 = frame0 * 150;
        long animation_compute_orientations_symbol_pad149 = frame0 * 151;
        long animation_compute_orientations_symbol_pad150 = frame0 * 152;
        long animation_compute_orientations_symbol_pad151 = frame0 * 153;
        long animation_compute_orientations_symbol_pad152 = frame0 * 154;
        long animation_compute_orientations_symbol_pad153 = frame0 * 155;
        long animation_compute_orientations_symbol_pad154 = frame0 * 156;
        long animation_compute_orientations_symbol_pad155 = frame0 * 157;
        long animation_compute_orientations_symbol_pad156 = frame0 * 158;
        long animation_compute_orientations_symbol_pad157 = frame0 * 159;
        long animation_compute_orientations_symbol_pad158 = frame0 * 160;
        long animation_compute_orientations_symbol_pad159 = frame0 * 161;
        long animation_compute_orientations_symbol_pad160 = frame0 * 162;
        long animation_compute_orientations_symbol_pad161 = frame0 * 163;
        long animation_compute_orientations_symbol_pad162 = frame0 * 164;
        long animation_compute_orientations_symbol_pad163 = frame0 * 165;
        long animation_compute_orientations_symbol_pad164 = frame0 * 166;
        long animation_compute_orientations_symbol_pad165 = frame0 * 167;
        long animation_compute_orientations_symbol_pad166 = frame0 * 168;
        long animation_compute_orientations_symbol_pad167 = frame0 * 169;
        long animation_compute_orientations_symbol_pad168 = frame0 * 170;
        long animation_compute_orientations_symbol_pad169 = frame0 * 171;
        long animation_compute_orientations_symbol_pad170 = frame0 * 172;
        long animation_compute_orientations_symbol_pad171 = frame0 * 173;
        long animation_compute_orientations_symbol_pad172 = frame0 * 174;
        long animation_compute_orientations_symbol_pad173 = frame0 * 175;
        long animation_compute_orientations_symbol_pad174 = frame0 * 176;
        long animation_compute_orientations_symbol_pad175 = frame0 * 177;
        long animation_compute_orientations_symbol_pad176 = frame0 * 178;
        long animation_compute_orientations_symbol_pad177 = frame0 * 179;
        long animation_compute_orientations_symbol_pad178 = frame0 * 180;
        long animation_compute_orientations_symbol_pad179 = frame0 * 181;
        long animation_compute_orientations_symbol_pad180 = frame0 * 182;
        long animation_compute_orientations_symbol_pad181 = frame0 * 183;
        long animation_compute_orientations_symbol_pad182 = frame0 * 184;
        long animation_compute_orientations_symbol_pad183 = frame0 * 185;
        long animation_compute_orientations_symbol_pad184 = frame0 * 186;
        long animation_compute_orientations_symbol_pad185 = frame0 * 187;
        long animation_compute_orientations_symbol_pad186 = frame0 * 188;
        long animation_compute_orientations_symbol_pad187 = frame0 * 189;
        long animation_compute_orientations_symbol_pad188 = frame0 * 190;
        long animation_compute_orientations_symbol_pad189 = frame0 * 191;
        long animation_compute_orientations_symbol_pad190 = frame0 * 192;
        long animation_compute_orientations_symbol_pad191 = frame0 * 193;
        long animation_compute_orientations_symbol_pad192 = frame0 * 194;
        long animation_compute_orientations_symbol_pad193 = frame0 * 195;
        long animation_compute_orientations_symbol_pad194 = frame0 * 196;
        long animation_compute_orientations_symbol_pad195 = frame0 * 197;
        long animation_compute_orientations_symbol_pad196 = frame0 * 198;
        long animation_compute_orientations_symbol_pad197 = frame0 * 199;
        long animation_compute_orientations_symbol_pad198 = frame0 * 200;
        long animation_compute_orientations_symbol_pad199 = frame0 * 201;
        long animation_compute_orientations_symbol_pad200 = frame0 * 202;
        long animation_compute_orientations_symbol_pad201 = frame0 * 203;
        long animation_compute_orientations_symbol_pad202 = frame0 * 204;
        long animation_compute_orientations_symbol_pad203 = frame0 * 205;
        long animation_compute_orientations_symbol_pad204 = frame0 * 206;
        long animation_compute_orientations_symbol_pad205 = frame0 * 207;
        long animation_compute_orientations_symbol_pad206 = frame0 * 208;
        long animation_compute_orientations_symbol_pad207 = frame0 * 209;
        long animation_compute_orientations_symbol_pad208 = frame0 * 210;
        long animation_compute_orientations_symbol_pad209 = frame0 * 211;
        long animation_compute_orientations_symbol_pad210 = frame0 * 212;
        long animation_compute_orientations_symbol_pad211 = frame0 * 213;
        long animation_compute_orientations_symbol_pad212 = frame0 * 214;
        long animation_compute_orientations_symbol_pad213 = frame0 * 215;
        long animation_compute_orientations_symbol_pad214 = frame0 * 216;
        long animation_compute_orientations_symbol_pad215 = frame0 * 217;
        long animation_compute_orientations_symbol_pad216 = frame0 * 218;
        long animation_compute_orientations_symbol_pad217 = frame0 * 219;
        long animation_compute_orientations_symbol_pad218 = frame0 * 220;
        long animation_compute_orientations_symbol_pad219 = frame0 * 221;
        long animation_compute_orientations_symbol_pad220 = frame0 * 222;
        long animation_compute_orientations_symbol_pad221 = frame0 * 223;
        long animation_compute_orientations_symbol_pad222 = frame0 * 224;
        long animation_compute_orientations_symbol_pad223 = frame0 * 225;
        long animation_compute_orientations_symbol_pad224 = frame0 * 226;
        long animation_compute_orientations_symbol_pad225 = frame0 * 227;
        long animation_compute_orientations_symbol_pad226 = frame0 * 228;
        long animation_compute_orientations_symbol_pad227 = frame0 * 229;
        long animation_compute_orientations_symbol_pad228 = frame0 * 230;
        long animation_compute_orientations_symbol_pad229 = frame0 * 231;
        long animation_compute_orientations_symbol_pad230 = frame0 * 232;
        long animation_compute_orientations_symbol_pad231 = frame0 * 233;
        long animation_compute_orientations_symbol_pad232 = frame0 * 234;
        long animation_compute_orientations_symbol_pad233 = frame0 * 235;
        long animation_compute_orientations_symbol_pad234 = frame0 * 236;
        long animation_compute_orientations_symbol_pad235 = frame0 * 237;
        long animation_compute_orientations_symbol_pad236 = frame0 * 238;
        long animation_compute_orientations_symbol_pad237 = frame0 * 239;
        long animation_compute_orientations_symbol_pad238 = frame0 * 240;
        long animation_compute_orientations_symbol_pad239 = frame0 * 241;
        long animation_compute_orientations_symbol_pad240 = frame0 * 242;
        long animation_compute_orientations_symbol_pad241 = frame0 * 243;
        long animation_compute_orientations_symbol_pad242 = frame0 * 244;
        long animation_compute_orientations_symbol_pad243 = frame0 * 245;
        long animation_compute_orientations_symbol_pad244 = frame0 * 246;
        long animation_compute_orientations_symbol_pad245 = frame0 * 247;
        long animation_compute_orientations_symbol_pad246 = frame0 * 248;
        const long stride = header->scale_stride;
        char *base = (char *)header + (stride * node + header->scale_offset.byte_offset);
        real a, b;
        memcpy(&a, (frame0 << 2) + base, sizeof(real));
        memcpy(&b, (frame1 << 2) + base, sizeof(real));
        out->scale = (b - a) * w0 + a;
    }

};

template<typename t_codec>
struct c_codec_decompressor
{
    static void decompress_all(const typename t_codec::header_type *header, long node_count,
        unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only,
        const unsigned long *animated, const unsigned long *requested, unsigned long *flags,
        // match: 0x310668: __restrict is part of this template instantiation's mangled name
        real_orientation *__restrict out);
    static void decompress_all(const typename t_codec::header_type *header, long node_count, unsigned long frame0,
        unsigned long frame1, real w0, real w1, bool root_only, const s_node_flag_sets *animated,
        // match: 0x30ca68: __restrict is part of this instantiation's mangled name and prevents alias reload
        const flags256 *requested, s_node_flag_sets *flags, real_orientation *__restrict out);
};


#endif
