// Sobel Outline Filter - Fragment Shader
 #version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageTexture;

uniform vec4 _OutlineColor;
uniform float _OutlineStrength;


// Sobel算子
mat3 sobelX = mat3( 
    -1.0, 0.0, 1.0, 
    -2.0, 0.0, 2.0, 
    -1.0, 0.0, 1.0 
);
mat3 sobelY = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
    -1.0, -2.0, -1.0 
);

// 计算梯度
float CalculateGradient() // calculates silhouette edges
{
	float GX = 0;
    float GY = 0;
	float sample;
    // Y = 0.298R + 0.612G + 0.117B
	float center = 0.298 * texelFetch(imageTexture, ivec2(gl_FragCoord), 0).r
                 + 0.612 * texelFetch(imageTexture, ivec2(gl_FragCoord), 0).g
                 + 0.117 * texelFetch(imageTexture, ivec2(gl_FragCoord), 0).b;

	// fetch the 3x3 neighbourhood
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) 
		{
            sample = 0.298 * texelFetch(imageTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).r
                  +  0.612 * texelFetch(imageTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).g
                  +  0.117 * texelFetch(imageTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).b;
			// sample = texelFetch( depthTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).r;

			// I[i][j] = abs(sample - center) * contourFilter[i][j] / 8.0;
            GX += abs(sample) * sobelX[i][j] / 8.0;
            GY += abs(sample) * sobelY[i][j] / 8.0;
		}
	}

    return sqrt(GX * GX + GY * GY);
}

// 主函数
void main(void)
{
	float SobelColor = CalculateGradient();
	// FragColor = vec4(SobelColor* 2.0, SobelColor*2.0, SobelColor*2.0, 1.0);
    vec3 black = vec3(0.0,0.0,0.0);
    vec3 color = mix(black, _OutlineColor.rgb, SobelColor*_OutlineStrength);
    FragColor = vec4(color,1.0);
}