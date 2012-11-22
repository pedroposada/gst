This module provides custom menu handlers and alterations all over the place
	

Instructions:
-------------
- Enable the module as usual, admin/modules "GS Helper".



Notes:
------
- Custom menu handlers:
  # "add/edit thumbnail" image cropper window 
  # nodequeues tab in workbench 
  # featured headlines tab in workbench
  # LGBT external links tab in workbench
  # hot topics in news tab in workbench
  # tech & games external links tab in workbench
  # store admin helper page tab in workbench
  # expire single page cache callback
  # toggle node status callback

- All homepage alterations are handled in this module. 
  All tabs in homepage are done in preprocess_page
  Moderation tab; list of revisions is altered in preprocess_table
  module.panels.inc is not used.
  
- Display Suite custom fields. See list of fields in module.display_suite.inc line 20. 

- Flickr API calls for flickr photo blocks lives in gs_helper.blocks.inc line 539
  drupal will cache the flicker images locally using 'imagecache_external' module.

- Nodequeues list page and autocomplete field alterations are handled in this module.

- Mapping of main navigation and taxonomy terms is handled in this module.

- Overlay forms for I Saw You and Confession are handled in this module.

- Twitter API call to post I Saw You's title and URL when published inside gs_helper.module

