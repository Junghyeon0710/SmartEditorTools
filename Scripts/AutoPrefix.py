import unreal
import sys
import os

plugin_name = "SmartEditorTools"
plugin_dir = os.path.join(unreal.Paths.project_plugins_dir(), plugin_name, "Scripts")
if plugin_dir not in sys.path:
    sys.path.append(plugin_dir)

from PrefixRules import get_prefix

print("Autopreifx Start In Python")
settings = unreal.get_default_object(unreal.SmartDeveloperSettings)
asset_path = settings.auto_asset_path 
asset_data = unreal.EditorAssetLibrary.find_asset_data(asset_path)
if not asset_data:
    unreal.log_warning(f"[PrefixFix] Asset not found: {asset_path}")
else:
    print("Import data In Python")
    asset = asset_data.get_asset()
    asset_name = asset.get_name()
    class_name = asset.get_class().get_name()
    prefix = get_prefix(class_name)

    if not prefix or asset_name.startswith(prefix + "_"):
        print("pass")
        settings.auto_asset_path = ""
        pass
    else:
        path_only = asset_path.rsplit("/", 1)[0] + "/"
        new_name = f"{prefix}_{asset_name}" 
        new_path = path_only + new_name
        settings.auto_asset_path = "ing"
        success = unreal.EditorAssetLibrary.rename_asset(asset_path, new_path)
        if success:
            unreal.log(f"[PrefixFix] Renamed {asset_path} → {new_path}")
            settings.auto_asset_path = ""
        else:
            unreal.log_warning(f"[PrefixFix] Failed to rename {asset_path}")
            settings.auto_asset_path = ""
