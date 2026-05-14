// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGDisruptorEnemy.h"

ATGDisruptorEnemy::ATGDisruptorEnemy()
{
	EnemyType = TEXT("교란병");
	MaxHP = 8;
	StructureAttackDamage = 1;
	StructureAttackInterval = 0.6f;
	StructureAttackRange = 200;
	EnergyDropAmount = 40;

	PlayerDetectRange = 400.f;
	PlayerDetectInterval = 0.2f;

	AreaAttackRange = 550.f;
	AreaAttackDelay = 0.8f;
	AreaAttackDamage = 1.f;

	KnockbackForce = 800.f;
	DebuffDuration = 2.f;
}
