#pragma once

#include "FastNoiseLite.h"
#include "RandomSeedWorldType.generated.h"


USTRUCT(BlueprintType)
struct FWorldCreateConfig
{
	GENERATED_BODY()

	// the z is the world's floor count
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FIntVector WorldSize = FIntVector(128,128,5);

	// the noise value check with this to create mesh ; -1 -> 1
	// 和 Noise 值比较决定是否创建
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CreateMeshThreshold = 0.f;

	// 采样 Noise 时的Z值缩放,避免相近图层相似度太高;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 SampleZScale = 1;

	// 随机Seed ?
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bRandomSeed = true;	
	


	
	int32 GetTotalCount() const
	{
		return WorldSize.X * WorldSize.Y * WorldSize.Z;
	}
	int32 GetIndexByXYZ(int32 X,int32 Y,int32 Z) const
	{
		return Z * WorldSize.X * WorldSize.Y + Y * WorldSize.X + X;
	}
};

UENUM()
enum class EFastNoiseBPType : uint8
{
	// 与 TYPE_PERLIN 不同，渐变存在于单纯形点阵中，而不是网格点阵中，从而避免了定向伪影。
	NoiseType_OpenSimplex2,
	// 对 TYPE_SIMPLEX 修改后得到的更高质量版本，但速度较慢。
	NoiseType_OpenSimplex2S,
	// 蜂窝包括 Worley 噪声图和 Voronoi 图，它们创建了相同值的不同区域。
	NoiseType_Cellular,
	// 随机渐变的一种格子。对它们的点积进行插值，以获得格子之间的值。
	NoiseType_Perlin,
	// 类似于 Value 噪声，但速度较慢。波峰和波谷的变化更大。
	// 在使用值噪声创建凹凸贴图时，可以使用三次噪声来避免某些伪影。一般来说，如果值噪声用于高度图或凹凸贴图，则应始终使用此模式。
	NoiseType_ValueCubic,
	// 点阵被分配随机值，然后根据相邻值进行插值。
	NoiseType_Value
};


UENUM()
enum class EFractalBPType : uint8
{
	// 无分形噪声。
	FractalType_None,
	// 使用分形布朗运动将八度音阶组合成分形的方法。
	FractalType_FBm,
	// 将八度音阶组合成分形的方法，从而产生一种“脊状”外观。
	FractalType_Ridged,
	// 将八度音阶组合成具有乒乓效果的分形的方法。
	FractalType_PingPong,
	// 逐渐扭曲空间，一个八度一个八度，导致更“液化”的失真。
	FractalType_DomainWarpProgressive,
	// 为每个八度音阶独立地扭曲空间，从而导致更混乱的失真。
	FractalType_DomainWarpIndependent
};


UENUM()
enum class ERotationType3DBP : uint8
{
	RotationType3D_None,
	RotationType3D_ImproveXYPlanes,
	RotationType3D_ImproveXZPlanes
};


UENUM()
enum class ECellularDistanceFunctionBP : uint8
{
	// 到最近点的欧几里得距离。
	CellularDistanceFunction_Euclidean,
	// 到最近点的欧几里得距离的平方。
	CellularDistanceFunction_EuclideanSq,
	// 到最近点的曼哈顿距离（出租车度量法）。
	CellularDistanceFunction_Manhattan,
	// DISTANCE_EUCLIDEAN 和 DISTANCE_MANHATTAN 的混合，以给出弯曲的单元格边界
	CellularDistanceFunction_Hybrid
};


UENUM()
enum class ECellularReturnTypeBP : uint8
{
	// 蜂窝单元格距离函数，将为单元格内的所有点返回相同的值。
	CellularReturnType_CellValue,
	// 蜂窝单元格距离函数，将返回一个由到最近点的距离确定的值。
	CellularReturnType_Distance,
	// 蜂窝单元格距离函数，将返回到第二最近点的距离。
	CellularReturnType_Distance2,
	// 将最近点的距离与次近点的距离相加。
	CellularReturnType_Distance2Add,
	// 将最近点的距离与次近点的距离相减。
	CellularReturnType_Distance2Sub,
	// 将最近点的距离与次近点的距离相乘。
	CellularReturnType_Distance2Mul,
	// 将最近点的距离与次近点的距离相除。
	CellularReturnType_Distance2Div
};

UENUM()
enum class EDomainWarpBPType : uint8
{
	// 使用单纯形噪声算法，对域进行扭曲。
	DomainWarpType_OpenSimplex2,
	// 使用简化版的单纯形噪声算法，对域进行扭曲。
	DomainWarpType_OpenSimplex2Reduced,
	// 使用简单的噪声栅格（不像其他方法那样平滑，但性能更高），对域进行扭曲。
	DomainWarpType_BasicGrid
};

USTRUCT(Blueprintable)
struct FFastNoiseLiteConfig
{
	GENERATED_BODY()

	// 所使用的噪声算法
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	EFastNoiseBPType NoiseType = EFastNoiseBPType::NoiseType_OpenSimplex2;
	// 所有噪声类型的随机数种子。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	int32 Seed = 1337;
	// 设置“三维噪波”和“三维域扭曲”的域旋转类型。在对3D中的2D平面进行采样时，可以帮助减少方向伪影
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	ERotationType3DBP RotationType3D = ERotationType3DBP::RotationType3D_None;
	// 所有噪声类型的频率。低频产生平滑的噪声，而高频产生更粗糙、颗粒状更明显的噪声
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|General")
	float Frequency = 0.01f;


	// 将八度音阶组合成分形的方法。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	EFractalBPType FractalType = EFractalBPType::FractalType_None;
	// 为获得分形噪声类型的最终值而采样的噪声层数。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	int32 FractalOctaves = 3;
	// 后续八度音阶之间的倍频器。增加该值，会产生更高的八度音阶，从而产生细节更精细、外观更粗糙的噪声
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalLacunarity = 2.f;
	// 确定分形噪声中噪声的每个后续层的强度。
	// 较低的值更强调较低频率的基础层，而较高的值则更强调较高频率的层。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalGain = 0.5f;
	// 较高的权重意味着如果较低的八度具有较大的影响，则较高的八度具有较小的影响
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalWeightedStrength = 0.f;
	// 设置分形乒乓类型的强度。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Fractal")
	float FractalPingPongStrength = 2.f;


	// 确定如何计算到最近/第二最近点的距离
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	ECellularDistanceFunctionBP CellularDistanceFunction = ECellularDistanceFunctionBP::CellularDistanceFunction_Euclidean;
	// 蜂窝噪声计算的返回类型
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	ECellularReturnTypeBP CellularReturnType = ECellularReturnTypeBP::CellularReturnType_Distance;
	// 一个点可以离开其栅格位置的最大距离。对于偶数栅格，设置为 0。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|Cellular")
	float CellularJitter = 1.f;


	// 设置扭曲算法
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|DomainWarp")
	EDomainWarpBPType DomainWarpType;
	// 设置距原点的最大扭曲距离。
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NoiseCreateParam|DomainWarp")
	float DomainWarpAmp =  1.f;


	
	FastNoiseLite::NoiseType GetNoiseType() const
	{
		FastNoiseLite::NoiseType Type = FastNoiseLite::NoiseType_Perlin;
		switch (NoiseType)
		{
		case EFastNoiseBPType::NoiseType_OpenSimplex2:
			Type = FastNoiseLite::NoiseType_OpenSimplex2;
			break;
		case EFastNoiseBPType::NoiseType_OpenSimplex2S:
			Type = FastNoiseLite::NoiseType_OpenSimplex2S;
			break;
		case EFastNoiseBPType::NoiseType_Cellular:
			Type = FastNoiseLite::NoiseType_Cellular;
			break;
		case EFastNoiseBPType::NoiseType_Perlin:
			Type = FastNoiseLite::NoiseType_Perlin;
			break;
		case EFastNoiseBPType::NoiseType_ValueCubic:
			Type = FastNoiseLite::NoiseType_ValueCubic;
			break;
		case EFastNoiseBPType::NoiseType_Value:
			Type = FastNoiseLite::NoiseType_Value;
			break;
		}
		return Type;
	}
	FastNoiseLite::RotationType3D GetRotationType3D() const
	{
		FastNoiseLite::RotationType3D Type = FastNoiseLite::RotationType3D_None;
		switch (RotationType3D)
		{
		case ERotationType3DBP::RotationType3D_None:
			Type = FastNoiseLite::RotationType3D_None;
			break;
		case ERotationType3DBP::RotationType3D_ImproveXYPlanes:
			Type = FastNoiseLite::RotationType3D_ImproveXYPlanes;
			break;
		case ERotationType3DBP::RotationType3D_ImproveXZPlanes:
			Type = FastNoiseLite::RotationType3D_ImproveXZPlanes;
			break;
		}

		return Type;
	}
	FastNoiseLite::FractalType GetFractalType() const
	{
		FastNoiseLite::FractalType Type = FastNoiseLite::FractalType_None;
		switch (FractalType)
		{
		case EFractalBPType::FractalType_None:
			Type = FastNoiseLite::FractalType_None;
			break;
		case EFractalBPType::FractalType_FBm:
			Type = FastNoiseLite::FractalType_FBm;
			break;
		case EFractalBPType::FractalType_Ridged:
			Type = FastNoiseLite::FractalType_Ridged;
			break;
		case EFractalBPType::FractalType_PingPong:
			Type = FastNoiseLite::FractalType_PingPong;
			break;
		case EFractalBPType::FractalType_DomainWarpProgressive:
			Type = FastNoiseLite::FractalType_DomainWarpProgressive;
			break;
		case EFractalBPType::FractalType_DomainWarpIndependent:
			Type = FastNoiseLite::FractalType_DomainWarpIndependent;
			break;
		}

		return Type;
	}
	FastNoiseLite::CellularDistanceFunction GetCellularDistanceFunction() const
	{
		FastNoiseLite::CellularDistanceFunction Type = FastNoiseLite::CellularDistanceFunction_Euclidean;
		switch (CellularDistanceFunction)
		{
		case ECellularDistanceFunctionBP::CellularDistanceFunction_Euclidean:
			Type = FastNoiseLite::CellularDistanceFunction_Euclidean;
			break;
		case ECellularDistanceFunctionBP::CellularDistanceFunction_EuclideanSq:
			Type = FastNoiseLite::CellularDistanceFunction_EuclideanSq;
			break;
		case ECellularDistanceFunctionBP::CellularDistanceFunction_Manhattan:
			Type = FastNoiseLite::CellularDistanceFunction_Manhattan;
			break;
		case ECellularDistanceFunctionBP::CellularDistanceFunction_Hybrid:
			Type = FastNoiseLite::CellularDistanceFunction_Hybrid;
			break;
		}
		return Type;
	}
	FastNoiseLite::CellularReturnType GetCellularReturnType() const
	{
		FastNoiseLite::CellularReturnType Type = FastNoiseLite::CellularReturnType_Distance;
		switch (CellularReturnType)
		{
		case ECellularReturnTypeBP::CellularReturnType_CellValue:
			Type = FastNoiseLite::CellularReturnType_CellValue;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance:
			Type = FastNoiseLite::CellularReturnType_Distance;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance2:
			Type = FastNoiseLite::CellularReturnType_Distance2;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance2Add:
			Type = FastNoiseLite::CellularReturnType_Distance2Add;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance2Sub:
			Type = FastNoiseLite::CellularReturnType_Distance2Sub;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance2Mul:
			Type = FastNoiseLite::CellularReturnType_Distance2Mul;
			break;
		case ECellularReturnTypeBP::CellularReturnType_Distance2Div:
			Type = FastNoiseLite::CellularReturnType_Distance2Div;
			break;
		}
		return Type;
	}
	FastNoiseLite::DomainWarpType GetDomainType() const
	{
		FastNoiseLite::DomainWarpType Type = FastNoiseLite::DomainWarpType_BasicGrid;
		switch (DomainWarpType)
		{
		case EDomainWarpBPType::DomainWarpType_OpenSimplex2:
			Type = FastNoiseLite::DomainWarpType_OpenSimplex2;
			break;
		case EDomainWarpBPType::DomainWarpType_OpenSimplex2Reduced:
			Type = FastNoiseLite::DomainWarpType_OpenSimplex2Reduced;
			break;
		case EDomainWarpBPType::DomainWarpType_BasicGrid:
			Type = FastNoiseLite::DomainWarpType_BasicGrid;
			break;
		}
		return Type;
	}
};
