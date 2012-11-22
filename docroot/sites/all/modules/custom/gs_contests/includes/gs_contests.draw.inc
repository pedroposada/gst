<?php

/**
 * Form to configure the download of CSV files.
 */
function contest_draw_form($form, &$form_state, $node) {
  module_load_include('inc', 'webform', 'includes/webform.export');
  module_load_include('inc', 'webform', 'includes/webform.components');
  
  $form_state['gs_contests']['qualified_draw_sids'] = gs_contests_qualified_draw_sids($node->nid);
  $form_state['gs_contests']['qualified_draw_count'] = count($form_state['gs_contests']['qualified_draw_sids']);
  
  $form = array();
  
  $form['node'] = array(
    '#type'  => 'value',
    '#value' => $node,
  );
  
  $form['draw'] = array(
    '#type'       => 'fieldset',
    '#title'      => t('Number of Entries to Draw'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['draw']['amount'] = array(
    '#type'          => 'textfield',
    '#size'          => 4,
    '#maxlength'     => 4,
    '#default_value' => 1,
    '#description'   => t('out of %count qualified draw entries.', array('%count' => $form_state['gs_contests']['qualified_draw_count'])),
  );
  
  $form['export_formats'] = array(
    '#type' => 'fieldset',
    '#title' => t('Export formats'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['export_formats']['format'] = array(
    '#type' => 'radios',
    '#title' => t('Export format'),
    '#options' => webform_export_list(),
    '#default_value' => isset($form_state['values']['format']) ? $form_state['values']['format'] : variable_get('webform_export_format', 'delimited'),
  );

  $form['export_formats']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimited text format'),
    '#description' => t('This is the delimiter used in the CSV/TSV file when downloading Webform results. Using tabs in the export is the most reliable method for preserving non-latin characters. You may want to change this to another character depending on the program with which you anticipate importing results.'),
    '#default_value' => isset($form_state['values']['delimiter']) ? $form_state['values']['delimiter'] : variable_get('webform_csv_delimiter', '\t'),
    '#options' => array(
      ','  => t('Comma (,)'),
      '\t' => t('Tab (\t)'),
      ';'  => t('Semicolon (;)'),
      ':'  => t('Colon (:)'),
      '|'  => t('Pipe (|)'),
      '.'  => t('Period (.)'),
      ' '  => t('Space ( )'),
    ),
  );

  $form['select_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select list options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['select_options']['select_keys'] = array(
    '#type' => 'radios',
    '#title' => t('Select keys'),
    '#options' => array(
      0 => t('Full, human-readable options (values)'),
      1 => t('Short, raw options (keys)'),
    ),
    '#default_value' => isset($form_state['values']['select_options']['select_keys']) ? $form_state['values']['select_options']['select_keys'] : 0,
    '#description' => t('Choose which part of options should be displayed from key|value pairs.'),
  );

  $form['select_options']['select_format'] = array(
    '#type' => 'radios',
    '#title' => t('Select list format'),
    '#options' => array(
      'separate' => t('Separate'),
      'compact' => t('Compact'),
    ),
    '#default_value' => isset($form_state['values']['select_options']['select_format']) ? $form_state['values']['select_options']['select_format'] : 'separate',
    '#attributes' => array('class' => array('webform-select-list-format')),
    '#theme' => 'webform_results_download_select_format',
  );

  $csv_components = array(
    'info' => t('Submission information'),
    'serial' => '-' . t('Submission Number'),
    'sid' => '-' . t('Submission ID'),
    'time' => '-' . t('Time'),
    'draft' => '-' . t('Draft'),
    'ip_address' => '-' . t('IP Address'),
    'uid' => '-' . t('User ID'),
    'username' => '-' . t('Username'),
  );
  $csv_components += webform_component_list($node, 'csv', TRUE);

  $form['components'] = array(
    '#type' => 'select',
    '#title' => t('Included export components'),
    '#options' => $csv_components,
    '#default_value' => isset($form_state['values']['components']) ? $form_state['values']['components'] : array_keys($csv_components),
    '#multiple' => TRUE,
    '#size' => 10,
    '#description' => t('The selected components will be included in the export.'),
    '#process' => array('webform_component_select'),
  );
  
  $form['download'] = array(
    '#type' => 'value',
    '#default_value' => TRUE,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Download'),
  );

  return $form;
}

/**
 * Validate handler for contest_draw_form().
 */
function contest_draw_form_validate(&$form, &$form_state) {
  $value = trim($form_state['values']['amount']);
  
  if (is_numeric($value)) {
    if (!is_int($value * 1)) {
      form_set_error('amount', t('"%amount" must be an integer.', array('%amount' => $value)));
    }
  }
  else {
    form_set_error('amount', t('"%amount" must be numeric.', array('%amount' => $value)));
  }
  
  if ($value > $form_state['gs_contests']['qualified_draw_count']) {
    form_set_error('amount', t('You cannot draw more than there are available entries.'));
  }
}

/**
 * Submit handler for contest_draw_form().
 */
function contest_draw_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'webform', 'includes/webform.report');
  
  $options = array(
    'delimiter' => $form_state['values']['delimiter'],
    'components' => array_keys(array_filter($form_state['values']['components'])),
    'select_keys' => $form_state['values']['select_keys'],
    'select_format' => $form_state['values']['select_format'],
    'range_type' => 'all',
    'download' => $form_state['values']['download'],
  );
  
  $options['sids'] = gs_contests_download_draw_sids($form_state['gs_contests']['qualified_draw_sids'], $form_state['values']['amount']);
  
  $export_info = webform_results_export($form_state['values']['node'], $form_state['values']['format'], $options);

  // If webform result file should be downloaded, send the file to the browser,
  // otherwise save information about the created file in $form_state.
  if ($options['download']) {
    webform_results_download($form_state['values']['node'], $export_info);
  }
  else {
    $form_state['export_info'] = $export_info;
  }
}

/**
 * Get SIDs of all qualified draw entries.
 */
function gs_contests_qualified_draw_sids($nid) {
  // Get all draw entries for contest.
  $result = db_select('gs_contests_entries', 'e')
    ->fields('e', array('sid', 'nid', 'data'))
    ->condition('nid', $nid)
    ->execute();
  
  $entries = array();
  
  foreach ($result as $submission) {
    $data = unserialize($submission->data);
    
    if (!empty($data)) {
      $db_or = db_or();
      
      // Clean up the blacklist of any expired blacklistees.
      contest_blacklist_prune();
      
      foreach($data as $key => $array){
        $db_or->condition($key , $array, 'IN');
      }
      
      // Check if the draw entry contains emails or phone numbers that are blacklisted.
      $query = db_select('gs_contests_blacklist', 'b')
        ->fields('b', array('bid'))
        ->condition($db_or)
        ->range(0, 1); // More efficient than using countQuery() for data checks.
      $count = $query->execute()->fetchField();
      
      // If blacklisted, do not store sid.
      if ($count) {
        continue;
      }
    }
    
    $entries[] = $submission->sid;
  }
  
  return $entries;
}

/**
 * Given the amount, retrieve a random set of SIDs for contest.
 */
function gs_contests_download_draw_sids($sids, $amount) {
  return array_rand(array_flip($sids), $amount);
}