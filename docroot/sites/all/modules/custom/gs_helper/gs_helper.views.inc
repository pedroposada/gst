<?php

/**
 * Implements hook_views_query_alter()
 */
function gs_helper_views_query_alter(&$view, &$query) {
  $views_to_remove_cartoons = array(
    'name' => array('nodequeue_19'),
    'current_display' => array('panel_pane_5', 'panel_pane_11'),
  );
  
  if (in_array($view->name, $views_to_remove_cartoons['name']) && in_array($view->current_display, $views_to_remove_cartoons['current_display'])) {
    if (in_array('Cartoons', $view-args)) {
      return;
    }
    
    $cartoon_terms = array(588, 643, 644, 641, 646, 645, 642);
    
    $join = new views_join();
    $join->construct('taxonomy_index', 'node', 'nid', 'nid');
    
    $query->table_queue['taxonomy_index'] = array(
      'table' => 'taxonomy_index',
      'num' => 1,
      'alias' => 'taxonomy_index',
      'join' => $join,
      'relationship' => 'node',
    );
    
    $query->add_where(1, 'taxonomy_index.tid', $cartoon_terms, 'NOT IN');
  }
}

/**
 * Implements hook_views_post_render(&$view, &$output, &$cache)
 */
function gs_helper_views_post_render(&$view, &$output, &$cache) {
  if ($view->name == "homepage_listings" && $view->current_display == "block_2") {
    $pat = "#(<div class=\".+field-name-gs-helper-node-thumbnail-universal.+><a.+><img.+>)(<\/a><\/div>)#";
    $rep = '$1<span class="icon"></span>$2';
    $output = preg_replace($pat, $rep, $output);
  }
}
