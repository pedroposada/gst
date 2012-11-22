<?php

function gs_stats_views_data() {
  $data['gs_stats'] = array(
    'table' => array(
      'group' => 'Resume',
      'title' => 'skills',
      'join' => array(
        'node' => array(
          'left_field' => 'nid',
          'field' => 'nid',        
         ),
      ),
    ),
    'skills' => array(
      'title' => t('Skills'),
      'help' => t('Applicant Skills.'), // The help that appears on the UI,
      // Information for displaying the nid
      'field' => array(
        'handler' => 'views_handler_field_node',
        'click sortable' => TRUE,
      ),

    ),
  );

  return $data;  
}