// Fill out your copyright notice in the Description page of Project Settings.

#include "LuaBlueprintFunctionLibrary.h"
#include "LuaMachine.h"



FLuaValue ULuaBlueprintFunctionLibrary::LuaCreateString(FString String)
{
	FLuaValue LuaString;
	LuaString.Type = ELuaValueType::String;
	LuaString.String = String;
	return LuaString;
}

FString ULuaBlueprintFunctionLibrary::Conv_LuaValueToString(FLuaValue Value)
{
	return Value.ToString();
}

UObject* ULuaBlueprintFunctionLibrary::Conv_LuaValueToObject(FLuaValue Value)
{
	if (Value.Type == ELuaValueType::Object)
	{
		return Value.Object;
	}
	return nullptr;
}

UClass* ULuaBlueprintFunctionLibrary::Conv_LuaValueToClass(FLuaValue Value)
{
	if (Value.Type == ELuaValueType::Object)
	{
		UClass* Class = Cast<UClass>(Value.Object);
		if (Class)
			return Class;
		UBlueprint* Blueprint = Cast<UBlueprint>(Value.Object);
		if (Blueprint)
			return Blueprint->GeneratedClass;
	}
	return nullptr;
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_ObjectToLuaValue(UObject* Object)
{
	FLuaValue LuaValue;
	LuaValue.Type = ELuaValueType::Object;
	LuaValue.Object = Object;
	return LuaValue;
}

int32 ULuaBlueprintFunctionLibrary::Conv_LuaValueToInt(FLuaValue Value)
{
	switch (Value.Type)
	{
	case ELuaValueType::Bool:
		return Value.Bool ? 1 : 0;
	case ELuaValueType::Integer:
		return Value.Integer;
	case ELuaValueType::Number:
		return Value.Number;
	case ELuaValueType::String:
		return FCString::Atoi(*Value.String);
	}
	return 0;
}

float ULuaBlueprintFunctionLibrary::Conv_LuaValueToFloat(FLuaValue Value)
{
	switch (Value.Type)
	{
	case ELuaValueType::Bool:
		return Value.Bool ? 1.0f : 0.0f;
	case ELuaValueType::Integer:
		return Value.Integer;
	case ELuaValueType::Number:
		return Value.Number;
	case ELuaValueType::String:
		return FCString::Atof(*Value.String);
	}
	return 0.0f;
}

FLuaValue ULuaBlueprintFunctionLibrary::Conv_IntToLuaValue(int32 Value)
{
	return FLuaValue(Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaGetGlobalTableValue(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Key)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return FLuaValue();
	uint32 ItemsToPop = L->GetFieldFromTree(Key);
	FLuaValue ReturnValue = L->ToLuaValue(-1);
	L->Pop(ItemsToPop);
	return ReturnValue;
}

int32 ULuaBlueprintFunctionLibrary::LuaGetTop(UObject* WorldContextObject, TSubclassOf<ULuaState> State)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return MIN_int32;
	return L->GetTop();
}

void ULuaBlueprintFunctionLibrary::LuaSetGlobalTableValue(UObject* WorldContextObject, TSubclassOf<ULuaState> State, FString Key, FLuaValue Value)
{
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(State, WorldContextObject->GetWorld());
	if (!L)
		return;
	L->SetFieldFromTree(Key, Value);
}

FLuaValue ULuaBlueprintFunctionLibrary::LuaCallGlobalFunction(UObject* WorldContextObject, TSubclassOf<ULuaState> LuaState, FString FunctionName, TArray<FLuaValue> Args)
{
	FLuaValue ReturnValue;
	ULuaState* L = FLuaMachineModule::Get().GetLuaState(LuaState, WorldContextObject->GetWorld());
	if (!L)
		return ReturnValue;

	int32 ItemsToPop = L->GetFieldFromTree(FunctionName);

	int NArgs = 0;
	for (FLuaValue& Arg : Args)
	{
		L->FromLuaValue(Arg);
		NArgs++;
	}

	L->PCall(NArgs, ReturnValue);

	// we have the return value and the function has been removed, so we do not need to change ItemsToPop
	L->Pop(ItemsToPop);

	return ReturnValue;
}