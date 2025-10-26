# Delete Unused Asset

This guide explains how to find and remove **unused assets** in the editor.

## Setup

1. **Open Content Drawer**

   Open the **Content Drawer** in the Unreal Editor and locate the asset you want to check.

   ![Right Click Asset](https://github.com/user-attachments/assets/ac05bcf9-3e4f-4435-b39d-2007587ddb7c)  
   *Right-click on an asset in the Content Drawer*

2. **Click Delete Unused Asset**

   From the context menu, select **Delete Unused Asset**.  
   If the asset is not referenced anywhere, it will be removed.

3. **Dependency Behavior**

   If an asset is not directly referenced but **depends on another asset**, it may still appear in the delete list.  

   For example:  
   - A **Game Mode** asset sets its **Default Pawn Class**.  
   - Even if the Game Mode is not directly referenced, it depends on the pawn asset.  
   - In this case, the Game Mode itself may be flagged as unused.

   This is because the tool treats assets that are **not referenced by others** but only **depend on other assets** as unused.

## Prevent Deleting Dependent Assets

1. **Open SmartEditorTools Plugin Settings**  

   Go to:  
   `Editor Menu → Project Settings → Plugins → SmartEditorTools`

   ![SmartEditorTools Plugin Settings](https://github.com/user-attachments/assets/7436f640-05e3-4046-ab1b-5b9d71c90867)  
   *SmartEditorTools plugin settings page*

2. **Enable "Depends on Other Assets"**

   In the plugin settings, enable **Depends on Other Assets**.  

   With this option on:  
   - Assets that depend on others will **not** be flagged as unused.  
   - For example, if the asset shows at least one dependency in the **Reference Viewer**, it will not be listed for deletion.

## Tips & Notes

- Make sure Unreal Editor is running in **Editor mode** before deleting assets.  
- Use **Reference Viewer** to confirm dependencies before removal.  
- Be cautious when deleting assets—once removed, they cannot be recovered unless backed up.  
