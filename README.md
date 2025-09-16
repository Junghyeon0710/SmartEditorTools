# SmartEditorTools

A plugin for Unreal Engine that provides a smarter editor environment using **Python** and **Slate widgets**.  

- Supported Version: **Unreal Engine 5.6**  
- [Update Notes](https://github.com/Junghyeon0710/UEAutomationTools/wiki/Update-Notes)

---

## Installation
1. Download the `.zip` file.
2. Extract it into the `Plugins` folder of your project as `SmartEditorTools`.
3. Launch your project.
4. In the editor, go to **Edit → Plugins** and enable **SmartEditorTools**.

---

## Features
- Automatically adds prefixes based on naming conventions for assets in a specified path.
- With **AutoPrefix** enabled, asset creation or renaming will be validated and prefixed automatically.
- Provides a custom combo box slatewidget for the editor level.

---

---

## Example Usage

### Enable the Plugin
<img width="1825" height="143" alt="image" src="https://github.com/user-attachments/assets/d284c166-8961-470e-a0e8-a6052ae0fc32" />  

Go to the **Editor Menu → Plugins**, then enable **SmartEditorTools**.

---

### How do I define prefix naming conventions?
Navigate to **Plugins → SmartEditorTools → Scripts** and open **PrefixRules.py**.  
<img width="386" height="725" alt="image" src="https://github.com/user-attachments/assets/c9d355e0-06ba-432b-abf3-ecaed845b368" />  

Inside **PREFIXRULES**, you can assign custom prefixes in the format:  

```python
"ClassType": "DesiredPrefix"
```

---

## 네이밍 컨변센 룰을 정했는데 어떻게 사용하나요

1. 지정 경로 모든 에셋 검사후 접두사 추가해주는 방법
   에디터 메뉴 -> Project Settings에서 Plugins카테고리에서 SmartEditor Tools를 찾아줍니다.
   <img width="1168" height="119" alt="image" src="https://github.com/user-attachments/assets/047a6564-89e0-4fec-a07e-181f86326d9c" />

PreFixAssetPath에 검사할 경로를 추가 해줍니다.
<img width="890" height="70" alt="image" src="https://github.com/user-attachments/assets/0c3bf924-d9a7-41a3-a917-2ecc2fbe6430" />
레벨 에디터 메뉴에 있는 PreFixAsset을 클릭해줍니다.


2. 자동으로 접두사 추가해주는 방법
   이름이 바뀌거나 에셋이 추가될 때 자동으로 검사후 알맞은 네이밍 컨벤션으로 바꿔줍니다.
    에디터 메뉴 -> Project Settings에서 Plugins카테고리에서 SmartEditor Tools를 찾아줍니다.
<img width="1104" height="209" alt="image" src="https://github.com/user-attachments/assets/1b03f607-9241-4266-88b4-1ec9e5a2956f" />
bAutoPrefix를 체크 해주면 적용이 된겁니다.




