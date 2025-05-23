// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "DynamicSplitCamCharacter.h"
#include "DynamicSplitPostProcessingActor.generated.h"

UCLASS()
class DYNAMICSPLITCAM_API ADynamicSplitPostProcessingActor : public AActor
{
	GENERATED_BODY()

	UMaterialInstanceDynamic* PostProcessingMaterialInstance;

	UPostProcessComponent* PostProcessComponent;

	TArray<ADynamicSplitCamCharacter*>* Players;

	void UpdateSplitPosition(float splitPosition);

	bool CheckPlayerDistanceToThreshold();
	
public:	
	// Sets default values for this actor's properties
	ADynamicSplitPostProcessingActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcessing")
	UMaterial* PostProcessingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcessing")
	float SplitThreshold;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
