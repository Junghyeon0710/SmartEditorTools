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

## How to Use

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

## How do I use the naming convention rules?

### 1. Manually apply prefixes to all assets in a specified path
Go to **Editor Menu → Project Settings → Plugins → SmartEditorTools**.  
<img width="1168" height="119" alt="image" src="https://github.com/user-attachments/assets/047a6564-89e0-4fec-a07e-181f86326d9c" />  

Add the paths you want to check under **PreFixAssetPath**.  
<img width="890" height="70" alt="image" src="https://github.com/user-attachments/assets/0c3bf924-d9a7-41a3-a917-2ecc2fbe6430" />  

Then, click **PreFixAsset** from the **Level Editor Menu** to run the prefix check and automatically update asset names.

---

### 2. Automatically apply prefixes when assets are created or renamed
When assets are renamed or newly added, SmartEditorTools can automatically check and update names to match your naming convention.  

Go to **Editor Menu → Project Settings → Plugins → SmartEditorTools**.  
<img width="1104" height="209" alt="image" src="https://github.com/user-attachments/assets/1b03f607-9241-4266-88b4-1ec9e5a2956f" />  

Enable **bAutoPrefix** to activate this feature.

> The examples above are just a few use cases.  
> For more features, please check the [docs](./docs).

<br>

## What if I run into an issue?
If you encounter any problems, please open a new [issue](./issues).  
We’ll review it and provide a fix.




