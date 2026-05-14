// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGTankEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"

ATGTankEnemy::ATGTankEnemy()
{
	HP = 15;
	StructureAttackDamage = 2;
	StructureAttackInterval = 0.9f;
	StructureAttackRange = 200;
	EnergyDropAmount = 50;

	GetCharacterMovement()->MaxWalkSpeed = 200;
}
