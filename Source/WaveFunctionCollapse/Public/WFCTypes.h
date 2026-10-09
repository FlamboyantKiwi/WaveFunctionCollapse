#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.generated.h"

// Forward declaration to prevent circular dependency crashes
class UTileDataAsset;

// ---------------------------------------------------------
// ENUMS & MATH
// ---------------------------------------------------------

UENUM(BlueprintType)
enum class EWfcRuleCondition : uint8 {
    Avoidant     UMETA(DisplayName = "Must Not Be Next To"),
    Required     UMETA(DisplayName = "Must Be Next To")
};

UENUM(BlueprintType)
enum class ENeighborDirection : uint8 {
    None		UMETA(DisplayName = "None"),
    Top			UMETA(DisplayName = "Top"),
    TopRight	UMETA(DisplayName = "Top Right"),
    Right		UMETA(DisplayName = "Right"),
    BottomRight	UMETA(DisplayName = "Bottom Right"),
    Bottom		UMETA(DisplayName = "Bottom"),
    BottomLeft	UMETA(DisplayName = "Bottom Left"),
    Left		UMETA(DisplayName = "Left"),
    TopLeft		UMETA(DisplayName = "Top Left")
};

// Lookup table mapped to the uint8 values of ENeighborDirection.
static inline const FIntPoint DirectionOffsets[9] = {
    FIntPoint(0, 0),   // 0: None
    FIntPoint(0, 1),   // 1: Top 
    FIntPoint(1, 1),   // 2: Top Right
    FIntPoint(1, 0),   // 3: Right
    FIntPoint(1, -1),  // 4: Bottom Right
    FIntPoint(0, -1),  // 5: Bottom
    FIntPoint(-1, -1), // 6: Bottom Left
    FIntPoint(-1, 0),  // 7: Left
    FIntPoint(-1, 1)   // 8: Top Left
};

// A clean iterator array skipping 'None'
static inline const ENeighborDirection ValidDirections[8] = {
    ENeighborDirection::Top,
    ENeighborDirection::TopRight,
    ENeighborDirection::Right,
    ENeighborDirection::BottomRight,
    ENeighborDirection::Bottom,
    ENeighborDirection::BottomLeft,
    ENeighborDirection::Left,
    ENeighborDirection::TopLeft
};

class FWfcMath {
public:
    // Returns the exact grid offset for a specific enum direction (O(1) lookup)
    static inline FIntPoint GetOffsetForDirection(ENeighborDirection Direction) {
        return DirectionOffsets[static_cast<uint8>(Direction)];
    }
    // Checks if a given grid offset matches any of the directions selected in a rule array
    static inline bool MatchesAnySelectedDirection(FIntPoint Offset, const TArray<ENeighborDirection>& AllowedDirections) {
        for (ENeighborDirection Dir : AllowedDirections) {
            if (Dir == ENeighborDirection::None) continue;

            if (Offset == GetOffsetForDirection(Dir)) {
                return true;
            }
        }
        return false;
    }
};

// ---------------------------------------------------------
// STRUCTS
// ---------------------------------------------------------

USTRUCT(BlueprintType)
struct FTileRuleWrapper {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule")
    UTileDataAsset* TargetTile = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule")
    EWfcRuleCondition Condition = EWfcRuleCondition::Avoidant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule")
    TArray<ENeighborDirection> Directions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule", meta = (EditCondition = "Condition == EWfcRuleCondition::Required", EditConditionHides, ClampMin = "1", ClampMax = "8"))
    int32 RequiredCount = 1;
};

/** * FWFCCell
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