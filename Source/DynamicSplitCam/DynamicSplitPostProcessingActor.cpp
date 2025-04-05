// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicSplitPostProcessingActor.h"
#include "DynamicSplitCamGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADynamicSplitPostProcessingActor::ADynamicSplitPostProcessingActor() {
  // Set this actor to call Tick() every frame.  You can turn this off to
  // improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
  PostProcessComponent->bUnbound = true;
  PostProcessComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADynamicSplitPostProcessingActor::BeginPlay() {
  Super::BeginPlay();

  // fetch materials
  if (PostProcessingMaterial) {
    PostProcessingMaterialInstance =UMaterialInstanceDynamic::Create(PostProcessingMaterial, this);
    PostProcessComponent->AddOrUpdateBlendable(PostProcessingMaterialInstance);
  }

  // fetch players
  if (auto gm = Cast<ADynamicSplitCamGameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
    Players = &gm->Players;
  }

  UpdateSplitPosition(1.0f);
}

void ADynamicSplitPostProcessingActor::UpdateSplitPosition(
    float splitPosition) {
  PostProcessingMaterialInstance->SetScalarParameterValue(FName("SplitPosition"), splitPosition);
}

bool ADynamicSplitPostProcessingActor::CheckPlayerDistanceToThreshold() {
  // get players distance
  if (Players->Num() < 2) {
    return false;
  }

  float distance = FMath::Abs((*Players)[0]->GetActorLocation().Y -
                              (*Players)[1]->GetActorLocation().Y);
  return distance > SplitThreshold;
}

// Called every frame
void ADynamicSplitPostProcessingActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  if (CheckPlayerDistanceToThreshold()) {
    UpdateSplitPosition(0.5f);
    (*Players)[0]->SceneCapture->SetRelativeLocation(FVector::ZeroVector);
  } else {
    UpdateSplitPosition(1.0f);

    // update camera position
    FVector cameraMiddlePoint =
        ((*Players)[0]->SceneCapture->GetComponentLocation() +
         (*Players)[1]->SceneCapture->GetComponentLocation()) /
        2;

    FVector currentLocation =(*Players)[0]->SceneCapture->GetComponentLocation();
    FVector newLocation =FMath::Lerp(currentLocation, cameraMiddlePoint, 0.05f);
    (*Players)[0]->SceneCapture->SetWorldLocation(newLocation);
  }
}
