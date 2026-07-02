// Fill out your copyright notice in the Description page of Project Settings.

#include "WFCGenerator.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

namespace{
    const FIntPoint WfcDirections[4] = {
        FIntPoint(0, 1),  // North
        FIntPoint(0, -1), // South
        FIntPoint(1, 0),  // East
        FIntPoint(-1, 0)  // West
    };
}

// Sets default values
AWfcGenerator::AWfcGenerator() {
    PrimaryActorTick.bCanEverTick = false; 
}

bool AWfcGenerator::StepWfc()
{
    int32 NextCellIndex = GetLowestEntropyCellIndex();

    // If -1, the grid is fully collapsed (or a contradiction occurred)
    if (NextCellIndex == -1)
    {
        return true; // We are done!
    }

    CollapseCell(NextCellIndex);
    PropagateConstraints(NextCellIndex);

    return false; // Not done yet, keep stepping
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

    while (FailsafeCounter < MaxIterations) {
        int32 NextCellIndex = GetLowestEntropyCellIndex();

        // If it returns -1, the grid is fully collapsed (or a contradiction occurred)
        if (NextCellIndex == -1) {
            break;
        }

        CollapseCell(NextCellIndex);
        PropagateConstraints(NextCellIndex);

        FailsafeCounter++;
    }

    if (FailsafeCounter >= MaxIterations) {
        UE_LOG(LogTemp, Warning, TEXT("WFC Loop hit the failsafe! Grid may not be fully collapsed."));
    }
}

int32 AWfcGenerator::GetLowestEntropyCellIndex() const {
    int32 LowestEntropy = 999999;
    TArray<int32> LowestEntropyCandidates;
    LowestEntropyCandidates.Reserve(Grid.Num());

    for (int32 Index = 0; Index < Grid.Num(); Index++) {
        if (!Grid[Index].bIsCollapsed) {
            int32 Entropy = Grid[Index].GetEntropy();

            // Contradiction: A cell has 0 possibilities left before it was collapsed
            if (Entropy == 0) {
                UE_LOG(LogTemp, Error, TEXT("WFC Contradiction at Cell %s! No possibilities left."), *Grid[Index].Coordinate.ToString());
                continue;
            }

            if (Entropy < LowestEntropy) {
                LowestEntropy = Entropy;
                LowestEntropyCandidates.Empty();
                LowestEntropyCandidates.Add(Index);
            }
            else if (Entropy == LowestEntropy) {
                LowestEntropyCandidates.Add(Index);
            }
        }
    }
	int32 CandidateNum = LowestEntropyCandidates.Num();
    if (CandidateNum == 0) {
        return -1; // Everything is collapsed
    }

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
        if (Tile) {
            RandomWeight -= Tile->SpawnWeight;
            if (RandomWeight <= 0.0f) {
                ChosenTile = Tile;
                break;
            }
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
    // A stack to keep track of cells that need their neighbors updated
    TArray<int32> Stack;
    Stack.Add(CollapsedCellIndex);

    while (Stack.Num() > 0) {
        int32 CurrentIndex = Stack.Pop();
        FWfcCell& CurrentCell = Grid[CurrentIndex];

        // Check all 4 neighbors
        for (FIntPoint Dir : WfcDirections) {
            int32 NeighborX = CurrentCell.Coordinate.X + Dir.X;
            int32 NeighborY = CurrentCell.Coordinate.Y + Dir.Y;

            if (IsValidCoordinate(NeighborX, NeighborY)) {
                int32 NeighborIndex = GetIndexFromGridPos(NeighborX, NeighborY);
                FWfcCell& NeighborCell = Grid[NeighborIndex];

                if (NeighborCell.bIsCollapsed) {
                    continue; // Skip already finalized cells
                }

                bool bPossibilitiesChanged = false;

                // Loop backwards because we might remove items from the array
                for (int32 Index = NeighborCell.PossibleTiles.Num() - 1; Index >= 0; Index--) {
                    UTileDataAsset* TileToEvaluate = NeighborCell.PossibleTiles[Index];

                    if (!IsTileStillValid(NeighborCell, TileToEvaluate)) {
                        NeighborCell.PossibleTiles.RemoveAt(Index);
                        bPossibilitiesChanged = true;
                    }
                }

                // If this neighbor lost possibilities, it might affect ITS neighbors, so add to stack
                if (bPossibilitiesChanged) {
                    Stack.AddUnique(NeighborIndex);
                }
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
                FRotator SpawnRotation = FRotator::ZeroRotator;

                GetWorld()->SpawnActor<AActor>(FinalTile->ActorClass, SpawnLocation, SpawnRotation);
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

    bool bNeedsDependency = TileToEvaluate->DependentTiles.Num() > 0;
    bool bDependencySatisfied = false;

    // Single pass over all valid neighbors
    for (FIntPoint Dir : WfcDirections) {
        int32 NeighborX = CurrentCell.Coordinate.X + Dir.X;
        int32 NeighborY = CurrentCell.Coordinate.Y + Dir.Y;

        if (IsValidCoordinate(NeighborX, NeighborY)) {
            const FWfcCell& Neighbor = Grid[GetIndexFromGridPos(NeighborX, NeighborY)];

            // AVOIDANT CHECK (Fast Fail)
            if (Neighbor.bIsCollapsed && Neighbor.PossibleTiles.Num() == 1) {
                UTileDataAsset* NeighborTile = Neighbor.PossibleTiles[0];
                if (NeighborTile) {
                    // Do I hate the neighbor? OR Does the neighbor hate me?
                    if (TileToEvaluate->AvoidantTiles.Contains(NeighborTile) ||
                        NeighborTile->AvoidantTiles.Contains(TileToEvaluate)) {
                        return false; // Found a one-way or mutual hatred! Reject immediately.
                    }
                }
            }

            // DEPENDENT CHECK (Track Success)
            // If we already satisfied the dependency, skip this inner loop to save CPU cycles
            if (bNeedsDependency && !bDependencySatisfied) {
                for (UTileDataAsset* DependentTile : TileToEvaluate->DependentTiles) {
                    if (Neighbor.PossibleTiles.Contains(DependentTile)) {
                        bDependencySatisfied = true;
                        break; // Satisfied! Stop checking this neighbor's tiles.
                    }
                }
            }
        }
    }

    // After checking all neighbors, if this tile needed a dependency but didn't find one, reject it.
    if (bNeedsDependency && !bDependencySatisfied) {
        return false;
    }

    return true;
}

