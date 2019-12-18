// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "SlicingComponent.h"

#include "SlicingTipComponent.generated.h"

class USlicingBladeComponent;

UCLASS()
class USLICINGLOGIC_API USlicingTipComponent: public USlicingComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	//* The tip component that is attached to the same SlicingComponent
	USlicingBladeComponent* BladeComponent;
	//* The component that is currently started overlapping with the tip
	UPrimitiveComponent* OverlappedComponent;

	bool bEnteredCurrentlyCutObject = false;

	/**** Implementation of the overlap events to detect early abortion of the slicing ****/
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};