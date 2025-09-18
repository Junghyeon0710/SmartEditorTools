# Delete Empty Folders

This guide explains how to find and remove **empty folders** in the editor.  

## Setup

1. **Open Content Drawer**

   Open the **Content Drawer** in the Unreal Editor and locate the folder you want to check.  

2. **Right-Click the Folder**

   Right-click on the folder you want to clean up.  

   <img width="280" height="551" alt="Image" src="https://github.com/user-attachments/assets/bb75bc9a-8d39-4141-8dff-0cf9e46efae4" />
   
   *Right-click on a folder in the Content Drawer*

4. **Click Delete Empty Folders**

   From the context menu, select **Delete Empty Folders**.  

5. **Folder Scan**

   The tool will scan the selected folder path.  
   - If the folder contains **no assets**, it will be removed automatically.  
   - If the folder has any assets, it will remain untouched.  

## Tips & Notes

- This operation only removes **completely empty folders**.  
- Subfolders are also checked during the scan.  
- To keep your project clean, run this regularly after large refactors or asset deletions.  
- Deleted folders are gone permanently, so confirm before proceeding.  
