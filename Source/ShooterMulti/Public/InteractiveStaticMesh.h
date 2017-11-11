// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "InteractiveStaticMesh.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AInteractiveStaticMesh : public AInteractiveObject
{
GENERATED_BODY()

public:
	AInteractiveStaticMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inherited)
	UStaticMeshComponent* StaticMesh;

protected:

	void OnSelectableEnter_Implementation(AActor* Actor) override;
	void OnSelectableExit_Implementation(AActor* Actor) override;
};
