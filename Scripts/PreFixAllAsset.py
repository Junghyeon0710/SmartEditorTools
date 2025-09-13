import unreal

# ------------------ 사용자 접두사 설정 ------------------
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

working_path = ["/Game/",  # 에셋 대상 경로
  ] 

# ------------------ 유틸 함수 ------------------
def get_prefix(class_name):
    return prefixes.get(class_name, "")

def format_new_name(prefix, asset_name):
    return f"{prefix}_{asset_name}.{prefix}_{asset_name}"

# ------------------ 에디터 라이브러리 ------------------
all_assets = unreal.EditorAssetLibrary.list_assets(working_path, recursive=True, include_folder=False)

# ------------------ 변경 대상 확인 ------------------
preview_list = []
for asset_path in all_assets:
    asset_data = unreal.EditorAssetLibrary.find_asset_data(asset_path)
    asset = asset_data.get_asset()
    asset_name = asset.get_name()
    class_name = asset_data.get_asset().get_class().get_name()
    prefix = get_prefix(class_name)

    if not prefix or asset_name.startswith(prefix + "_"):
        continue

    path_only = asset_path.rsplit("/", 1)[0] + "/"
    new_name = format_new_name(prefix, asset_name)
    new_path = path_only + new_name
    preview_list.append((asset_path, new_path))

# ------------------ 팝업 출력 ------------------
if not preview_list:
    unreal.EditorDialog.show_message(
        title="접두사 적용 결과",
        message="접두사를 추가할 에셋이 없습니다.",
        message_type=unreal.AppMsgType.OK
    )
else:
    # 다이얼로그에 보여줄 미리보기 문자열 만들기
    preview_text = "변경될 에셋 목록:\n\n"
    for old, new in preview_list[:15]:  # 너무 많으면 잘리니 15개만 미리보기
        preview_text += f"{old} →\n  {new}\n\n"

    if len(preview_list) > 15:
        preview_text += f"...외 {len(preview_list)-15}개"

    result = unreal.EditorDialog.show_message(
        title="접두사 적용 확인",
        message=preview_text + "\n\n이 에셋들의 이름을 변경하시겠습니까?",
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
                title="완료",
                message=f"접두사 적용 완료\n총 {changed_count}개의 에셋이 이름 변경됨.",
                message_type=unreal.AppMsgType.OK
            )
    else:
        unreal.EditorDialog.show_message(
            title="취소됨",
            message="작업이 취소되었습니다.",
            message_type=unreal.AppMsgType.OK
        )
