
struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float fill;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 reflect;
};

void ComputeDirectionalLight(Material mat, DirectionalLight l,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// 初始化输出
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 光向量与照射方向相反
	float3 lightVec = -l.direction;

	// 添加环境光
	ambient = mat.ambient * l.ambient;

	// 添加漫反射光和镜面光
	float diffuseFactor = dot(lightVec, normal);
	
	// 展开，避免动态分支
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * l.diffuse;
		specular = specFactor * mat.specular * l.specular;
	}
}