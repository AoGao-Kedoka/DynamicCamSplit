// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "DynamicSplitCamCharacter.h"
#include "DynamicSplitCamGameMode.generated.h"

UCLASS(minimalapi)
class ADynamicSplitCamGameMode : public AGameModeBase
{
	GENERATED_BODY()

	void OnViewportResized(FViewport* Viewport, uint32 val);

    void UpdateRTSize(FIntPoint NewSize);

public:
	ADynamicSplitCamGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;

	TArray<ADynamicSplitCamCharacter*> Players;

private:
	FVector Player1Location;
	TArray<TObjectPtr<UTextureRenderTarget2D>> RenderTargets;
	TArray<FString> RTPath = { TEXT("/Game/CamSplit/RT_Player1"), TEXT("/Game/CamSplit/RT_Player2") };
};



