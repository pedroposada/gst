This module provides alterations to the user pages.


Instructions:
-------------
- Enable the module as usual, admin/modules "GS Profile".



Notes:
------
- Module creates one new page "user/UID/notification" where UID the is the user id from the user being viewed.

- It creates tabs for the following pages:
	
	user/* 					View (View User Profile). Panels page called "View Profile". This panels page has
							multiple panes that are configured to hide and show depending on the user's roles.
	
	user/*/edit				Account (Edit User Account). Panels page called "Edit Account".
	user/*/edit/profile		Profile (Edit User Profile). Default Profile2 page
	user/*/notification		Notificactions (Edit User Notifications). Default page created by this module.
	
  This tabs appear under the above paths only. 

- There are three Contexts that handle the blocks on the right hand side of the pages as follows:
	
	user_notification_editor
	user_profile_anon
	user_profile_editor
		
- All alterations are done in the module. There is no manipulation done in template.php for these pages.