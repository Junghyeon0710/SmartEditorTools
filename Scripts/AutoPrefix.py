import unreal
import sys
import os

plugin_name = "SmartEditorTools"
plugin_dir = os.path.join(unreal.Paths.project_plugins_dir(), plugin_name, "Scripts")
if plugin_dir not in sys.path:
    sys.path.append(plugin_dir)

from PrefixRules import get_prefix

print("Autopreifx")
# SmartDeveloperSettings에서 단일 경로 가져오기
settings = unreal.get_default_object(unreal.SmartDeveloperSettings)
asset_path = settings.auto_asset_path  # FName → str
print(asset_path)

# 에셋 데이터 가져오기
asset_data = unreal.EditorAssetLibrary.find_asset_data(asset_path)
if not asset_data:
    unreal.log_warning(f"[PrefixFix] Asset not found: {asset_path}")
else:
    print("Autopreifx2")
    asset = asset_data.get_asset()
    asset_name = str(asset.get_name())
    class_name = str(asset.get_class().get_name())
    prefix = get_prefix(class_name)

    if not prefix or asset_name.startswith(prefix + "_"):
        # 이미 올바른 접두사가 붙었거나 prefix가 없으면 종료
        pass
    else:
        # 이름 변경
        path_only = asset_path.rsplit("/", 1)[0] + "/"
        new_name = f"{prefix}_{asset_name}"  # format_new_name(prefix, asset_name) 대신
        new_path = path_only + new_name

        success = unreal.EditorAssetLibrary.rename_asset(asset_path, new_path)
        if success:
            unreal.log(f"[PrefixFix] Renamed {asset_path} → {new_path}")
        else:
            unreal.log_warning(f"[PrefixFix] Failed to rename {asset_path}")
