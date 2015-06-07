
matrix ViewMatrix;
matrix ViewProjMatrix;

vector AmbientMtrl;
vector DiffuseMtrl;
vector LightDirection;
vector DiffuseLightIntensity = {0.0f, 0.0f, 1.0f, 1.0f};
vector AmbientLightIntensity = {0.0f, 0.0f, 0.2f, 1.0f};

// Input and Output structures.
struct VS_INPUT
{
    vector position : POSITION;
    vector normal   : NORMAL;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    vector diffuse  : COLOR;
};

VS_OUTPUT Main(VS_INPUT input)
{
    // zero out all members of the output instance.
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Transform position to homogeneous clip space
    // and store in the output.position member. 
    output.position = mul(input.position, ViewProjMatrix);

    // Transform lights and normals to view space.  Set w
    // componentes to zero since we're transforming vectors 
    // here and not points.
    LightDirection.w = 0.0f;
    input.normal.w   = 0.0f;
    LightDirection   = mul(LightDirection, ViewMatrix);
    input.normal     = mul(input.normal,   ViewMatrix);

    // Compute cosine of the angle between light and normal.
    float s = dot(LightDirection, input.normal);
    if ( s < 0.0f )
        s = 0.0f;

    output.diffuse = (AmbientMtrl * AmbientLightIntensity) +
                     (s * (DiffuseLightIntensity * DiffuseMtrl));
    
    return output;
}





