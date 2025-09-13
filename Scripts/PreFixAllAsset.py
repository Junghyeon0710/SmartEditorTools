import unreal

prefixes = {
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
    "TextureCube": "TC"
}

settings = unreal.get_default_object(unreal.AutoDeveloperSettings)
working_paths = [p.path for p in settings.pre_fix_asset_paths if p.path]

if not working_paths:
    unreal.EditorDialog.show_message(
        title="Path Error",
        message="No paths are set in AutoDeveloperSettings.",
        message_type=unreal.AppMsgType.OK
    )
    raise RuntimeError("Cannot get paths from AutoDeveloperSettings")

def get_prefix(class_name):
    return prefixes.get(class_name, "")

def format_new_name(prefix, asset_name):
    return f"{prefix}_{asset_name}.{prefix}_{asset_name}"

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
    new_name = format_new_name(prefix, asset_name)
    new_path = path_only + new_name
    preview_list.append((asset_path, new_path))

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
