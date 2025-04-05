// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicSplitCamGameMode.h"
#include "UObject/ConstructorHelpers.h"

ADynamicSplitCamGameMode::ADynamicSplitCamGameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_TopdownPlayerController"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    // find both render target material for both player
    for (int i = 0; i < RTPath.Num(); ++i)
    {
        FString path = RTPath[i];
        ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTarget(*path);
        if (!RenderTarget.Succeeded())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find render targets"));
            return;
        }

        RenderTargets.Emplace(RenderTarget.Object);
    }
}

void ADynamicSplitCamGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (!NewPlayer)
        return;

    int32 PlayerIndex = GetGameInstance()->GetNumLocalPlayers() - 1;
    UE_LOG(LogTemp, Verbose, TEXT("Player: %d"), PlayerIndex);

    ADynamicSplitCamCharacter *PlayerCharacter = Cast<ADynamicSplitCamCharacter>(NewPlayer->GetPawn());
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("PostLogin: No pawn found for player %d"), PlayerIndex);
        return;
    }

    Players.Emplace(PlayerCharacter);

    if (PlayerIndex == 0)
    {
        Player1Location = NewPlayer->GetPawn()->GetActorLocation();
    }
    else
    {
        Players[PlayerIndex]->SetActorLocation(Players[PlayerIndex - 1]->GetActorLocation() + FVector(0, 200, 0));
    }

    if (RenderTargets.Num() != RTPath.Num())
    {
        return;
    }

    // Update render target screen size
    FIntPoint ScreenSize = GEngine->GameViewport->Viewport->GetSizeXY();
    UpdateRTSize(ScreenSize);

    PlayerCharacter->SceneCapture->TextureTarget = RenderTargets[PlayerIndex];
}

void ADynamicSplitCamGameMode::BeginPlay()
{
    Super::BeginPlay();

    // register viewport resize event
    if (GEngine && GEngine->GameViewport)
    {
        FViewport *Viewport = GEngine->GameViewport->Viewport;
        if (Viewport)
        {
            Viewport->ViewportResizedEvent.AddUObject(this, &ADynamicSplitCamGameMode::OnViewportResized);
        }
    }
}

void ADynamicSplitCamGameMode::OnViewportResized(FViewport *Viewport, uint32 val)
{
    UpdateRTSize(Viewport->GetSizeXY());
}

void ADynamicSplitCamGameMode::UpdateRTSize(FIntPoint NewSize)
{
    UE_LOG(LogTemp, Verbose, TEXT("Update render targets size: %d, %d"), NewSize.X, NewSize.Y);
    for (int i = 0; i < RenderTargets.Num(); ++i)
    {
        UTextureRenderTarget2D *RenderTarget = RenderTargets[i];
        if (!RenderTarget)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find render target"));
            return;
        }
        RenderTarget->ResizeTarget(NewSize.X, NewSize.Y);
    }
}
