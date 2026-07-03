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
enum class EWfcDirection : uint8 {
    Orthogonal   UMETA(DisplayName = "Up/Down/Left/Right"),
    Diagonal     UMETA(DisplayName = "Corners Only"),
    Any          UMETA(DisplayName = "Any Neighbor")
};

// A single, constant array of all 8 possible directions
static inline const FIntPoint AllDirections[8] = {
    FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0), // 0-3: Orthogonal
    FIntPoint(1, 1), FIntPoint(-1, 1), FIntPoint(1, -1), FIntPoint(-1, -1) // 4-7: Diagonal
};

class FWfcMath {
public:
    // Returns a memory-free "window" into the hardcoded array
    static inline TArrayView<const FIntPoint> GetDirectionOffsets(EWfcDirection DirectionType) {
        switch (DirectionType) {
        case EWfcDirection::Orthogonal: return TArrayView<const FIntPoint>(AllDirections, 4);
        case EWfcDirection::Diagonal:   return TArrayView<const FIntPoint>(AllDirections + 4, 4);
        case EWfcDirection::Any:
        default:                        return TArrayView<const FIntPoint>(AllDirections, 8);
        }
    }
    //Lets the math library decide if an offset matches a specific rule type
    static inline bool MatchesDirection(FIntPoint Offset, EWfcDirection RuleDirection) {
        if (RuleDirection == EWfcDirection::Any) return true;

        // The absolute sum trick (1 = Orthogonal, 2 = Diagonal)
        int32 AbsSum = FMath::Abs(Offset.X) + FMath::Abs(Offset.Y);

        if (RuleDirection == EWfcDirection::Orthogonal) return AbsSum == 1;
        if (RuleDirection == EWfcDirection::Diagonal) return AbsSum == 2;
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
    EWfcDirection Direction = EWfcDirection::Orthogonal;

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