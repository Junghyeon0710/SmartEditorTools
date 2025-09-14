# PrefixRules.py
import unreal

PREFIX_RULES = {
    "AnimBlueprint": "ABP",
    "AnimSequence": "A",
    "Animation": "A",
    "BlendSpace1D": "BS",
    "Blueprint": "BP",
    "WidgetBlueprint": "WBP",
    "CurveFloat": "CF",
    "CurveLinearColor": "CLC",
    "Material": "M",
    "MaterialFunction": "MF",
    "MaterialInstance": "MI",
    "ParticleSystem": "PS",
    "PhysicsAsset": "PHYS",
    "SkeletalMesh": "SK",
    "Skeleton": "SKL",
    "SoundCue": "SC",
    "SoundWave": "SW",
    "StaticMesh": "SM",
    "Texture2D": "T",
    "TextureCube": "TC",
    "NiagaraSystem": "NS",
    "World": "L"
}

def get_prefix(class_name: str) -> str:
    return PREFIX_RULES.get(class_name, "")

def needs_prefix(asset) -> bool:
    class_name = asset.get_class().get_name()
    prefix = get_prefix(class_name)
    if not prefix:
        return False
    return not asset.get_name().startswith(prefix + "_")
