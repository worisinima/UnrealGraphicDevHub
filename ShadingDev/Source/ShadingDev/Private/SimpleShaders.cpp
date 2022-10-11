
#include "ShadingDev/Public/SimpleShaders.h"

#include "RendererInterface.h"
#include "RenderGraphResources.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"
#include "SceneRenderTargetParameters.h"
#include "RenderResource.h"
#include "EngineModule.h"
#include "Renderer/Private/PostProcess/SceneRenderTargets.h"
#include "Renderer/Private/SceneRendering.h"

class FSimpleComputeShader : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FSimpleComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FSimpleComputeShader, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, RWOutput)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, InDataBuffer)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
};
IMPLEMENT_GLOBAL_SHADER(FSimpleComputeShader, "/ShadingDev/SimpleShaders.usf", "SimpleComputeCS", SF_Compute);


FSimpleRenderer::FSimpleRenderer()
	:ThisModule(nullptr)
{
	RenderHandle = GetRendererModule().RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateRaw(this, &FSimpleRenderer::Render));
	ThisModule = &FModuleManager::GetModuleChecked<FShadingDevModule>("ShadingDev");
}

FSimpleRenderer::~FSimpleRenderer()
{
	GetRendererModule().RemovePostOpaqueRenderDelegate(RenderHandle);
	RenderHandle.Reset();

	ThisModule = nullptr;
}

void FSimpleRenderer::Render(FPostOpaqueRenderParameters& InParameters)
{
	FRDGBuilder& GraphBuilder = *InParameters.GraphBuilder;
	const FViewInfo& View = *InParameters.View;

	const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	const TShaderMapRef<FSimpleComputeShader> ComputeShader(GlobalShaderMap);

	RDG_EVENT_SCOPE(GraphBuilder, "Test");

	FRDGTextureDesc TextDesc = FRDGTextureDesc::Create2D(
		FIntPoint(512, 512),
		EPixelFormat::PF_FloatRGBA,
		FClearValueBinding::Black,
		TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV
	);
	FRDGTextureRef Texture = GraphBuilder.CreateTexture(TextDesc, TEXT("RadiationVolumeTexture"));

	TArray<FLinearColor> Data;
	Data.Add(FLinearColor(1, 0, 0, 1));
	Data.Add(FLinearColor(0, 1, 0, 1));
	Data.Add(FLinearColor(0, 0, 1, 1));
	Data.Add(FLinearColor(1, 1, 1, 1));
	Data.Add(FLinearColor(1, 0, 1, 1));

	FRDGBufferRef Buffer = CreateStructuredBuffer(
		GraphBuilder,
		TEXT("MyCS.RadiationDataBuffer"),
		sizeof(FLinearColor),
		Data.Num(),
		Data.GetData(),
		sizeof(FLinearColor) * Data.Num()
	);

	// Compute Shader Parameters and Textures
	FSimpleComputeShader::FParameters* CSParams = GraphBuilder.AllocParameters<FSimpleComputeShader::FParameters>();
	CSParams->RWOutput = GraphBuilder.CreateUAV(Texture);
	CSParams->InDataBuffer = GraphBuilder.CreateSRV(Buffer, PF_FloatRGBA);
	uint32 GroupCount = FMath::DivideAndRoundUp((uint32)512, (uint32)32);

	// Add Compute Pass, Render Radiation Data to VolumeTexture
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("TestComputeVolumeTexture"),
		ComputeShader,
		CSParams,
		FIntVector(GroupCount, GroupCount, 1)
	);
}