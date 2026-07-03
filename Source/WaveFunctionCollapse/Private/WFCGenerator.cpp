// Fill out your copyright notice in the Description page of Project Settings.

#include "WFCGenerator.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "TileDataAsset.h"

// Sets default values
AWfcGenerator::AWfcGenerator() {
    PrimaryActorTick.bCanEverTick = false; 
}

// Called when the game starts or when spawned
void AWfcGenerator::BeginPlay() {
    Super::BeginPlay();
}

// Called every frame
void AWfcGenerator::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void AWfcGenerator::GenerateGrid() {
    if (AvailableTiles.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("WFC Generator has no Available Tiles set!"));
        return;
    }
    InitializeGrid();
    RunWfc();
    SpawnActorsFromGrid();
}

void AWfcGenerator::InitializeGrid() {
    // Pre-allocate memory to prevent the TArray from resizing during the loop
    Grid.Empty(GridSize.X * GridSize.Y);

    // Fill array with our 2D coordinates
    for (int32 Y = 0; Y < GridSize.Y; Y++) {
        for (int32 X = 0; X < GridSize.X; X++) {
            // Give every cell the full list of available tiles to start
            Grid.Add(FWfcCell(FIntPoint(X, Y), AvailableTiles));
        }
    }
}

void AWfcGenerator::RunWfc() {
    int32 FailsafeCounter = 0;
    int32 MaxIterations = GridSize.X * GridSize.Y * 10; // Prevent infinite loops
    bool bIsFinished = false;

    while (!bIsFinished && FailsafeCounter < MaxIterations) {
        bIsFinished = StepWfc();
        FailsafeCounter++;
    }

    if (FailsafeCounter >= MaxIterations) {
        UE_LOG(LogTemp, Warning, TEXT("WFC Loop hit the failsafe! Grid may not be fully collapsed."));
    }
}

bool AWfcGenerator::StepWfc() {
    int32 NextCellIndex = GetLowestEntropyCellIndex();

    // If -1, the grid is fully collapsed (or a contradiction occurred)
    if (NextCellIndex == -1)    return true; // We are done

    CollapseCell(NextCellIndex);
    PropagateConstraints(NextCellIndex);

    return false; // Not done yet, keep stepping
}

int32 AWfcGenerator::GetLowestEntropyCellIndex() const {
    int32 LowestEntropy = 999999;
    TArray<int32> LowestEntropyCandidates;
    LowestEntropyCandidates.Reserve(Grid.Num());

    for (int32 Index = 0; Index < Grid.Num(); Index++) {
        if (Grid[Index].bIsCollapsed) continue; // Skip finalized cells
        int32 Entropy = Grid[Index].GetEntropy();

        // Contradiction: A cell has 0 possibilities left before it was collapsed
        if (Entropy == 0) {
            UE_LOG(LogTemp, Error, TEXT("WFC Contradiction at Cell %s! No possibilities left."), *Grid[Index].Coordinate.ToString());
            continue;
        }

        // Track the lowest entropy found so far
        if (Entropy < LowestEntropy) {
            LowestEntropy = Entropy;
            LowestEntropyCandidates.Empty();
            LowestEntropyCandidates.Add(Index);
        } else if (Entropy == LowestEntropy) {
            LowestEntropyCandidates.Add(Index);
        }
    }
	int32 CandidateNum = LowestEntropyCandidates.Num();
    if (CandidateNum == 0)  return -1; // Everything is collapsed

    // If multiple cells have the same low entropy, pick one randomly
    int32 RandomIndex = FMath::RandRange(0, CandidateNum - 1);
    return LowestEntropyCandidates[RandomIndex];
}

void AWfcGenerator::CollapseCell(int32 CellIndex) {
    FWfcCell& Cell = Grid[CellIndex];

    // Calculate total weight of remaining possibilities
    float TotalWeight = 0.0f;
    for (UTileDataAsset* Tile : Cell.PossibleTiles) {
        if (Tile) TotalWeight += Tile->SpawnWeight;
    }

    // Pick a random float up to the total weight
    float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
    UTileDataAsset* ChosenTile = nullptr;

    // Subtract weights until we hit 0 to find our chosen tile
    for (UTileDataAsset* Tile : Cell.PossibleTiles) {
        if (!Tile) continue;
        RandomWeight -= Tile->SpawnWeight;
        if (RandomWeight <= 0.0f) {
            ChosenTile = Tile;
            break;
        }
        
    }

    // Failsafe in case floating point math misses by a fraction
    if (!ChosenTile && Cell.PossibleTiles.Num() > 0) {
        ChosenTile = Cell.PossibleTiles.Last();
    }

    // Collapse the cell down to ONLY the chosen tile
    Cell.PossibleTiles.Empty();
    if (ChosenTile) {
        Cell.PossibleTiles.Add(ChosenTile);
    }

    Cell.bIsCollapsed = true;
}

void AWfcGenerator::PropagateConstraints(int32 CollapsedCellIndex) {
    // A stack to keep track of cells that recently changed and need their neighbors checked
    TArray<int32> Stack;
    Stack.Add(CollapsedCellIndex);
    TArrayView<const FIntPoint> DirectionsToCheck = FWfcMath::GetDirectionOffsets(EWfcDirection::Any);

    while (Stack.Num() > 0) {
        int32 CurrentIndex = Stack.Pop();
        FWfcCell& CurrentCell = Grid[CurrentIndex];

		// Check Directions based on the collapsed tile's constraints
        for (FIntPoint Dir : DirectionsToCheck) {
            FWfcCell* NeighborCell = GetNeighbor(CurrentCell, Dir);

            // Early exit if off the board, or if already collapsed
            if (!NeighborCell || NeighborCell->bIsCollapsed) continue;

            bool bPossibilitiesChanged = false;

            // Loop backwards because we might remove items from the array
            for (int32 Index = NeighborCell->PossibleTiles.Num() - 1; Index >= 0; Index--) {
                UTileDataAsset* TileToEvaluate = NeighborCell->PossibleTiles[Index];

                // Dereference (*) the pointer to pass it as a reference
                if (!IsTileStillValid(*NeighborCell, TileToEvaluate)) {
                    NeighborCell->PossibleTiles.RemoveAt(Index);
                    bPossibilitiesChanged = true;
                }
            }

            // If this neighbor lost possibilities, it might affect ITS neighbors, so add to stack
            if (bPossibilitiesChanged) {
                // Add the Index to the stack
                int32 NeighborIndex = GetNeighborIndex(CurrentCell, Dir);
                Stack.AddUnique(NeighborIndex);
            }
        }
    }
}

void AWfcGenerator::SpawnActorsFromGrid() {
    FVector StartLocation = GetActorLocation();

    for (const FWfcCell& Cell : Grid) {
        if (Cell.bIsCollapsed && Cell.PossibleTiles.Num() == 1) {
            UTileDataAsset* FinalTile = Cell.PossibleTiles[0];

            if (FinalTile && FinalTile->ActorClass) {
                // Calculate physical world position based on grid coordinate and spacing
                FVector SpawnLocation = StartLocation + FVector(Cell.Coordinate.X * TileSpacing, Cell.Coordinate.Y * TileSpacing, 0.0f);

                GetWorld()->SpawnActor<AActor>(FinalTile->ActorClass, SpawnLocation, FRotator::ZeroRotator);
            }
        }
    }
}

int32 AWfcGenerator::GetIndexFromGridPos(int32 X, int32 Y) const {
    return X + (Y * GridSize.X);
}

bool AWfcGenerator::IsValidCoordinate(int32 X, int32 Y) const {
    return (X >= 0 && X < GridSize.X && Y >= 0 && Y < GridSize.Y);
}

bool AWfcGenerator::IsTileStillValid(const FWfcCell& CurrentCell, UTileDataAsset* TileToEvaluate) const {
    if (!TileToEvaluate) return false;

    // Does TileToEvaluate allow this placement?
    for (const FTileRuleWrapper& Rule : TileToEvaluate->AdjacencyRules) {
        if (!Rule.TargetTile) continue;

        // Get only the directions this rule cares about
        TArrayView<const FIntPoint> Offsets = FWfcMath::GetDirectionOffsets(Rule.Direction);
        int32 PossibleCount = 0;

        for (FIntPoint Offset : Offsets) {
            const FWfcCell* Neighbor = GetNeighbor(CurrentCell, Offset);
            if (!Neighbor) continue;

            if (Rule.Condition == EWfcRuleCondition::Avoidant) {
                // FAST FAIL: If the neighbor is already collapsed to the tile we hate, reject!
                if (Neighbor->bIsCollapsed && Neighbor->PossibleTiles.Num() == 1 && Neighbor->PossibleTiles[0] == Rule.TargetTile) {
                    return false;
                }
            } else if (Rule.Condition == EWfcRuleCondition::Required) {
                // TALLY: Does this neighbor at least have the POTENTIAL to be our required tile?
                if (Neighbor->PossibleTiles.Contains(Rule.TargetTile)) {
                    PossibleCount++;
                }
            }
        }

        // SLOW FAIL: Is it mathematically impossible to satisfy the RequiredCount?
        if (Rule.Condition == EWfcRuleCondition::Required && PossibleCount < Rule.RequiredCount) {
            return false;
        }
    }

    // BIDIRECTIONAL CHECK: Do the neighbors hate TileToEvaluate?)
    TArrayView<const FIntPoint> AllOffsets = FWfcMath::GetDirectionOffsets(EWfcDirection::Any);

    for (FIntPoint Offset : AllOffsets) {
        const FWfcCell* Neighbor = GetNeighbor(CurrentCell, Offset);

        // Skip if off the board, not collapsed, or has no specific tile locked in
        if (!Neighbor || !Neighbor->bIsCollapsed || Neighbor->PossibleTiles.Num() != 1) continue;

        UTileDataAsset* NeighborTile = Neighbor->PossibleTiles[0];
        if (!NeighborTile) continue;

        for (const FTileRuleWrapper& NeighborRule : NeighborTile->AdjacencyRules) {
            if (NeighborRule.Condition == EWfcRuleCondition::Avoidant && NeighborRule.TargetTile == TileToEvaluate) {
                // Ask the math library if this neighbor's rule applies to the direction we are standing in
                if (FWfcMath::MatchesDirection(Offset, NeighborRule.Direction)) {
                    return false; // The neighbor hates us
                }
            }
        }
    }
    return true; // Survived all constraints
}

int32 AWfcGenerator::GetNeighborIndex(const FWfcCell& CurrentCell, FIntPoint Offset) const {
    int32 X = CurrentCell.Coordinate.X + Offset.X;
    int32 Y = CurrentCell.Coordinate.Y + Offset.Y;

    if (IsValidCoordinate(X, Y)) {
        return GetIndexFromGridPos(X, Y);
    } return -1;
}

// Write-only version
FWfcCell* AWfcGenerator::GetNeighbor(const FWfcCell& CurrentCell, FIntPoint Offset) {
    int32 Index = GetNeighborIndex(CurrentCell, Offset);
    return (Index != -1) ? &Grid[Index] : nullptr;
}

// Read-only version of the above function
const FWfcCell* AWfcGenerator::GetNeighbor(const FWfcCell& CurrentCell, FIntPoint Offset) const {
    int32 Index = GetNeighborIndex(CurrentCell, Offset);
    return (Index != -1) ? &Grid[Index] : nullptr;
}