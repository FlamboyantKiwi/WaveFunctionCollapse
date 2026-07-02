#pragma once

#include "CoreMinimal.h"
#include "TileDataAsset.h" 
#include "WFCTypes.generated.h"

/**
 * FWFCCell
 * Represents a single grid coordinate and its current state of superposition.
 */
USTRUCT(BlueprintType)
struct FWfcCell {
    GENERATED_BODY()

public:
    // The physical grid coordinate (X, Y)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WFC|Cell")
    FIntPoint Coordinate;

    // The remaining valid tiles that can be placed here (Superposition)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WFC|Cell")
    TArray<UTileDataAsset*> PossibleTiles;

    // Has this cell been reduced to exactly one final tile?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WFC|Cell")
    bool bIsCollapsed;

    // Default constructor (Required by Unreal Engine's reflection system)
    FWfcCell() {
        Coordinate = FIntPoint::ZeroValue;
        bIsCollapsed = false;
    }

    // Custom constructor to easily set up a cell
    FWfcCell(FIntPoint InCoordinate, const TArray<UTileDataAsset*>& InitialPossibilities) {
        Coordinate = InCoordinate;
        PossibleTiles = InitialPossibilities;
        bIsCollapsed = false;
    }

    // Helper function to quickly get the current Entropy (number of possibilities)
    int32 GetEntropy() const {
        return PossibleTiles.Num();
    }
};
