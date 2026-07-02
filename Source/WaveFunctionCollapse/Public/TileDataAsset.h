// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileDataAsset.generated.h"

/** UTileDataAsset
 * The core data container for the Wave Function Collapse (WFC) generation system.
 * This asset represents a single tile possibility (e.g., an enemy, a wall, or an empty space).
 * It dictates what actor to spawn in the world, its base probability, and the strict
 * adjacency rules (Avoidant and Dependent) that the WFC algorithm uses to build the grid.
 */
UCLASS()
class WAVEFUNCTIONCOLLAPSE_API UTileDataAsset : public UDataAsset {
	GENERATED_BODY()
public:
    /* The actual Blueprint Actor to spawn in the world when this tile is permanently
     * selected (collapsed) by the WFC algorithm.*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Spawning")
    TSubclassOf<AActor> ActorClass;

    // The clean name displayed on the UI
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Visuals")
    FString DisplayName = "Unknown";

    // The color this tile will display in the UI board
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Visuals")
    FLinearColor TileColor = FLinearColor::Green;

    /* Determines how likely this tile is to be selected compared to other valid tiles.
     * A higher weight means it will be chosen more frequently during the collapse phase.*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Spawning")
    float SpawnWeight = 1.0f;

    /* AVOIDANT RULE (Blacklist):
     * This tile CANNOT be placed directly next to ANY of the tiles in this list.*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Rules")
    TArray<UTileDataAsset*> AvoidantTiles;

    /* DEPENDENT RULE (Whitelist/Requirement):
     * This tile MUST be placed next to AT LEAST ONE of the tiles in this list.*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Rules")
    TArray<UTileDataAsset*> DependentTiles;
};
