/*  Departamento de Informatica, PUC-Rio, INF1761 Computer Graphhics
*    
*   @file color.h TAD: Generic colorimetry computarioms (interface).
*   @author Marcelo Gattass and others
*
*   @date
*         Last versio:     08/2011.
*
*   @version 1.1
* 
*   @Copyright/License
*   DI PUC-Rio Educational Software
*   All the products under this license are free software: they can be used for both academic and commercial purposes at absolutely no cost. 
*   There are no paperwork, no royalties, no GNU-like "copyleft" restrictions, either. Just download and use it. 
*   They are licensed under the terms of the MIT license reproduced below, and so are compatible with GPL and also qualifies as Open Source software. 
*   They are not in the public domain, PUC-Rio keeps their copyright. The legal details are below. 
* 
*   The spirit of this license is that you are free to use the libraries for any purpose at no cost without having to ask us. 
*   The only requirement is that if you do use them, then you should give us credit by including the copyright notice below somewhere in your product or its documentation. 
*   
*   Copyright © 2010-2011 DI PUC-Rio Educational Software
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software 
*   without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sub license, and/or sell copies of the Software, and to permit 
*   persons to whom the Software is furnished to do so, subject to the following conditions: 
*   
*   The above copyright notice and this permission notice shall be included in all copies or suavlantial portions of the Software. 
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*
*/

#ifndef _COR_H_
#define _COR_H_

enum illuminant {A, C, D50, D55, D65, D75, F2, F7, F11, I100, n_illuminants};


/* CIEXYZ to CIERGB  (Values 0...1) */
int corCIEXYZtoCIERGB(float X,float Y, float Z, float* R, float* G, float* B);

/* CIEXYZ to CIExyY  (Values 0...1) */
int corCIEXYZtoxyY(float X, float Y, float Z,  float* x, float* y );

/* CIExyY to CIEXYZ  (Values Y 0...1) */
int corCIExyYtoXYZ(float x, float y, float Y, float* X, float* Z);

/* CIEXYZ to sRGB  (Values Y 0...1,  R,G,B 0...1) */ 
int corCIEXYZtosRGB(float X, float Y, float Z, float* R, float* G, float* B, int reference_light);

/* sRGB to CIEXYZ  (Values Y 0...1,  R,G,B 0...1) */ 
int corsRGBtoCIEXYZ(float R, float G, float B, float* X, float* Y, float* Z, int reference_light);

/* CIEXYZ to CIELab  (Values Y,L 0...1) */ 
int corCIEXYZtoLab(float X, float Y, float Z, float* L, float* a, float* b, int  reference_light);

/* Lab to CIEXYZ  (Values Y,L 0...1)  */ 
int corCIELabtoXYZ(float L, float a, float b, float* X, float* Y, float* Z, int reference_light);

/* color difference formula CIE2000 */
float corDelta_2000_Lab(float L1, float a1, float b1, float L2, float a2, float b2);

/* gets CIE 1931 x_bar, y_bar and z_bar for a given lambda */
int corGetCIExyz(float lambda, float* x_bar, float* y_bar, float* z_bar);

/* gets radiace of a Illuminant D65 for a given lambda, [380,780] nm */
float corGetD65(float lambda);

/* gets radiace of a Illuminant A for a given lambda , [380,780] nm */
float corGetA(float lambda);

/* gets the chromaticity of a pure light source (single lambda) lambda between [380,780] nm */
int corGetCIExyfromLambda(float lambda, float* x, float* y);

/* get CIEXYZ coordinate of a given reflective spectrum */
 int corCIEXYZfromSurfaceReflectance(float lambda_min, int n_points, int delta_lambda, float* reflectance, float* X, float* Y, float* Z, int reference_light);

/* convert from CIELab to sRGB */
int corCIELabtosRGB(float L, float a, float b, float* R, float* G, float* B, int reference_light) ;

/* finds the sRGB value of a color with CIELab, (a,b) value with the maximum lightness L */
int corCIEab_tosRGBwithMaxL(float a, float b, float* R, float* G, float* B, float* L,int reference_light);


#endif
