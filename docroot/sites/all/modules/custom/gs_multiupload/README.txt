This module provides a menu callback for uploading multiple image files simultaneously to a node.


Instructions:
-------------
- Enable the module as usual, admin/modules "GS Multiupload".
  


Notes:
------
- A new menu item can be called "#overlay=gs/multiupload/NID" where NID is the node id that has field_collection_items configured.
  For this to work properly, your content type needs to have a field_collection with an image field in it.
  The image field can be a media field as well.
  
- Go to "#overlay=gs/multiupload/NID", where NID is the node id. Drop the image files in the black rectangle to upload them 
  and then hit "Save and Close" button to save the images to the node and close the overlay.

- The files are uploaded via HTML5 and javascript.

- It only supports HTML5 friendly browsers.

- Files are stored initally in Drupal's temp directoy and then when user hits "Save and Close" button, files are moved to public folder.

- Supports 10 simultaneous uploads by default. This can be configured via drush as follows:
	
	drush vset gs_multiupload_maxfiles 15
	
- Important note: the upload page can only be used in an overlay. 

