<?php

/**
 * @file
 * Phone Number component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_phone_number() {
  return array(
    'name'      => '',
    'form_key'  => NULL,
    'mandatory' => 0,
    'pid'       => 0,
    'weight'    => 0,
    'extra'     => array(
      'title_display' => 0,
      'private'       => FALSE,
      'attributes'    => array(),
      'description'   => '',
      'unique'         => 0,
   ),
 );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_phone_number() {
  return array(
    'webform_display_phone_number' => array(
      'render element' => 'element',
   ),
 );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_phone_number($component) {
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_phone_number($component, $value = NULL, $filter = TRUE) {
  $form_item = array(
    '#type'             => 'textfield',
    '#default_value'    => $filter ? _webform_filter_values($component['value']) : $component['value'],
    '#attributes'       => $component['extra']['attributes'],
    '#theme_wrappers'   => array('webform_element'),
    '#description'      => $filter ? _webform_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#element_validate' => array('webform_validate_phone_number'),
    '#maxlength'        => 20,
    '#required'         => $component['mandatory'],
    '#size'             => 17,
    '#title'            => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display'    => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#weight'           => $component['weight'],
    '#translatable'     => array('title', 'description'),
 );
  
  // Enforce uniqueness.
  if ($component['extra']['unique']) {
    $element['#element_validate'][] = 'webform_validate_unique';
  }
  
  if (isset($value)) {
    $form_item['#default_value'] = $value[0];
  }
  
  return $form_item;
}

/**
 * Validation Callback for phone number field
 */
function webform_validate_phone_number(&$element, $form_state) {
  $phone_number = trim($element['#value']);
  
  if (!preg_match('/^(?:\+?1[-. ]?)?\(?(\d{3})\)?[-. ]?(\d{3})[-. ]?(\d{4})[-. ]?(?:(?:x|ext) ?(\d+))?$/i', $phone_number, $matches)) {
    form_error($element, t('"%value" is not a valid phone number. Please enter a valid ten-digit phone number with optional extension. Example: 604-555-5555 x123', array('%value' => $element['#value'])));
  }
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_phone_number($component, $value, $format = 'html') {
  return array(
    '#title'          => $component['name'],
    '#weight'         => $component['weight'],
    '#theme'          => 'webform_display_phone_number',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render'    => array('webform_element_wrapper'),
    '#component'      => $component,
    '#format'         => $format,
    '#value'          => isset($value[0]) ? $value[0] : '',
 );
}

/**
 * Format the output of data for this component.
 */
function theme_webform_display_phone_number($variables) {
  $markup = array();
  $element = $variables['element'];
  $plain_value = check_plain($element['#value']);
  $markup['#markup'] = $plain_value;
  $markup['#prefix'] = '<div class="field-item">';
  $markup['#suffix'] = '</div>';
  
  return drupal_render($markup);
}

/**
 * Implements _webform_submit_component().
 */
function _webform_submit_phone_number($component, $value) {
  if (isset($value) && !empty($value)) {
    preg_match('/^(?:\+?1[-. ]?)?\(?(\d{3})\)?[-. ]?(\d{3})[-. ]?(\d{4})[-. ]?(?:(?:x|ext) ?(\d+))?$/i', $value, $matches);
    $save_value = '(' . $matches[1] . ') ' . $matches[2] . '-' . $matches[3] . (isset($matches[4]) ? ' x' . $matches[4] : '');
  }
  else {
    $save_value = FALSE;
  }
  
  return $save_value;
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_phone_number($component, $sids = array(), $single = FALSE) {
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);
    
  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }
  
  $non_blanks = 0;
  $submissions = 0;
  
  $result = $query->execute();
  foreach ($result as $data) {
    if (drupal_strlen(trim($data['data'])) > 0) {
      $non_blanks++;
    }
    $submissions++;
  }
  
  $rows[0] = array(t('Left Blank'), ($submissions - $non_blanks));
  $rows[1] = array(t('User entered value'), $non_blanks);
  return $rows;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_phone_number($component, $value) {
  return check_plain(empty($value[0]) ? '' : $value[0]);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_phone_number($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_phone_number($component, $export_options, $value) {
  return !isset($value[0]) ? '' : $value[0];
}