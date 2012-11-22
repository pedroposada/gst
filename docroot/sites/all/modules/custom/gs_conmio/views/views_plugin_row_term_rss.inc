<?php

/**
 * @file
 * Contains the node RSS row style plugin.
 */

/**
 * Plugin which formats the terms as RSS items.
 */
class views_plugin_row_term_rss extends views_plugin_row {
  var $base_table = 'taxonomy_term_data';
  var $base_field = 'tid';
  
  function pre_render($result) {
    $tids = array();
    foreach ($result as $row) {
      $tids[] = $row->{$this->field_alias};
    }
    if (!empty($tids)) {
      $this->terms = taxonomy_term_load_multiple($tids);
    }
  }
  
  function render($row) {
    $tid = $row->{$this->field_alias};
    if (!is_numeric($tid)) {
      return;
    }
    
    $term = $this->terms[$tid];
    $uri = entity_uri('taxonomy_term', $term);
    
    // Set title, description, and link to avoid having to write own theme handler for rss output.
    $row->description = $term->description;
    $row->title = $term->name;
    $row->link = url($uri['path'], $uri['options'] + array('absolute' => TRUE));
    
    return theme($this->theme_functions(),
      array(
        'view' => $this->view,
        'options' => $this->options,
        'row' => $row,
        'field_alias' => isset($this->field_alias) ? $this->field_alias : '',
      ));
  }
}