// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Public/WFCTypes.h"
#include "WFCGenerator.generated.h"

UCLASS()
class AWfcGenerator : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWfcGenerator();
    // Returns true if the grid is fully collapsed (finished), false if still working
    UFUNCTION(BlueprintCallable, Category = "WFC|Execution")
    bool StepWfc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Settings", meta = (AllowPrivateAccess = "true"))
    FIntPoint GridSize = FIntPoint(10, 10);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Settings", meta = (AllowPrivateAccess = "true"))
    float TileSpacing = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC|Settings", meta = (AllowPrivateAccess = "true"))
    TArray<UTileDataAsset*> AvailableTiles;

    // -------------------------------------------------------------------------
    // WFC STATE & LOGIC
    // -------------------------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WFC|Debug", meta = (AllowPrivateAccess = "true"))
    TArray<FWfcCell> Grid;

    UFUNCTION(BlueprintCallable, Category = "WFC|Execution")
    void GenerateGrid();

    UFUNCTION(BlueprintCallable, Category = "WFC|Execution")
    void InitializeGrid();

    UFUNCTION(BlueprintCallable, Category = "WFC|Execution")
    void SpawnActorsFromGrid();

    void RunWfc();
    int32 GetLowestEntropyCellIndex() const;
    void CollapseCell(int32 CellIndex);
    void PropagateConstraints(int32 CollapsedCellIndex);

    // -------------------------------------------------------------------------
    // HELPER FUNCTIONS
    // -------------------------------------------------------------------------

    int32 GetIndexFromGridPos(int32 X, int32 Y) const;
    bool IsValidCoordinate(int32 X, int32 Y) const;
    bool IsTileStillValid(const FWfcCell& CurrentCell, UTileDataAsset* TileToEvaluate) const;
};