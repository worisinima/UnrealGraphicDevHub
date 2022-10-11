
#pragma once

#include "CoreMinimal.h"
#include "Renderer/Private/SceneRendering.h"

class FSimpleRenderer
{
public:
	FSimpleRenderer();
	~FSimpleRenderer();

	void Render(FPostOpaqueRenderParameters& InParameters);

private:
	FDelegateHandle RenderHandle;
	class FShadingDevModule* ThisModule;
};