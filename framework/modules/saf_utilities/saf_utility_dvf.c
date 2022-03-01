/*
* Copyright 2020-2021 Michael McCrea, Leo McCormack
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
* REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
* INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
* OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*/

/**
 * @file saf_utility_dvf.c
 * @ingroup Utilities
 * @brief Distance variation function filter coefficient data [1].
 *
 * @see [1] S. Spagnol, E. Tavazzi, and F. Avanzini, “Distance rendering and
 *          perception of nearby virtual sound sources with a near-field filter
 *          model,” Applied Acoustics, vol. 115, pp. 61–73, Jan. 2017,
 *          doi: 10.1016/j.apacoust.2016.08.015.
 *
 * @author Michael McCrea
 * @date 20.02.2021
 * @license ISC
 */

#include "saf_utility_dvf.h"
#include "saf_utility_filters.h"

/**
 * Table 1: Coefficients for Eqs. (8), (13), and (14) for generating high-shelf coefficients
 */
static const float p11[19] = { 12.97f, 13.19f, 12.13f, 11.19f, 9.91f, 8.328f, 6.493f, 4.455f, 2.274f, 0.018f, -2.24f, -4.43f, -6.49f, -8.34f, -9.93f, -11.3f, -12.2f, -12.8f, -13.f };
static const float p21[19] = { -9.69f, 234.2f, -11.2f, -9.03f, -7.87f, -7.42f, -7.31f, -7.28f, -7.29f, -7.48f, -8.04f, -9.23f, -11.6f, -17.4f, -48.4f, 9.149f, 1.905f, -0.75f, -1.32f };
static const float q11[19] = { -1.14f, 18.48f, -1.25f, -1.02f, -0.83f, -0.67f, -0.5f, -0.32f, -0.11f, -0.13f, 0.395f, 0.699f, 1.084f, 1.757f, 4.764f, -0.64f, 0.109f, 0.386f, 0.45f };
static const float q21[19] = { 0.219f, -8.5f, 0.346f, 0.336f, 0.379f, 0.421f, 0.423f, 0.382f, 0.314f, 0.24f, 0.177f, 0.132f, 0.113f, 0.142f, 0.462f, -0.14f, -0.08f, -0.06f, -0.05f };
static const float p12[19] = { -4.39f, -4.31f, -4.18f, -4.01f, -3.87f, -4.1f, -3.87f, -5.02f, -6.72f, -8.69f, -11.2f, -12.1f, -11.1f, -11.1f, -9.72f, -8.42f, -7.44f, -6.78f, -6.58f };
static const float p22[19] = { 2.123f, -2.78f, 4.224f, 3.039f, -0.57f, -34.7f, 3.271f, 0.023f, -8.96f, -58.4f, 11.47f, 8.716f, 21.8f, 1.91f, -0.04f, -0.66f, 0.395f, 2.662f, 3.387f };
static const float q12[19] = { -0.55f, 0.59f, -1.01f, -0.56f, 0.665f, 11.39f, -1.57f, -0.87f, 0.37f, 5.446f, -1.13f, -0.63f, -2.01f, 0.15f, 0.243f, 0.147f, -0.18f, -0.67f, -0.84f };
static const float q22[19] = { -0.06f, -0.17f, -0.02f, -0.32f, -1.13f, -8.3f, 0.637f, 0.325f, -0.08f, -1.19f, 0.103f, -0.12f, 0.098f, -0.4f, -0.41f, -0.34f, -0.18f, 0.05f, 0.131f };
static const float p13[19] = { 0.457f, 0.455f, -0.87f, 0.465f, 0.494f, 0.549f, 0.663f, 0.691f, 3.507f, -27.4f, 6.371f, 7.032f, 7.092f, 7.463f, 7.453f, 8.101f, 8.702f, 8.925f, 9.317f };
static const float p23[19] = { -0.67f, 0.142f, 3404.f, -0.91f, -0.67f, -1.21f, -1.76f, 4.655f, 55.09f, 10336.f, 1.735f, 40.88f, 23.86f, 102.8f, -6.14f, -18.1f, -9.05f, -9.03f, -6.89f };
static const float p33[19] = { 0.174f, -0.11f, -1699.f, 0.437f, 0.658f, 2.02f, 6.815f, 0.614f, 589.3f, 16818.f, -9.39f, -44.1f, -23.6f, -92.3f, -1.81f, 10.54f, 0.532f, 0.285f, -2.08f };
static const float q13[19] = { -1.75f, -0.01f, 7354.f, -2.18f, -1.2f, -1.59f, -1.23f, -0.89f, 29.23f, 1945.f, -0.06f, 5.635f, 3.308f, 13.88f, -0.88f, -2.23f, -0.96f, -0.9f, -0.57f };
static const float q23[19] = { 0.699f, -0.35f, -5350.f, 1.188f, 0.256f, 0.816f, 1.166f, 0.76f, 59.51f, 1707.f, -1.12f, -6.18f, -3.39f, -12.7f, -0.19f, 1.295f, -0.02f, -0.08f, -0.4f };

static const int numAz_table = sizeof(q23);
static const float a_0 = 0.0875;       /**< Reference head size, 8.75 centimeters, used in the generation of the coeff lookup table. */
static const float a_head = 0.09096;   /**< This head size, See note for head_radius in binauraliser_nf. */
static const float headDim = SAF_PI * (a_0 / a_head);
static const float sosDiv2PiA = 343 / (2 * SAF_PI * a_head);

/** Linear interpolation between two values */
static float interpolate_lin
(
    float a,
    float b,
    float ifac
)
{
    return a + (b-a) * ifac;
}

/** Covert decibels to a magnitude */
static float db2mag
(
    float dB
)
{
    return powf(10.f, dB / 20.f);
}

/*
 * Calculate high-shelf parameters, g0, gInf, fc, from the lookup table coefficients (10 degree steps).
 * Called twice per update as the returned values are subsequently interpolated to exact azimuth. */
void calcHighShelfParams
(
    int i,          /* index into the coefficient table, dictated by azimuth */
    float rhoIn,    /* normalized source distance */
    float* g0,      /* high shelf gain at DC */
    float* gInf,    /* high shelf gain at inf */
    float* fc       /* high shelf cutoff frequency */
)
{
    double fc_tmp;
    double rho = (double)rhoIn;
    double rhoSq = pow(rho, 2.0f);
    
    /*  Eq (8), (13) and (14) */
    *g0    = (p11[i] * rho   + p21[i]) / (rhoSq + q11[i] * rho + q21[i]);
    *gInf  = (p12[i] * rho   + p22[i]) / (rhoSq + q12[i] * rho + q22[i]);
    fc_tmp = (p13[i] * rhoSq + p23[i] * rho + p33[i]) / (rhoSq + q13[i] * rho + q23[i]);
    
    /* denormalize fc = fc * sos/(2pi*a) */
    *fc = (float)fc_tmp * sosDiv2PiA;
}

/*
 * Interpolate (linear) the high shelf parameters generated by calcHighShelfParams()
 * which is called twice to generate the high shelf parameters for the nearest thetas
 * in the lookup table. */
void interpHighShelfParams
(
    float theta,   /* ipsilateral azimuth, on the inter-aural axis [0, 180] (deg) */
    float rho,     /* distance, normalized to head radius, >= 1 */
    /* output */
    float* iG0,
    float* iGInf,
    float* iFc
)
{
    int theta_idx_lower, theta_idx_upper;
    float ifac;
    float thetaDiv10;
    float g0_1, g0_2;       /* high shelf gain at DC */
    float gInf_1, gInf_2;   /* high shelf gain at inf */
    float fc_1, fc_2;       /* high shelf cutoff frequency */
    
    // TODO: range checking - clip theta and rho to valid range
    /* linearly interpolate DC gain, HF gain, center freq at theta */
    // TODO: rethink this indexing logic...
    thetaDiv10 = theta / 10.f;
    theta_idx_lower = (int)thetaDiv10;      /* Table is in 10 degree steps, floor(x/10) gets lower index */
    theta_idx_upper = theta_idx_lower + 1;
    if(theta_idx_upper == numAz_table) {    // TODO: if instead check theta_idx_upper => numAz_table, could clip the value > 180 here
        theta_idx_upper = theta_idx_lower;
        theta_idx_lower = theta_idx_lower - 1;
    }
    
    calcHighShelfParams(theta_idx_lower, rho, &g0_1, &gInf_1, &fc_1);
    calcHighShelfParams(theta_idx_upper, rho, &g0_2, &gInf_2, &fc_2);

    ifac = thetaDiv10 - theta_idx_lower;  /* interpolation factor between table steps */
    *iG0   = interpolate_lin(g0_1,   g0_2,   ifac);
    *iGInf = interpolate_lin(gInf_1, gInf_2, ifac);
    *iFc   = interpolate_lin(fc_1,   fc_2,   ifac);
}

/*
 * Generate IIR coefficients from the shelf parameters generated by calcHighShelfParams() and
 * interpHighShelfParams(). */
void calcIIRCoeffs
(
    /* Input */
    float g0,      /* high shelf dc gain */
    float gInf,    /* high shelf high gain */
    float fc,      /* high shelf center freq */
    float fs,      /* sample rate */
    /* Output */
    float* b0,     /* IIR coeffs */
    float* b1,
    float* a1
)
{
    float v0;
    float g0_mag;
    float tanF;
    float v0tanF;
    float a_c;
    float v;
    float va_c;
    
    v0     = db2mag(gInf);              /* Eq. (12), (10), and (11) */
    g0_mag = db2mag(g0);                // TODO: revisit; does g0, gInf need to be in dB?
    tanF   = tanf((headDim / fs) * fc); // TODO: this /fs calc can be optimized out with precomputed head dimension
    v0tanF = v0 * tanF;
    a_c    = (v0tanF - 1.f) / (v0tanF + 1.f);
    
    v    = (v0 - 1.f) * 0.5f;           /* Eq (10) */
    va_c = v * a_c;
    *b0  = g0_mag * (v - va_c + 1.f);   /* = V*(1 - a_c) + 1   */
    *b1  = g0_mag * (va_c - v + a_c);   /* = V*(a_c - 1) + a_c */
    *a1  = a_c;
}

void applyDVF
(
    float theta,        /* ipsilateral azimuth, on the inter-aural axis [0, 180] (deg) */
    float rho,          /* distance, normalized to head radius, >= 1 */
    float* in_signal,
    int nSamples,       /* Number of samples to process */
    float fs,           /* Sample rate */
    float* wz,          /* (&) Filter coefficients to be passed to the next block */
    float* out_signal
)
{
    float b[2] = {0.f, 0.f};
    float a[2] = {1.f, 0.f};
    float iG0, iGInf, iFc;
    
    interpHighShelfParams(theta, rho, &iG0, &iGInf, &iFc);
    calcIIRCoeffs(iG0, iGInf, iFc, fs, &b[0], &b[1], &a[1]);
    applyIIR(in_signal, nSamples, 2, &b[0], &a[0], wz, out_signal);
}

void convertFrontalDoAToIpsilateral
(
    float thetaFront, /* DOA wrt 0˚ forward-facing  [deg, (-180, 180)] */
    float* ipsiDoaLR  /* pointer to 2-element array of left and right ear DoAs wrt interaural axis */
)
{
    float thetaL;
    
    // TODO: clamp thetaFront (-180, 180)
    thetaL = fabsf(90.f - thetaFront);
    if (thetaL > 180.f) {
        thetaL = 360.f - thetaL;
    }
    
    ipsiDoaLR[0] = thetaL;
    ipsiDoaLR[1] = 180.f - thetaL; // thetaR
}