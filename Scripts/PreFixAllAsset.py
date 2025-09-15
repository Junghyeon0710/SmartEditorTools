import sys
import os
import unreal

PLUGIN_NAME = "SmartEditorTools"

plugin_base_dir = os.path.join(unreal.Paths.project_plugins_dir(), PLUGIN_NAME)

plugin_script_dir = os.path.join(plugin_base_dir, "Scripts")

if plugin_script_dir not in sys.path:
    sys.path.append(plugin_script_dir)

from PrefixRules import PREFIX_RULES, get_prefix, needs_prefix
 

settings = unreal.get_default_object(unreal.SmartDeveloperSettings)
working_paths = [p.path for p in settings.pre_fix_asset_paths if p.path]

if not working_paths:
    unreal.EditorDialog.show_message(
        title="Path Error",
        message="No paths are set in AutoDeveloperSettings.",
        message_type=unreal.AppMsgType.OK
    )
    raise RuntimeError("Cannot get paths from AutoDeveloperSettings")


all_assets = []
for path in working_paths:
    assets = unreal.EditorAssetLibrary.list_assets(path, recursive=True, include_folder=False)
    all_assets += assets

preview_list = []
for asset_path in all_assets:
    asset_data = unreal.EditorAssetLibrary.find_asset_data(asset_path)
    asset = asset_data.get_asset()
    asset_name = asset.get_name()
    class_name = asset.get_class().get_name()
    prefix = get_prefix(class_name)

    if not prefix or asset_name.startswith(prefix + "_"):
        continue

    path_only = asset_path.rsplit("/", 1)[0] + "/"
    new_name = f"{prefix}_{asset_name}"
    new_path = path_only + new_name
    preview_list.append((asset_path, new_path))
    print(asset_path + "Junghyeon") 
if not preview_list:
    unreal.EditorDialog.show_message(
        title="Result",
        message="No assets found to apply prefixes.",
        message_type=unreal.AppMsgType.OK
    )
else:
    preview_text = "Assets to be renamed:\n\n"
    for old, new in preview_list[:15]:
        preview_text += f"{old} →\n  {new}\n\n"
    if len(preview_list) > 15:
        preview_text += f"...and {len(preview_list)-15} more"

    result = unreal.EditorDialog.show_message(
        title="Confirm Prefix Rename",
        message=preview_text + "\nDo you want to rename these assets?",
        message_type=unreal.AppMsgType.YES_NO,
        default_value=unreal.AppReturnType.NO
    )

    if result == unreal.AppReturnType.YES:
        with unreal.ScopedSlowTask(len(preview_list), "Renaming Assets...") as slow_task:
            slow_task.make_dialog(True)
            changed_count = 0
            for old_path, new_path in preview_list:
                if unreal.EditorAssetLibrary.rename_asset(old_path, new_path):
                    unreal.log(f"[PrefixFix] Renamed {old_path} → {new_path}")
                    changed_count += 1
                slow_task.enter_progress_frame(1)
                if slow_task.should_cancel():
                    break
        unreal.EditorDialog.show_message(
            title="Completed",
            message=f"Prefix applied successfully.\nTotal assets renamed: {changed_count}",
            message_type=unreal.AppMsgType.OK
        )
    else:
        unreal.EditorDialog.show_message(
            title="Cancelled",
            message="Operation was cancelled.",
            message_type=unreal.AppMsgType.OK
        )
