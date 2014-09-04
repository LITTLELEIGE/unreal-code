// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameplayTagsModulePrivatePCH.h"
#include "GameplayTagsModule.h"

UGameplayTagsSettings::UGameplayTagsSettings(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{


}

#if WITH_EDITOR
void UGameplayTagsSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UGameplayTagsSettings, GameplayTags))
		{
			IGameplayTagsModule::Get().GetGameplayTagsManager().DestroyGameplayTagTree();
			IGameplayTagsModule::Get().GetGameplayTagsManager().ConstructGameplayTagTree();
		}
	}
}
#endif
