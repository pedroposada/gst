<?php
/**
 * @file
 * Contains code specific to the Panels module.
 */


/**
 * Pre-process panels
 */
function gs_helper_panels_pre_render(&$display, $renderer) {
  // set arguments for homepage blocks
  if (drupal_is_front_page()) {  
    // news block
      _gs_helper_set_homepage_listing_view($display, 6, GS_TID_NEWS);
    
    // music block
      _gs_helper_set_homepage_listing_view($display, 7, GS_TID_MUSIC);
    
    // arts block
      _gs_helper_set_homepage_listing_view($display, 8, GS_TID_ARTS);
    
    // movies block
      _gs_helper_set_homepage_listing_view($display, 9, GS_TID_MOVIES);
    
    // food block
      _gs_helper_set_homepage_listing_view($display, 10, GS_TID_FOOD);
    
    // life block
      _gs_helper_set_homepage_listing_view($display, 11, GS_TID_LIFE);
  }
  
  // set arguments for a bunch of views
  foreach ($display->content as $item) {
    if ($item->type == 'views') {
      // Arts view
      if ($item->subtype == 'arts') {
        // Arts News block
        if ($item->configuration['display'] == 'arts_news') {
          $item->configuration['args'] = GS_TID_ARTS_NOTES;
        }
        // Arts Reviews block
        elseif ($item->configuration['display'] == 'arts_reviews') {
          $item->configuration['args'] = GS_TID_ARTS_REVIEWS;
        }
      }
    }
  }
}

function _gs_helper_set_homepage_listing_view(&$display, $id, $tid) {
  if (isset($display->content[$id]) && $display->content[$id]->type == 'views' && $display->content[$id]->subtype == 'homepage_listings') {
    $display->content[$id]->configuration['args'] = $tid;
  }
}

