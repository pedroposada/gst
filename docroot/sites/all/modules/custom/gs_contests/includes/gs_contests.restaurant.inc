<?php

/**
 * @file
 * Autocomplete component.
 */

// We treat textfield as a sort of "parent class"
module_load_include('inc', 'webform', 'components/textfield');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_restaurant() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'width' => '',
      'field_prefix' => '',
      'field_suffix' => '',
      'unique' => 0,
      'title_display' => 0,
      'description' => '',
      'attributes' => array(),
      'private' => FALSE,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_restaurant($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Default value'),
    '#default_value' => $component['value'],
    '#description' => t('The default value of the field.') . theme('webform_token_help'),
    '#size' => 60,
    '#maxlength' => 1024,
    '#weight' => 0,
  );
  $form['display']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $component['extra']['width'],
    '#description' => t('Width of the textfield.') . ' ' . t('Leaving blank will use the default size.'),
    '#size' => 5,
    '#maxlength' => 10,
    '#weight' => 0,
    '#parents' => array('extra', 'width'),
  );
  $form['display']['field_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Label placed to the left of the textfield'),
    '#default_value' => $component['extra']['field_prefix'],
    '#description' => t('Examples: $, #, -.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 1.1,
    '#parents' => array('extra', 'field_prefix'),
  );
  $form['display']['field_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Label placed to the right of the textfield'),
    '#default_value' => $component['extra']['field_suffix'],
    '#description' => t('Examples: lb, kg, %.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 1.2,
    '#parents' => array('extra', 'field_suffix'),
  );
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_restaurant($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $element = array(
    '#type' => 'textfield',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#default_value' => $filter ? _webform_filter_values($component['value'], $node, NULL, NULL, FALSE) : $component['value'],
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#field_prefix' => empty($component['extra']['field_prefix']) ? NULL : ($filter ? _webform_filter_xss($component['extra']['field_prefix']) : $component['extra']['field_prefix']),
    '#field_suffix' => empty($component['extra']['field_suffix']) ? NULL : ($filter ? _webform_filter_xss($component['extra']['field_suffix']) : $component['extra']['field_suffix']),
    '#description' => $filter ? _webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
    '#attributes' => $component['extra']['attributes'],
    '#theme_wrappers' => array('webform_element'),
    '#translatable' => array('title', 'description', 'field_prefix', 'field_suffix'),
  );
  // Autocomplete callback
  if ($node && !empty($component['cid'])) {
    $element['#autocomplete_path'] = 'webform_restaurant/js/' . $node->nid . '/' . $component['cid'];
  }

  // Enforce uniqueness.
  if ($component['extra']['unique']) {
    $element['#element_validate'][] = 'webform_validate_unique';
  }

  // Change the 'width' option to the correct 'size' option.
  if ($component['extra']['width'] > 0) {
    $element['#size'] = $component['extra']['width'];
  }

  if (isset($value)) {
    $element['#default_value'] = $value[0];
  }

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_restaurant($component, $value, $format = 'html') {
  return _webform_display_textfield($component, $value, $format);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_restaurant($component, $sids = array()) {
  return _webform_analysis_textfield($component, $sids);
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_restaurant($component, $value) {
  return _webform_table_textfield($component, $value);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_restaurant($component, $export_options) {
  return _webform_csv_headers_textfield($component, $export_options);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_restaurant($component, $export_options, $value) {
  return _webform_csv_data_textfield($component, $export_options, $value);
}

/**
 * Autocomplete callback.
 */
function webform_restaurant_js($node, $cid, $str = '') {
  if ($str) {
    $component = $node->webform['components'][$cid];
    $results = array();
    
    try {
      Database::addConnectionInfo('d6', 'default', $GLOBALS['databases']['d6']['default']);
      
      $query = Database::getConnection('default', 'd6')->select('timeout_restaurants', 'r')
        ->distinct()
        ->fields('r')
        ->condition('r.timeout_restaurant_name', '%' . db_like($str) . '%', 'LIKE')
        ->range(0, 10)
        ->execute();
        
      foreach ($query->fetchAll() as $row) {
        $results[$row->timeout_restaurant_name] = $row->timeout_restaurant_name;
      }
    }
    catch (Exception $e) {
      watchdog_exception('gs_contests', $e);
    }
    
    // Sort php and sql results together
    natcasesort($results);
    drupal_json_output($results);
  }
  exit();
}
