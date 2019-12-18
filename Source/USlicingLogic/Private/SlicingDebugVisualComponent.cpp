// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingDebugVisualComponent.h"
#include "SlicingBladeComponent.h"
#include "SlicingTipComponent.h"
#include "SlicingComponent.h"
#include "SlicingHelper.h"

#include "DrawDebugHelpers.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

USlicingDebugVisualComponent::USlicingDebugVisualComponent()
{
	// Enables the usage of the TickComponent function
	PrimaryComponentTick.bCanEverTick = true;
}

void USlicingDebugVisualComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SlicingLogicModule = &FModuleManager::Get().LoadModuleChecked<FUSlicingLogicModule>("USlicingLogic");
	
	// Get all the relevant instances of the slicing object
	SlicingObject = (AStaticMeshActor*)GetOwner();
	BladeComponent =
		FSlicingHelper::GetSlicingComponent<USlicingBladeComponent>(SlicingObject->GetStaticMeshComponent());
	TipComponent =
		FSlicingHelper::GetSlicingComponent<USlicingTipComponent>(SlicingObject->GetStaticMeshComponent());

	if (SlicingLogicModule->bEnableDebugConsoleOutput)
	{
		if (BladeComponent != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SLICING: The blade component has been loaded into the world"));
		}
		if (TipComponent != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SLICING: The tip component has been loaded into the world"));
		}
	}
}

// Called every frame (only used for the debugging-visuals)
void USlicingDebugVisualComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only display the debugging visuals if the slicing components exist
	if (BladeComponent == nullptr || TipComponent == nullptr)
	{
		return;
	}

	if (SlicingLogicModule->bEnableDebugShowComponents)
	{
		DrawComponents();
	}

	// The slicing related debugging is only needed when cutting
	if (!BladeComponent->bIsCurrentlyCutting)
	{
		if (bWasCutting)
		{
			bWasCutting = false;
			// Only remove the debug visuals when the cutting was aborted
			// (so that visuals like the DebugShowComponents are still seen outside of cutting)
			FlushPersistentDebugLines(this->GetWorld());
		}
		return;
	}
	else
	{
		bWasCutting = true;
	}

	if (SlicingLogicModule->bEnableDebugShowPlane)
	{
		DrawSlicingPlane();
		DrawCuttingEntrancePoint();
		DrawCuttingExitPoint();
	}
	if (SlicingLogicModule->bEnableDebugShowTrajectory)
	{
		DrawCuttingTrajectory();
	}
}

void USlicingDebugVisualComponent::DrawSlicingPlane()
{
	FPlane SlicingPlane = FPlane(BladeComponent->SlicingObject->GetComponentLocation(), BladeComponent->GetUpVector());

	// This size is actually about double the size of the component, but this is just the amount we need
	float BladeComponentSize;
	// Both of those variables are unused and not needed here
	FVector BladeComponentOrigin, BladeComponentExtends;
	UKismetSystemLibrary::GetComponentBounds(BladeComponent, BladeComponentOrigin, BladeComponentExtends, BladeComponentSize);

	DrawDebugSolidPlane(GetWorld(), SlicingPlane, BladeComponent->CutComponent->GetComponentLocation(),
		BladeComponentSize,	FColor::Red, false);
}

void USlicingDebugVisualComponent::DrawCuttingEntrancePoint()
{
	FVector ComponentPosition = UKismetMathLibrary::TransformLocation(
		BladeComponent->CutComponent->GetComponentTransform(), BladeComponent->RelativeLocationToCutComponent);

	DrawDebugBox(GetWorld(), ComponentPosition, FVector(3, 3, 3), BladeComponent->CutComponent->GetComponentQuat(),
		FColor::Green);
}

void USlicingDebugVisualComponent::DrawCuttingExitPoint()
{
	FVector EndPosition = UKismetMathLibrary::TransformLocation(BladeComponent->GetComponentTransform(),
		FVector(0, 1000, 0));

	FHitResult Hits;
	BladeComponent->CutComponent->LineTraceComponent(Hits, EndPosition, BladeComponent->GetComponentLocation(),
		FCollisionQueryParams::DefaultQueryParam);
	
	DrawDebugBox(GetWorld(), Hits.Location, FVector(3, 3, 3), BladeComponent->CutComponent->GetComponentQuat(),
		FColor::Red);
}

void USlicingDebugVisualComponent::DrawCuttingTrajectory()
{
	FPlane SlicingPlane = FPlane(BladeComponent->SlicingObject->GetComponentLocation(), BladeComponent->GetUpVector());

	DrawDebugPoint(GetWorld(), BladeComponent->SlicingObject->GetSocketLocation(USlicingComponent::SocketBladeName),
		2, FColor::Purple, true);
}

void USlicingDebugVisualComponent::DrawComponents()
{
	// Draw all the slicing components
	DrawComponent(BladeComponent);
	DrawComponent(TipComponent);
}

void USlicingDebugVisualComponent::DrawComponent(USlicingComponent* SlicingComponent)
{
	DrawDebugBox(SlicingComponent->GetWorld(), SlicingComponent->GetComponentLocation(),
		SlicingComponent->GetScaledBoxExtent(), SlicingComponent->GetComponentRotation().Quaternion(),
		FColor::Green, false, 0.01f);
}