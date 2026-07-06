// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WFCTypes.h"
#include "TileDataAsset.generated.h"

/** UTileDataAsset
 * The core data container for the Wave Function Collapse (WFC) generation system.
 * This asset represents a single tile possibility (e.g., an enemy, a wall, or an empty space).
 * It dictates the tile's UI visualization, the physical 3D Actor to spawn upon final generation, 
 * its base selection probability (weight), and its complex adjacency constraints.
 * Rules are defined via an array of FTileRuleWrappers, which allows designers to build highly 
 * customizable, data-driven constraints—including Avoidant/Required conditions, specific 
 * directional checks (Orthogonal/Diagonal), and exact neighbor counts—without altering C++ logic.
 */
UCLASS(BlueprintType)
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Rules")
    float SpawnWeight = 1.0f;

    // The master list of all rules for this tile
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC|Rules")
    TArray<FTileRuleWrapper> AdjacencyRules;
};
